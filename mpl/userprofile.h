#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QObject>
#include <QList>
#include <QUuid>
#include <QJsonObject>
#include <QIcon>

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

    QList<QUuid>& cerificates();
    void setSertificates(const QList<QUuid>& certs);

//    void setTypeOperation(const QString& value);
//    QString typeOperation();

    void setUuid(const QUuid& value);
    QUuid uuid();

    QString certsUuidToString();
    bool isNew();

    QJsonObject to_modelObject();

    void setIcon(const QString& rcPath);

    QIcon icon();

private:
    QString _address;
    QString _name;
    QList<QUuid> _cerificates;
    //QString _type_of_operation;
    QUuid _uuid;
    QString _profile;
    QIcon _image;

    void join(const std::vector<std::string>& v, char c, std::string& s);

signals:

};

#endif // USERPROFILE_H
