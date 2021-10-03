#include "arcutils.h"

#include <QDebug>
#include <QByteArray>

ArcUtils::ArcUtils(QObject *parent) : QObject(parent)
{

}

QString ArcUtils::base64_decode(const QString &s)
{

    QByteArray data = QByteArray::fromBase64(s.toUtf8(), QByteArray::Base64Encoding);
    return data;


}
