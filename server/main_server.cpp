#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QHash>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QPointF>
#include <cmath>
#include <algorithm>

// Подключаем готовые модули
#include "../crypto/sha256.h"
#include "../math/spline.h"
#include "../math/gradientdescent.h"

// ==============================================
// TCP СЕРВЕР
// ==============================================
class MyTcpServer : public QObject {
public:
    explicit MyTcpServer(QObject *parent = nullptr) : QObject(parent) {
        m_server = new QTcpServer(this);
        QObject::connect(m_server, &QTcpServer::newConnection, this, &MyTcpServer::onNewConnection);

        if (!m_server->listen(QHostAddress::Any, 33333)) {
            qCritical() << "[Server] Failed to start on port 33333";
        } else {
            qDebug() << "[Server] Started on port 33333";
        }
    }

public slots:
    void onNewConnection() {
        while (m_server->hasPendingConnections()) {
            QTcpSocket* socket = m_server->nextPendingConnection();
            m_buffers[socket] = QByteArray();
            m_roles[socket] = "";

            QObject::connect(socket, &QTcpSocket::readyRead, this, &MyTcpServer::onReadyRead);
            QObject::connect(socket, &QTcpSocket::disconnected, this, &MyTcpServer::onClientDisconnected);

            socket->write("CONNECTED: Server ready. Commands: reg&, auth&, sha256&, spline&, gd&, users&\r\n");
            qDebug() << "[Server] New client:" << socket->peerPort();
        }
    }

    void onReadyRead() {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) return;

        m_buffers[socket] += socket->readAll();

        int idx;
        while ((idx = m_buffers[socket].indexOf('\n')) != -1) {
            QByteArray line = m_buffers[socket].left(idx);
            m_buffers[socket].remove(0, idx + 1);

            QString command = QString::fromUtf8(line).trimmed();
            if (command.isEmpty()) continue;

            qDebug() << "[Server] Cmd from" << socket->peerPort() << ":" << command;

            QString response = handleCommand(command, m_roles[socket]);
            socket->write(response.toUtf8());
            socket->flush();
        }
    }

    void onClientDisconnected() {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket) return;

        qDebug() << "[Server] Client disconnected:" << socket->peerPort();
        m_buffers.remove(socket);
        m_roles.remove(socket);
        socket->deleteLater();
    }

