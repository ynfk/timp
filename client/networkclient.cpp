#include "networkclient.h"
#include "../crypto/tripledes.h"
#include <QDebug>

static TripleDES s_clientTdes;

NetworkClient::NetworkClient() : QObject(nullptr) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    s_clientTdes.loadFixedKey();
}

NetworkClient::~NetworkClient() {
    if (m_socket->state() == QTcpSocket::ConnectedState) m_socket->disconnectFromHost();
}

NetworkClient& NetworkClient::instance() {
    static NetworkClient instance;
    return instance;
}

void NetworkClient::connectToServer(const QString& host, quint16 port) {
    if (m_socket->state() == QTcpSocket::ConnectedState) return;
    m_socket->connectToHost(host, port);
}

void NetworkClient::disconnect() {
    if (m_socket->state() == QTcpSocket::ConnectedState) m_socket->disconnectFromHost();
}

bool NetworkClient::isConnected() const {
    return m_socket->state() == QTcpSocket::ConnectedState;
}

void NetworkClient::sendCommand(const QString& command) {
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        emit messageReceived("ERR: Not connected to server");
        return;
    }
    QString cmd = command.trimmed();
    if (!cmd.endsWith('\n')) cmd += '\n';
    m_socket->write(cmd.toUtf8());
    m_socket->flush();
}

void NetworkClient::sendEncryptedAuth(const QString& login, const QString& password) {
    QString encrypted = s_clientTdes.encrypt(password);
    sendCommand(QString("auth&%1,%2").arg(login, encrypted));
}

void NetworkClient::sendEncryptedRegister(const QString& login, const QString& password) {
    QString encrypted = s_clientTdes.encrypt(password);
    sendCommand(QString("reg&%1,%2").arg(login, encrypted));
}

void NetworkClient::onConnected() { emit connected(); }
void NetworkClient::onDisconnected() { emit disconnected(); }
void NetworkClient::onError(QAbstractSocket::SocketError) { emit error(m_socket->errorString()); }
void NetworkClient::onReadyRead() {
    m_buffer.append(m_socket->readAll());
    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx + 1);
        m_buffer.remove(0, idx + 1);
        emit messageReceived(QString::fromUtf8(line).trimmed());
    }
}
