#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <QObject>
#include <QJsonObject>
//#include <QDateTime>

class Certificate : public QObject
{
    Q_OBJECT
public:
    explicit Certificate(QObject *parent = nullptr);

    void setSourceObject(const QJsonObject& obj);

    QString subject() const;
    QString issuer() const;
    QString container() const;
    QString notValidBefore() const;
    QString notValidAfter() const;
    QString parentUser() const;
    QString serial() const;

private:
    QJsonObject sourceObject;
    QString _subject;
    QString _issuer;
    QString _container;
    QString _notValidBefore;
    QString _notValidAfter;
    QString _parentUser;
    QString _serial;

    QString getStringValue(int start, const QString& source);

signals:

};

#endif // CERTIFICATE_H
