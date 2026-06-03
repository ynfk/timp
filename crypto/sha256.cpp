#include "sha256.h"
#include <QCryptographicHash>

QString sha256(const QString& input) {
    return QString::fromLatin1(QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256).toHex());
}
