#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client/networkclient.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isAuthenticated(false)
{
    ui->setupUi(this);
    setupUI();

    auto& client = NetworkClient::instance();
    connect(&client, &NetworkClient::connected, this, &MainWindow::onNetworkConnected);
    connect(&client, &NetworkClient::disconnected, this, &MainWindow::onNetworkDisconnected);
    connect(&client, &NetworkClient::error, this, &MainWindow::onNetworkError);
    connect(&client, &NetworkClient::messageReceived, this, &MainWindow::onNetworkMessage);

    client.connectToServer();
}

MainWindow::~MainWindow() {
    NetworkClient::instance().disconnect();
    delete ui;
}

void MainWindow::setupUI() {
    m_historyTable = ui->tableHistory;

    int adminIndex = ui->tabWidget->indexOf(ui->tabAdmin);
    int historyIndex = ui->tabWidget->indexOf(ui->tabHistory);
    ui->tabWidget->setTabVisible(adminIndex, false);
    ui->tabWidget->setTabVisible(historyIndex, false);

    showLoginPage();

    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(ui->btnRegister, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(ui->btnBackToLogin, &QPushButton::clicked, this, &MainWindow::onBackToLoginClicked);
    connect(ui->btnSubmitRegister, &QPushButton::clicked, this, &MainWindow::onSubmitRegisterClicked);
    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(ui->btnSha256, &QPushButton::clicked, this, &MainWindow::onSha256Clicked);
    connect(ui->btnSplineInterpolate, &QPushButton::clicked, this, &MainWindow::onSplineClicked);
    connect(ui->btnGradientDescent, &QPushButton::clicked, this, &MainWindow::onGdClicked);
    connect(ui->btnListUsers, &QPushButton::clicked, this, &MainWindow::onUsersClicked);
    connect(ui->btnBrowseSplineInput, &QPushButton::clicked, this, &MainWindow::onBrowseSplineInputClicked);
    connect(ui->btnBrowseSplineOutput, &QPushButton::clicked, this, &MainWindow::onBrowseSplineOutputClicked);
}

void MainWindow::showLoginPage() {
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditLogin->clear();
    ui->lineEditPassword->clear();
}

void MainWindow::showRegisterPage() {
    ui->stackedWidget->setCurrentIndex(1);
    ui->lineEditRegLogin->clear();
    ui->lineEditRegPassword->clear();
    ui->lineEditRegConfirm->clear();
}

void MainWindow::showMainPage() {
    ui->stackedWidget->setCurrentIndex(2);
    updateUserInfo();
}

void MainWindow::updateUserInfo() {
    ui->labelCurrentUser->setText(QString("%1 (%2)").arg(m_currentUser, m_currentRole));

    bool isAdmin = (m_currentRole == "admin");
    int adminIndex = ui->tabWidget->indexOf(ui->tabAdmin);
    int historyIndex = ui->tabWidget->indexOf(ui->tabHistory);
    ui->tabWidget->setTabVisible(adminIndex, isAdmin);
    ui->tabWidget->setTabVisible(historyIndex, true);
}

void MainWindow::addToHistory(const QString& action, const QString& result) {
    if (!m_historyTable) return;

    int row = m_historyTable->rowCount();
    m_historyTable->insertRow(row);
    m_historyTable->setItem(row, 0, new QTableWidgetItem(getCurrentTimestamp()));
    m_historyTable->setItem(row, 1, new QTableWidgetItem(action));
    m_historyTable->setItem(row, 2, new QTableWidgetItem(result));
    m_historyTable->scrollToBottom();
}

QString MainWindow::getCurrentTimestamp() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

void MainWindow::onLoginClicked() {
    QString login = ui->lineEditLogin->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter login and password");
        return;
    }

    NetworkClient::instance().sendEncryptedAuth(login, password);
}

void MainWindow::onRegisterClicked() {
    showRegisterPage();
}

void MainWindow::onBackToLoginClicked() {
    showLoginPage();
}

void MainWindow::onSubmitRegisterClicked() {
    QString login = ui->lineEditRegLogin->text().trimmed();
    QString password = ui->lineEditRegPassword->text().trimmed();
    QString confirm = ui->lineEditRegConfirm->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Fill all fields");
        return;
    }

    if (password != confirm) {
        QMessageBox::warning(this, "Error", "Passwords do not match");
        return;
    }

    if (password.length() < 4) {
        QMessageBox::warning(this, "Error", "Password must be at least 4 characters");
        return;
    }

    NetworkClient::instance().sendEncryptedRegister(login, password);
}

void MainWindow::onLogoutClicked() {
    m_isAuthenticated = false;
    m_currentUser.clear();
    m_currentRole.clear();
    showLoginPage();
    addToHistory("Logout", "Session ended");
}

void MainWindow::onSha256Clicked() {
    if (!m_isAuthenticated) {
        QMessageBox::warning(this, "Error", "Please login first");
        showLoginPage();
        return;
    }

    QString text = ui->textEditShaInput->toPlainText().trimmed();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter text to hash");
        return;
    }

    addToHistory("SHA-256 request", "Text: " + text.left(30) + "...");
    NetworkClient::instance().sendCommand(QString("sha256&%1").arg(text));
}

