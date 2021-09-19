#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QCryptographicHash>

namespace arc_json{

    static QString get_hash(const QString& source){

        return QString("%1").arg(QString(QCryptographicHash::hash(source.toUtf8(),QCryptographicHash::Sha1).toHex()));

    }

}


#endif // GLOBAL_H
