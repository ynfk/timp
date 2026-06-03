#include "tripledes.h"
#include <openssl/des.h>

static const QByteArray FIXED_KEY = QByteArray::fromHex("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");

TripleDES::TripleDES() { loadFixedKey(); }
void TripleDES::loadFixedKey() { m_key = FIXED_KEY; }

QByteArray TripleDES::pad(const QByteArray& data) {
    int padLen = 8 - (data.size() % 8);
    if (padLen == 0) padLen = 8;
    QByteArray padded = data;
    padded.append(padLen, static_cast<char>(padLen));
    return padded;
}

QByteArray TripleDES::unpad(const QByteArray& data) {
    if (data.isEmpty()) return data;
    int padLen = static_cast<unsigned char>(data[data.size() - 1]);
    if (padLen < 1 || padLen > 8) return data;
    return data.left(data.size() - padLen);
}

QByteArray TripleDES::encryptECB(const QByteArray& data) {
    QByteArray padded = pad(data);
    QByteArray result(padded.size(), 0);
    DES_key_schedule ks1, ks2, ks3;
    DES_cblock key1, key2, key3;
    memcpy(key1, m_key.data(), 8);
    memcpy(key2, m_key.data() + 8, 8);
    memcpy(key3, m_key.data() + 16, 8);
    DES_set_key_unchecked(&key1, &ks1);
    DES_set_key_unchecked(&key2, &ks2);
    DES_set_key_unchecked(&key3, &ks3);
    for (int i = 0; i < padded.size(); i += 8) {
        DES_cblock* input = reinterpret_cast<DES_cblock*>(const_cast<char*>(padded.data() + i));
        DES_cblock* output = reinterpret_cast<DES_cblock*>(result.data() + i);
        DES_ecb3_encrypt(input, output, &ks1, &ks2, &ks3, DES_ENCRYPT);
    }
    return result;
}

QByteArray TripleDES::decryptECB(const QByteArray& data) {
    if (data.size() % 8 != 0) return QByteArray();
    QByteArray result(data.size(), 0);
    DES_key_schedule ks1, ks2, ks3;
    DES_cblock key1, key2, key3;
    memcpy(key1, m_key.data(), 8);
    memcpy(key2, m_key.data() + 8, 8);
    memcpy(key3, m_key.data() + 16, 8);
    DES_set_key_unchecked(&key1, &ks1);
    DES_set_key_unchecked(&key2, &ks2);
    DES_set_key_unchecked(&key3, &ks3);
    for (int i = 0; i < data.size(); i += 8) {
        DES_cblock* input = reinterpret_cast<DES_cblock*>(const_cast<char*>(data.data() + i));
        DES_cblock* output = reinterpret_cast<DES_cblock*>(result.data() + i);
        DES_ecb3_encrypt(input, output, &ks1, &ks2, &ks3, DES_DECRYPT);
    }
    return unpad(result);
}

QString TripleDES::encrypt(const QString& plaintext) {
    return encryptECB(plaintext.toUtf8()).toBase64();
}

QString TripleDES::decrypt(const QString& ciphertext) {
    return QString::fromUtf8(decryptECB(QByteArray::fromBase64(ciphertext.toUtf8())));
}