void MainWindow::onSplineClicked() {
    if (!m_isAuthenticated) {
        QMessageBox::warning(this, "Error", "Please login first");
        showLoginPage();
        return;
    }

    QString inputFile = ui->lineEditSplineInput->text().trimmed();
    QString outputFile = ui->lineEditSplineOutput->text().trimmed();

    if (inputFile.isEmpty() || outputFile.isEmpty()) {
        QMessageBox::warning(this, "Error", "Select input and output files");
        return;
    }

    addToHistory("Spline interpolation", "Input: " + inputFile);
    NetworkClient::instance().sendCommand(QString("spline&%1,%2").arg(inputFile, outputFile));
}

void MainWindow::onGdClicked() {
    if (!m_isAuthenticated) {
        QMessageBox::warning(this, "Error", "Please login first");
        showLoginPage();
        return;
    }

    int funcId = ui->comboGDFunction->currentIndex();
    double startX = ui->doubleSpinBoxStartX->value();
    double lr = ui->doubleSpinBoxLearningRate->value();

    addToHistory("Gradient descent", "Function: " + QString::number(funcId));
    NetworkClient::instance().sendCommand(QString("gd&%1&%2&%3").arg(funcId).arg(startX).arg(lr));
}

void MainWindow::onUsersClicked() {
    if (!m_isAuthenticated) {
        QMessageBox::warning(this, "Error", "Please login first");
        showLoginPage();
        return;
    }

    if (m_currentRole != "admin") {
        QMessageBox::warning(this, "Error", "Admin only");
        return;
    }

    QString sortBy = ui->comboSortBy->currentText();
    if (sortBy == "login") sortBy = "login";
    if (sortBy == "role") sortBy = "role";

    addToHistory("User list request", "Sort by: " + sortBy);
    NetworkClient::instance().sendCommand(QString("users&%1").arg(sortBy));
}

void MainWindow::onBrowseSplineInputClicked() {
    QString file = QFileDialog::getOpenFileName(this, "Select CSV file", "", "CSV Files (*.csv)");
    if (!file.isEmpty()) {
        ui->lineEditSplineInput->setText(file);
    }
}

void MainWindow::onBrowseSplineOutputClicked() {
    QString file = QFileDialog::getSaveFileName(this, "Save CSV file", "", "CSV Files (*.csv)");
    if (!file.isEmpty()) {
        ui->lineEditSplineOutput->setText(file);
    }
}

void MainWindow::onNetworkConnected() {
    ui->labelConnectionStatus->setText("Connected to server");
    ui->labelConnectionStatus->setStyleSheet("color: green;");
    addToHistory("Connection", "Connected to server");
}

void MainWindow::onNetworkDisconnected() {
    m_isAuthenticated = false;
    ui->labelConnectionStatus->setText("Disconnected from server");
    ui->labelConnectionStatus->setStyleSheet("color: red;");
    showLoginPage();
    addToHistory("Connection", "Disconnected from server");
}

void MainWindow::onNetworkError(const QString& error) {
    addToHistory("Network error", error);
    QMessageBox::warning(this, "Network Error", error);
}

void MainWindow::onNetworkMessage(const QString& response) {
    ui->textEditOutput->append(response);

    if (response.startsWith("REG_OK:")) {
        addToHistory("Registration", response.mid(7).trimmed());
        QMessageBox::information(this, "Success", "Registration successful! Please login.");
        showLoginPage();
    }
    else if (response.startsWith("REG_ERR:")) {
        addToHistory("Registration error", response.mid(8).trimmed());
        QMessageBox::warning(this, "Registration Failed", response);
    }
    else if (response.startsWith("AUTH_OK:")) {
        m_isAuthenticated = true;
        QString rest = response.mid(8).trimmed();

        int parenStart = rest.lastIndexOf('(');
        int parenEnd = rest.lastIndexOf(')');

        if (parenStart != -1 && parenEnd != -1) {
            m_currentUser = rest.left(parenStart).trimmed();
            m_currentRole = rest.mid(parenStart + 1, parenEnd - parenStart - 1);
        } else {
            m_currentUser = rest;
            m_currentRole = "user";
        }

        addToHistory("Authentication", m_currentUser + " (" + m_currentRole + ")");
        showMainPage();
    }
    else if (response.startsWith("AUTH_ERR:")) {
        addToHistory("Authentication error", response.mid(9).trimmed());
        QMessageBox::warning(this, "Login Failed", response);
    }
    else if (response.startsWith("SHA256_OK:")) {
        QString hash = response.mid(10).trimmed();
        ui->textEditShaOutput->setText(hash);
        addToHistory("SHA-256", "Hash: " + hash.left(30) + "...");
    }
    else if (response.startsWith("SPLINE_OK:")) {
        addToHistory("Spline", response.mid(10).trimmed());
        QMessageBox::information(this, "Spline Interpolation", "Completed successfully!");
    }
    else if (response.startsWith("SPLINE_ERR:")) {
        addToHistory("Spline error", response.mid(11).trimmed());
        QMessageBox::warning(this, "Spline Error", response);
    }
    else if (response.startsWith("GD_OK:")) {
        ui->textEditGDOutput->setText(response.mid(6).trimmed());
        addToHistory("Gradient Descent", response.mid(6).trimmed().left(50));
    }
    else if (response.startsWith("USERS_OK:")) {
        QString users = response.mid(9).trimmed();
        ui->textEditUsersList->setText(users.replace(", ", "\n"));
        addToHistory("User list", "Loaded " + QString::number(users.split(", ").size()) + " users");
    }
    else if (response.startsWith("USERS_ERR:")) {
        addToHistory("Error", response.mid(10).trimmed());
        QMessageBox::warning(this, "Error", response);
    }
    else if (response.startsWith("ERR:")) {
        addToHistory("Error", response.mid(4).trimmed());
    }
}
