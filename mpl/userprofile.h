#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QObject>
#include <QList>
#include <QUuid>
#include <QJsonObject>

class UserProfile : public QObject
{
    Q_OBJECT
public:
    explicit UserProfile(QObject *parent = nullptr);

    QString name();
    void setName(const QString& value);

    QString profile();
    void setProfile(const QString& value);

    QString defaultAddress();
    void setDefaultAddress(const QString& value);

    QList<QUuid> serificates();
    void setSertificates(const QList<QUuid>& certs);

//    void setTypeOperation(const QString& value);
//    QString typeOperation();

    void setUuid(const QUuid& value);
    QUuid uuid();

    QString certsUuidToString();
    bool isNew();

    QJsonObject to_modelObject();

private:
    QString _address;
    QString _name;
    QList<QUuid> _serificates;
    //QString _type_of_operation;
    QUuid _uuid;
    QString _profile;

signals:

};

#endif // USERPROFILE_H
