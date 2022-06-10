#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <QObject>
#include <QJsonObject>
#include "converter.h"

class Certificate : public QObject
{
    Q_OBJECT
public:
    explicit Certificate(QObject *parent = nullptr);

    void setSourceObject(const QJsonObject& obj);

    bool isValid();
    QString subject() const;
    QString issuer() const;
    QString container() const;
    QString notValidBefore() const;
    QString notValidAfter() const;
    QString parentUser() const;
    QString serial() const;
    ByteArray data();
    void setData(const ByteArray& dt);

    QString getParam(const QString &key, const QString& name);

    void fromData(ByteArray &cer);

private:
    QJsonObject sourceObject;
    QString _subject;
    QString _issuer;
    QString _container;
    QString _notValidBefore;
    QString _notValidAfter;
    QString _parentUser;
    QString _serial;
    ByteArray _data;
    bool _isValid;

    QString getStringValue(int start, const QString& source);

signals:

};

#endif // CERTIFICATE_H