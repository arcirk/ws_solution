#include "profilemanager.h"
#include <QFile>
#include <QDir>
#include <QSettings>

ProfileManager::ProfileManager(const QString& homePath, QObject *parent)
    : QObject{parent}
{
    _mozillaProfilesFile = mozillaProfilesFile();
    loadProfilesFromINI();
    _homePath = homePath;
    getConf();
    getSettings();
}

QMap<QUuid, UserProfile *> ProfileManager::profiles()
{
    return _profiles;
}

void ProfileManager::getSettings()
{
    QFile settings(_homePath + "/settings.json");
    if (!settings.open(QIODevice::ReadOnly))
    {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(settings.readAll());
    settings.close();
    QJsonArray arr = doc.array();

    if(!arr.isEmpty()){
        for (auto itr : arr) {
            if(itr.isObject()){
                auto item = itr.toObject();
                auto profile = new UserProfile();
                QString name = item.value("name").toString();
                profile->setName(name);
                QString address = item.value("address").toString();
                profile->setDefaultAddress(address);
                QString typeOperation = item.value("typeOperation").toString();
                profile->setTypeOperation(typeOperation);
                QUuid uuid = QUuid::fromString(item.value("uuid").toString());
                profile->setUuid(uuid);
                auto certs = item.value("certs");
                if(certs.isArray()){
                    auto arrLst = certs.toArray();
                    QList<QUuid> lst;
                    for(auto _uuid : arrLst){
                        lst.append(QUuid::fromString(_uuid.toString()));
                    }
                    profile->setSertificates(lst);
                }
                _profiles.insert(uuid, profile);
            }

        }
    }
}

void ProfileManager::saveSettings()
{
    QFile file(_homePath + "/settings.json");
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument doc = QJsonDocument();
    QJsonArray arr = doc.array();

    for (auto itr : _profiles) {
        QJsonObject obj = QJsonObject();
        obj.insert("name", itr->name());
        obj.insert("address", itr->defaultAddress());
        obj.insert("typeOperation", itr->typeOperation());
        obj.insert("uuid", itr->uuid().toString());
        QJsonArray lst = QJsonArray();
        for (auto cert : itr->serificates()) {
            lst.append(cert.toString());
        }
        obj.insert("certs", lst);
        arr.append(obj);
    }

    doc.setArray(arr);

    file.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

    file.close();
}

QString ProfileManager::mozillaProfilesFile()
{
    QFile conf(_homePath + "/conf.json");

    QString _mozillaPDir = "";

    if(conf.open(QIODevice::ReadOnly)){
       QJsonDocument doc = QJsonDocument::fromJson(conf.readAll());
       QJsonObject obj = doc.object();
       if(!obj.empty()){
           auto itr = obj.find("profilesPath");
           if(itr->isString()){
               _mozillaPDir = itr.value().toString();
           }
           itr = obj.find("mozillaExeFile");
           if(itr->isString()){
               _mozillaExeFile = itr.value().toString();
           }
       }
       conf.close();
    }

    if(!_mozillaPDir.isEmpty())
        return _mozillaPDir;

    QString appData = QDir::homePath();

#ifndef Q_OS_LINUX
    appData.append("/AppData/Roaming/Mozilla/Firefox/profiles.ini");
#else
    appData.append("/.mozilla/firefox/profiles.ini");
#endif

    qDebug() << QDir::fromNativeSeparators(appData);

    if(!QFile::exists(QDir::fromNativeSeparators(appData))){
#ifdef Q_OS_LINUX
        if(QFile::exists(QDir::homePath() + QDir::separator() + "snap")){
            appData = QDir::homePath()  + "/snap/firefox/common/.mozilla/firefox/profiles.ini";
        }
        if(!QFile::exists(appData))
            return "";
#else
        return "";
#endif
    }

    if(conf.open(QIODevice::WriteOnly)){
        QJsonDocument doc = QJsonDocument();
        QJsonObject obj = QJsonObject();
        obj.insert("profilesPath", appData);
        obj.insert("mozillaExeFile", _mozillaExeFile);
        doc.setObject(obj);
        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
        conf.close();
    }

    return appData;

}

QString ProfileManager::mozillaExeFile()
{
    return _mozillaExeFile;
}

void ProfileManager::setMozillaExeFile(const QString &value)
{
    QFile conf(_homePath + "/conf.json");
    _mozillaExeFile = value;
    if(conf.open(QIODevice::WriteOnly)){
        QJsonDocument doc = QJsonDocument();
        QJsonObject obj = QJsonObject();
        obj.insert("profilesPath", mozillaProfilesFile());
        obj.insert("mozillaExeFile", _mozillaExeFile);
        doc.setObject(obj);
        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
        conf.close();
    }
}

QStringList ProfileManager::profilesNames()
{
    return _profilesNames;
}

void ProfileManager::setProfile(UserProfile *prof)
{
    _profiles.insert(prof->uuid(), prof);
}

void ProfileManager::clear()
{
    _profiles.clear();
}

void ProfileManager::getConf()
{
    QFile conf(_homePath + "/conf.json");

    if(conf.open(QIODevice::ReadOnly)){
       QJsonDocument doc = QJsonDocument::fromJson(conf.readAll());
       QJsonObject obj = doc.object();
       if(!obj.empty()){
           auto itr = obj.find("profilesPath");
           if(itr->isString()){
               _mozillaProfilesFile = itr.value().toString();
           }
           itr = obj.find("mozillaExeFile");
           if(itr->isString()){
               _mozillaExeFile = itr.value().toString();
           }
       }
       conf.close();
    }
}

void ProfileManager::loadProfilesFromINI()
{
    QFile file(_mozillaProfilesFile);
    if(!file.exists())
        return;

    _profilesNames.clear();

    QSettings ini = QSettings(QDir::fromNativeSeparators(_mozillaProfilesFile), QSettings::IniFormat);
    QStringList keys = ini.allKeys();
    foreach(const QString& key, keys){
        if(key.compare("Profile")){
            if(key.endsWith("/Name")){
                _profilesNames.append(ini.value(key).toString());
            }

        }
    }
}
