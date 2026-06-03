#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class NetworkClient : public QObject {
    Q_OBJECT
    
public:
    static NetworkClient& instance();
    void connectToServer(const QString& host = "127.0.0.1", quint16 port = 33333);
    void disconnect();
    bool isConnected() const;
    void sendCommand(const QString& command);
    void sendEncryptedAuth(const QString& login, const QString& password);
    void sendEncryptedRegister(const QString& login, const QString& password);
    
signals:
    void connected();
    void disconnected();
    void error(const QString& message);
    void messageReceived(const QString& response);
    
private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    
private:
    NetworkClient();
    ~NetworkClient();
    NetworkClient(const NetworkClient&) = delete;
    NetworkClient& operator=(const NetworkClient&) = delete;
    
    QTcpSocket* m_socket;
    QByteArray m_buffer;
};

#endif
