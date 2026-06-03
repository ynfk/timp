#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onBackToLoginClicked();
    void onSubmitRegisterClicked();
    void onLogoutClicked();
    
    void onSha256Clicked();
    void onSplineClicked();
    void onGdClicked();
    void onUsersClicked();
    
    void onBrowseSplineInputClicked();
    void onBrowseSplineOutputClicked();
    
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkError(const QString& error);
    void onNetworkMessage(const QString& response);
    
private:
    void setupUI();
    void showLoginPage();
    void showRegisterPage();
    void showMainPage();
    void updateUserInfo();
    void addToHistory(const QString& action, const QString& result);
    QString getCurrentTimestamp();
    
    Ui::MainWindow* ui;
    QString m_currentUser;
    QString m_currentRole;
    bool m_isAuthenticated;
    QTableWidget* m_historyTable;
};

#endif