private:
    QString handleCommand(const QString& cmd, QString& role) {
        QStringList parts = cmd.split('&');
        if (parts.isEmpty()) {
            return "ERR: Empty command\r\n";
        }

        QString action = parts[0].toLower();

        // Объединяем все остальные части обратно в payload
        QString payload;
        for (int i = 1; i < parts.size(); i++) {
            if (i > 1) payload += '&';
            payload += parts[i];
        }

        qDebug() << "[Server] Action:" << action << "Payload:" << payload;

        if (action == "reg") return handleRegister(payload);
        if (action == "auth") return handleAuth(payload, role);
        if (role.isEmpty()) return "ERR: Not authenticated\r\n";
        if (action == "sha256") return handleSha256(payload);
        if (action == "spline") return handleSpline(payload);
        if (action == "gd") return handleGradientDescent(payload);
        if (action == "users") return handleUsers(payload, role);
        return "ERR: Unknown command\r\n";
    }

    QString handleRegister(const QString& payload) {
        QStringList parts = payload.split(',');
        if (parts.size() != 2) return "REG_ERR: Invalid format. Use: reg&login,password\r\n";

        QString login = parts[0].trimmed();
        QString pass = parts[1].trimmed();

        if (login.isEmpty() || pass.isEmpty()) return "REG_ERR: Login or password empty\r\n";

        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT COUNT(*) FROM users WHERE login = :login");
        checkQuery.bindValue(":login", login);
        checkQuery.exec();

        if (checkQuery.next() && checkQuery.value(0).toInt() > 0) {
            return "REG_ERR: User already exists\r\n";
        }

        QString role = (login == "admin") ? "admin" : "user";
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (login, hash, role) VALUES (:login, :hash, :role)");
        insertQuery.bindValue(":login", login);
        insertQuery.bindValue(":hash", pass);
        insertQuery.bindValue(":role", role);

        if (insertQuery.exec()) {
            return "REG_OK: " + login + " registered as " + role + "\r\n";
        }
        return "REG_ERR: Database error\r\n";
    }

    QString handleAuth(const QString& payload, QString& role) {
        QStringList parts = payload.split(',');
        if (parts.size() != 2) return "AUTH_ERR: Invalid format. Use: auth&login,password\r\n";

        QString login = parts[0].trimmed();
        QString pass = parts[1].trimmed();

        QSqlQuery query;
        query.prepare("SELECT role FROM users WHERE login = :login AND hash = :hash");
        query.bindValue(":login", login);
        query.bindValue(":hash", pass);

        if (query.exec() && query.next()) {
            role = query.value(0).toString();
            return "AUTH_OK: " + login + " (" + role + ")\r\n";
        }
        return "AUTH_ERR: Invalid login or password\r\n";
    }

    QString handleSha256(const QString& payload) {
        if (payload.isEmpty()) return "SHA256_ERR: No input text\r\n";
        return "SHA256_OK: " + sha256(payload) + "\r\n";
    }

    QString handleSpline(const QString& payload) {
        qDebug() << "[Spline] Raw payload:" << payload;

        // Пробуем разделить по ',' (запятой)
        QStringList parts = payload.split(',');
        qDebug() << "[Spline] Parts after split:", parts;
        qDebug() << "[Spline] Parts count:", parts.size();

        if (parts.size() < 2) {
            return "SPLINE_ERR: Need input_file and output_file\r\n";
        }

        QString inputFile = parts[0].trimmed();
        QString outputFile = parts[1].trimmed();

        qDebug() << "[Spline] Input file:" << inputFile;
        qDebug() << "[Spline] Output file:" << outputFile;

        if (inputFile.isEmpty() || outputFile.isEmpty()) {
            return "SPLINE_ERR: File paths cannot be empty\r\n";
        }

        // Проверка существования файла
        QFile checkFile(inputFile);
        if (!checkFile.exists()) {
            return "SPLINE_ERR: Input file does not exist\r\n";
        }

        QVector<QPointF> points = loadPointsFromCSV(inputFile);
        qDebug() << "[Spline] Loaded points count:" << points.size();

        if (points.size() < 2) {
            return "SPLINE_ERR: Need at least 2 points in CSV\r\n";
        }

        SplineResult result = interpolateCubicSpline(points, 200);
        if (!result.success) {
            return "SPLINE_ERR: " + result.error + "\r\n";
        }

        if (!saveSplineToCSV(result, outputFile)) {
            return "SPLINE_ERR: Cannot save to " + outputFile + "\r\n";
        }

        return "SPLINE_OK: Interpolated " + QString::number(points.size()) +
               " points to " + QString::number(result.x.size()) + " points. Saved to " + outputFile + "\r\n";
    }

    QString handleGradientDescent(const QString& payload) {
        qDebug() << "[GD] Raw payload:" << payload;

        // Разделяем по '&' (как отправляет клиент)
        QStringList parts = payload.split('&');
        qDebug() << "[GD] Parts after split('&'):" << parts;

        // Если не сработало, пробуем по ','
        if (parts.size() < 2 && payload.contains(',')) {
            parts = payload.split(',');
            qDebug() << "[GD] Parts after split(','):" << parts;
        }

        if (parts.size() < 2) {
            return "GD_ERR: Use: gd&func_id&start_x&learning_rate (0=quadratic, 1=cubic)\r\n";
        }

        bool ok1, ok2;
        int funcId = parts[0].toInt(&ok1);
        double startX = parts[1].toDouble(&ok2);
        double lr = (parts.size() > 2) ? parts[2].toDouble() : 0.01;

        if (!ok1 || !ok2) {
            return "GD_ERR: Invalid parameters. Use: gd&0&5&0.01\r\n";
        }

        qDebug() << "[GD] funcId=" << funcId << "startX=" << startX << "lr=" << lr;

        double (*func)(double) = nullptr;
        double (*deriv)(double) = nullptr;
        QString funcName;

        if (funcId == 0) {
            func = funcQuadratic;
            deriv = derivQuadratic;
            funcName = "x^2";
        } else {
            func = funcCubic;
            deriv = derivCubic;
            funcName = "x^3 - 2x^2 + x";
        }

        GDResult result = gradientDescent(func, deriv, startX, lr, 1e-6, 1000);

        QString response = QString("GD_OK: f(x)=%1, min at x=%2, f(min)=%3, iterations=%4, converged=%5\r\n")
                               .arg(funcName)
                               .arg(result.xMin, 0, 'g', 8)
                               .arg(result.fMin, 0, 'g', 8)
                               .arg(result.iterations)
                               .arg(result.converged ? "yes" : "no");

        qDebug() << "[GD] Response:" << response;

        return response;
    }

    QString handleUsers(const QString& payload, const QString& role) {
        if (role != "admin") return "USERS_ERR: Admin only\r\n";

        QString sortBy = payload.trimmed().toLower();
        if (sortBy != "login" && sortBy != "role") sortBy = "login";

        QString order = (sortBy == "role") ? "role, login" : "login";
        QSqlQuery query("SELECT login, role FROM users ORDER BY " + order);

        QStringList users;
        while (query.next()) {
            users << query.value(0).toString() + ":" + query.value(1).toString();
        }

        if (users.isEmpty()) return "USERS_ERR: No users found\r\n";
        return "USERS_OK: " + users.join(", ") + "\r\n";
    }

    QTcpServer* m_server;
    QHash<QTcpSocket*, QByteArray> m_buffers;
    QHash<QTcpSocket*, QString> m_roles;
};

// ==============================================
// MAIN
// ==============================================
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("users.db");

    if (!db.open()) {
        qCritical() << "[Server] Cannot open database:" << db.lastError().text();
        return -1;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "login TEXT PRIMARY KEY, "
               "hash TEXT NOT NULL, "
               "role TEXT DEFAULT 'user')");

    query.exec("INSERT OR IGNORE INTO users VALUES ('admin', 'admin', 'admin')");

    qDebug() << "[Server] Database initialized";

    MyTcpServer server;
    qDebug() << "[Server] Listening on port 33333";

    return a.exec();
}
