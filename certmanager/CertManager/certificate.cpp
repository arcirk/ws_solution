#include "certificate.h"

Certificate::Certificate(QObject *parent)
    : QObject{parent}
{

}

void Certificate::setSourceObject(const QJsonObject &obj)
{
    sourceObject = obj;

    QString sSubj = obj.value("Subject").toString();
    int ind = sSubj.indexOf("CN=");
    if(ind != -1)
        _subject = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed().replace("\"", "");

    QString t;
    QString g;
    QString sn;
    ind = sSubj.indexOf("T=");
    if(ind != -1)
        t = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();


    ind = sSubj.indexOf("G=");
    if(ind != -1)
        g = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();


    ind = sSubj.indexOf("SN=");
    if(ind != -1)
        sn = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed();


    _parentUser = t + ", " + sn + " " + g;

    sSubj = obj.value("Issuer").toString();
    ind = sSubj.indexOf("CN=");
    if(ind != -1)
        _issuer = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed().replace("\"", "");



    sSubj = obj.value("Container").toString().replace("\r", "");
    _container = sSubj.trimmed();

    sSubj = obj.value("Not valid after").toString().replace("\r", "");
    _notValidAfter = sSubj.trimmed().replace("/", ".");

    sSubj = obj.value("Not valid before").toString().replace("\r", "");
    _notValidBefore = sSubj.trimmed().replace("/", ".");

    sSubj = obj.value("Serial").toString().replace("\r", "");
    _serial = sSubj.trimmed();
}

QString Certificate::subject() const
{
    return _subject;
}

QString Certificate::issuer() const
{
    return _issuer;
}

QString Certificate::container() const
{
    return _container;
}

QString Certificate::notValidBefore() const
{
    return _notValidBefore;
}

QString Certificate::notValidAfter() const
{
    return _notValidAfter;
}

QString Certificate::parentUser() const
{
    return _parentUser;
}

QString Certificate::serial() const
{
    return _serial;
}

QString Certificate::getStringValue(int start, const QString &source)
{
    for (int i = start; i < source.length(); ++i) {
        QString s = source.mid(i, 1);
        if(s == "," || s == "\n" || s == "\r" || i == source.length() - 1)
            return source.mid(start, i - start);
    }

    return "";
}
