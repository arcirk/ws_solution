#include "arcirk.h"

namespace arcirk {

QString get_sha1(const QByteArray& p_arg){
    auto sha = QCryptographicHash::hash(p_arg, QCryptographicHash::Sha1);
    return sha.toHex();
}

QString get_hash(const QString& first, const QString& second){
    QString _usr = first.trimmed();
    _usr = _usr.toUpper();
    return get_sha1(QString(_usr + second).toUtf8());
}

QUuid nil_uuid(){
    return {};
}

QString nil_string_uuid(){
    return QUuid().toString();
}


}
