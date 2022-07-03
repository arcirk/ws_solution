#include "profilemanager.h"
#include <QFile>
#include <QDir>
#include <QSettings>

ProfileManager::ProfileManager(const QString& homePath, QObject *parent)
    : QObject{parent},
    _settings(homePath, this)
{

    _homePath = homePath;

    readProfiles();
}

std::unordered_map<QString, UserProfile *> &ProfileManager::profiles()
{
    return _profiles;
}

MplSettings& ProfileManager::settings()
{
    return _settings;
}

QStringList ProfileManager::profilesNames()
{
    return _profilesNames;
}

void ProfileManager::setProfile(UserProfile *prof)
{
    _profiles.emplace(prof->uuid().toString(), prof);
}

void ProfileManager::clear()
{
    //qDeleteAll(_profiles);
    _profiles.clear();
}

void ProfileManager::setCache(const QJsonObject &resp)
{

    _settings.fromObject(resp.value("mpl").toObject());

    //qDeleteAll(_profiles);
    _profiles.clear();

    QJsonArray arr = resp.value("profiles").toObject().value("rows").toArray();

    if(!arr.isEmpty()){
        for (auto itr : arr) {
            if(itr.isObject()){
                auto item = itr.toObject();
                auto profile = new UserProfile();
                QString name = item.value("name").toString();
                profile->setName(name);
                QString _profile = item.value("profile").toString();
                profile->setProfile(_profile);
                QString address = item.value("address").toString();
                profile->setDefaultAddress(address);
                QUuid uuid = QUuid::fromString(item.value("uuid").toString());
                profile->setUuid(uuid);
                auto certs = item.value("certs").toString().split("/");
                QList<QUuid> lst;
                for(auto _uuid : certs){
                    if(_uuid.isEmpty())
                        continue;
                    lst.append(QUuid::fromString(_uuid));
                }
                profile->setSertificates(lst);
                _profiles.emplace(uuid.toString(), profile);
            }

        }
    }
}

QString ProfileManager::model()
{
    const QStringList _columns = {"Empty", "name", "profile", "address", "uuid", "certs"};
    auto objMain = QJsonObject();
    auto columns = QJsonArray();
    auto rows = QJsonArray();

    foreach(auto _col, _columns){
        columns.append(_col);
    }

    objMain.insert("columns", columns);

    foreach (auto itr , _profiles) {
        auto obj = itr.second->to_modelObject();
        rows.append(obj);
    }

    objMain.insert("rows", rows);

    return QJsonDocument(objMain).toJson();
}

void ProfileManager::fromModel(const QString &modelText)
{
    //qDeleteAll(_profiles);
    _profiles.clear();
    QJsonDocument doc = QJsonDocument::fromJson(modelText.toUtf8());

    QJsonArray arr = doc.object().value("rows").toArray();

    if(!arr.isEmpty()){
        for (auto itr : arr) {
            if(itr.isObject()){
                auto item = itr.toObject();
                auto profile = new UserProfile();
                QString name = item.value("name").toString();
                profile->setName(name);
                QString _profile = item.value("profile").toString();
                profile->setProfile(_profile);
                QString address = item.value("address").toString();
                profile->setDefaultAddress(address);
                QUuid uuid = QUuid::fromString(item.value("uuid").toString());
                profile->setUuid(uuid);
                auto certs = item.value("certs").toString().split("/");
                QList<QUuid> lst;
                for(auto _uuid : certs){
                    if(_uuid.isEmpty())
                        continue;
                    lst.append(QUuid::fromString(_uuid));
                }
                profile->setSertificates(lst);
                _profiles.emplace(uuid.toString(), profile);
            }

        }
    }
}

QJsonObject ProfileManager::to_profiles_table()
{
    const QStringList _columns = {"Empty", "name", "profile", "address", "uuid", "certs"};
    auto objMain = QJsonObject();
    auto columns = QJsonArray();
    auto rows = QJsonArray();

    foreach(auto _col, _columns){
        columns.append(_col);
    }

    objMain.insert("columns", columns);

    foreach (auto itr , _profiles) {
        auto obj = itr.second->to_modelObject();
        rows.append(obj);
    }

    objMain.insert("rows", rows);

    return objMain;
}

void ProfileManager::save()
{
    _settings.save();
    auto objMain = QJsonObject();
    objMain.insert("mpl", _settings.to_object());
    objMain.insert("profiles", to_profiles_table());

    QString fileName = QDir::toNativeSeparators(_homePath + QDir::separator() + "profiles.json");

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    jsonFile.write(QJsonDocument(objMain).toJson(QJsonDocument::Indented));
    jsonFile.close();

}

QJsonObject ProfileManager::to_object()
{

    _settings.save();
    auto objMain = QJsonObject();
    objMain.insert("mpl", _settings.to_object());
    objMain.insert("profiles", to_profiles_table());

    return objMain;
}

void ProfileManager::readProfiles()
{
    QFile file(_settings[MplProfilesIniFile].toString());
    if(!file.exists())
        return;

    _profilesNames.clear();

    QSettings ini = QSettings(QDir::fromNativeSeparators(_settings[MplProfilesIniFile].toString()), QSettings::IniFormat);
    QStringList keys = ini.allKeys();
    foreach(const QString& key, keys){
        if(key.compare("Profile")){
            if(key.endsWith("/Name")){
                _profilesNames.append(ini.value(key).toString());
            }
        }
    }

    QFile settings(_homePath + "/profiles.json");
    if (!settings.open(QIODevice::ReadOnly))
    {
        return;
    }

    //qDeleteAll(_profiles);
    _profiles.clear();
    QJsonDocument doc = QJsonDocument::fromJson(settings.readAll());
    settings.close();
    QJsonArray arr = doc.object().value("profiles").toObject().value("rows").toArray();

    if(!arr.isEmpty()){
        for (auto itr : arr) {
            if(itr.isObject()){
                auto item = itr.toObject();
                auto profile = new UserProfile();
                QString name = item.value("name").toString();
                profile->setName(name);
                QString _profile = item.value("profile").toString();
                profile->setProfile(_profile);
                QString address = item.value("address").toString();
                profile->setDefaultAddress(address);
                QUuid uuid = QUuid::fromString(item.value("uuid").toString());
                profile->setUuid(uuid);
                auto certs = item.value("certs").toString().split("/");
                QList<QUuid> lst;
                for(auto _uuid : certs){
                    if(_uuid.isEmpty())
                        continue;
                    lst.append(QUuid::fromString(_uuid));
                }
                profile->setSertificates(lst);
                _profiles.emplace(uuid.toString(), profile);
            }

        }
    }
}
