#include "userprofile.h"

UserProfile::UserProfile(QObject *parent)
    : QObject{parent}
{
    _uuid = QUuid{};
}

QString UserProfile::name()
{
    return _name;
}

void UserProfile::setName(const QString &value)
{
    _name = value;
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

void UserProfile::setTypeOperation(const QString &value)
{
    _type_of_operation = value;
}

QString UserProfile::typeOperation()
{
    return _type_of_operation;
}

void UserProfile::setUuid(const QUuid &value)
{
    _uuid = value;
}

QUuid UserProfile::uuid()
{
    return _uuid;
}

QString UserProfile::certToString() {
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
