#include "userprofile.h"
#include <QJsonArray>

UserProfile::UserProfile(QObject *parent)
    : QObject{parent}
{
    _uuid = QUuid{};
    _image = QIcon();
}

QString UserProfile::name()
{
    return _name;
}

void UserProfile::setName(const QString &value)
{
    _name = value;
}

QString UserProfile::profile()
{
    return _profile;
}

void UserProfile::setProfile(const QString &value)
{
    _profile = value;
}

QString UserProfile::defaultAddress()
{
    return _address;
}

void UserProfile::setDefaultAddress(const QString &value)
{
    _address = value;
}

QList<QUuid> UserProfile::serificates()
{
    return _serificates;
}

void UserProfile::setSertificates(const QList<QUuid> &certs)
{
    _serificates = certs;
}

//void UserProfile::setTypeOperation(const QString &value)
//{
//    _type_of_operation = value;
//}

//QString UserProfile::typeOperation()
//{
//    return _type_of_operation;
//}

void UserProfile::setUuid(const QUuid &value)
{
    _uuid = value;
}

QUuid UserProfile::uuid()
{
    return _uuid;
}

QString UserProfile::certsUuidToString() {
    if (_serificates.isEmpty())
        return "";

    QStringList lst;
    for (auto itr : _serificates) {
        lst.append(itr.toString());
    }

    return lst.join(",");
}

bool UserProfile::isNew()
{
    bool result = uuid() == QUuid{};

    return result;
}

QJsonObject UserProfile::to_modelObject()
{
    auto objMain = QJsonObject();
    objMain.insert("Empty", "");
    objMain.insert("name", name());
    objMain.insert("profile", profile());
    objMain.insert("address", defaultAddress());
    objMain.insert("uuid", uuid().toString());
    QStringList lst = {};
    for (auto cert : serificates()) {
        lst.append(cert.toString());
    }
    objMain.insert("certs", lst.join("/"));

    return objMain;
}

void UserProfile::setIcon(const QString &rcPath)
{
    _image = QIcon(rcPath);
}

QIcon UserProfile::icon()
{
    return _image;
}
