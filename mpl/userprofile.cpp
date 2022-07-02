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

QList<QUuid>& UserProfile::cerificates()
{
    return _cerificates;
}

void UserProfile::setSertificates(const QList<QUuid> &certs)
{
    _cerificates = certs;
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

void UserProfile::join(const std::vector<std::string>& v, char c, std::string& s) {

   s.clear();

   for (std::vector<std::string>::const_iterator p = v.begin();
        p != v.end(); ++p) {
      s += *p;
      if (p != v.end() - 1)
        s += c;
   }
}

QString UserProfile::certsUuidToString() {

    if (_cerificates.isEmpty())
        return "";

    std::vector<std::string> lst;
    for (auto itr : _cerificates) {
        lst.push_back(itr.toString().toStdString());
    }

    std::string result;

   join(lst, ',', result);

   return QString::fromStdString(result);

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
    std::vector<std::string> lst;
    std::string result;
    for (auto cert : cerificates()) {
        lst.push_back(cert.toString().toStdString());
    }
    join(lst, '/', result);
    objMain.insert("certs", QString::fromStdString(result));

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
