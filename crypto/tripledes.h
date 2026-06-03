#ifndef TRIPLEDES_H
#define TRIPLEDES_H

#include <QString>
#include <QByteArray>

class TripleDES {
public:
    TripleDES();
    void loadFixedKey();
    QString encrypt(const QString& plaintext);
    QString decrypt(const QString& ciphertext);
    
private:
    QByteArray m_key;
    QByteArray pad(const QByteArray& data);
    QByteArray unpad(const QByteArray& data);
    QByteArray encryptECB(const QByteArray& data);
    QByteArray decryptECB(const QByteArray& data);
};

#endif
