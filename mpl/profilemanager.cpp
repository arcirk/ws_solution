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




    //getConf();
    //getSettings();
    //_bindCertificates = false;
    //_mozillaProfilesFile = mozillaProfilesFile();
}

//ProfileManager::ProfileManager(const QString &homePath, const QJsonObject &object, QObject *parent)
//    : QObject{parent}
//{

//    readProfiles();
//    _homePath = homePath;

//    _bindCertificates = false;

//    if(!object.isEmpty())
//        setCache(object);
//    else{
//       _mozillaProfilesFile = mozillaProfilesFile();
//       getConf();
//       getSettings();
//    }
//}

QMap<QUuid, UserProfile *> &ProfileManager::profiles()
{
    return _profiles;
}

MplSettings& ProfileManager::settings()
{
    return _settings;
}

//void ProfileManager::getSettings()
//{
//    QFile settings(_homePath + "/profiles.json");
//    if (!settings.open(QIODevice::ReadOnly))
//    {
//        return;
//    }
//    QJsonDocument doc = QJsonDocument::fromJson(settings.readAll());
//    settings.close();
//    QJsonArray arr = doc.array();

//    if(!arr.isEmpty()){
//        for (auto itr : arr) {
//            if(itr.isObject()){
//                auto item = itr.toObject();
//                auto profile = new UserProfile();
//                QString name = item.value("name").toString();
//                profile->setName(name);
//                QString _profile = item.value("profile").toString();
//                profile->setProfile(_profile);
//                QString address = item.value("address").toString();
//                profile->setDefaultAddress(address);
//                QUuid uuid = QUuid::fromString(item.value("uuid").toString());
//                profile->setUuid(uuid);
//                auto certs = item.value("certs");
//                if(certs.isArray()){
//                    auto arrLst = certs.toArray();
//                    QList<QUuid> lst;
//                    for(auto _uuid : arrLst){
//                        lst.append(QUuid::fromString(_uuid.toString()));
//                    }
//                    profile->setSertificates(lst);
//                }
//                _profiles.insert(uuid, profile);
//            }

//        }
//    }
//}

//void ProfileManager::saveSettings()
//{
//    QFile file(_homePath + "/profiles.json");
//    if (!file.open(QIODevice::WriteOnly))
//    {
//        return;
//    }

//    QJsonDocument doc = QJsonDocument();
//    QJsonArray arr = doc.array();

//    foreach (auto itr , _profiles) {
//        QJsonObject obj = QJsonObject();
//        obj.insert("name", itr->name());
//        obj.insert("profile", itr->profile());
//        obj.insert("address", itr->defaultAddress());
//        //obj.insert("typeOperation", itr->typeOperation());
//        obj.insert("uuid", itr->uuid().toString());
//        QJsonArray lst = QJsonArray();
//        for (auto cert : itr->serificates()) {
//            lst.append(cert.toString());
//        }
//        obj.insert("certs", lst);
//        arr.append(obj);
//    }

//    doc.setArray(arr);

//    file.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

//    file.close();
//}

//QString ProfileManager::mozillaProfilesFile()
//{
//    QFile conf(_homePath + "/mozilla.json");

//    QString _mozillaPDir = "";

//    if(conf.open(QIODevice::ReadOnly)){
//       QJsonDocument doc = QJsonDocument::fromJson(conf.readAll());
//       QJsonObject obj = doc.object();
//       if(!obj.empty()){
//           auto itr = obj.find("profilesPath");
//           if(itr->isString()){
//               _mozillaPDir = itr.value().toString();
//           }
//           itr = obj.find("mozillaExeFile");
//           if(itr->isString()){
//               _mozillaExeFile = itr.value().toString();
//           }
//           itr = obj.find("bindCertificates");
//           if(itr->isBool()){
//               _bindCertificates = itr.value().toBool();
//           }
//       }
//       conf.close();
//    }

//    if(!_mozillaPDir.isEmpty())
//        return _mozillaPDir;

//    QString appData = QDir::homePath();

//#ifndef Q_OS_LINUX
//    appData.append("/AppData/Roaming/Mozilla/Firefox/profiles.ini");
//#else
//    appData.append("/.mozilla/firefox/profiles.ini");
//#endif

//    qDebug() << QDir::fromNativeSeparators(appData);

//    if(!QFile::exists(QDir::fromNativeSeparators(appData))){
//#ifdef Q_OS_LINUX
//        if(QFile::exists(QDir::homePath() + QDir::separator() + "snap")){
//            appData = QDir::homePath()  + "/snap/firefox/common/.mozilla/firefox/profiles.ini";
//        }
//        if(!QFile::exists(appData))
//            return "";
//#else
//        return "";
//#endif
//    }

//    if(conf.open(QIODevice::WriteOnly)){
//        QJsonDocument doc = QJsonDocument();
//        QJsonObject obj = QJsonObject();
//        obj.insert("profilesPath", appData);
//        obj.insert("mozillaExeFile", _mozillaExeFile);
//        doc.setObject(obj);
//        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
//        conf.close();
//    }

//    return appData;

//}

//QString ProfileManager::mozillaExeFile()
//{
//    return _mozillaExeFile;
//}

//void ProfileManager::setMozillaExeFile(const QString &value)
//{
//    QFile conf(_homePath + "/mozilla.json");
//    _mozillaExeFile = value;
//    if(conf.open(QIODevice::WriteOnly)){
//        QJsonDocument doc = QJsonDocument();
//        QJsonObject obj = QJsonObject();
//        obj.insert("profilesPath", mozillaProfilesFile());
//        obj.insert("mozillaExeFile", _mozillaExeFile);
//        obj.insert("bindCertificates", _bindCertificates);
//        doc.setObject(obj);
//        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));
//        conf.close();
//    }

//}

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

void ProfileManager::setCache(const QJsonObject &resp)
{

    _settings.fromObject(resp.value("mpl").toObject());

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

//QJsonObject ProfileManager::cache()
//{
//    auto objMain = QJsonObject();
//    auto objmozilla = QJsonObject();
//    objmozilla.insert("profilesPath", mozillaProfilesFile());
//    objmozilla.insert("mozillaExeFile", _mozillaExeFile);
//    objmozilla.insert("bindCertificates", _bindCertificates);
//    objMain.insert("mozilla", objmozilla);


//    auto arr = QJsonArray();

//    foreach (auto itr , _profiles) {
//        QJsonObject obj = QJsonObject();
//        obj.insert("name", itr->name());
//        obj.insert("profile", itr->profile());
//        obj.insert("address", itr->defaultAddress());
//        //obj.insert("typeOperation", itr->typeOperation());
//        obj.insert("uuid", itr->uuid().toString());
//        QJsonArray lst = QJsonArray();
//        for (auto cert : itr->serificates()) {
//            lst.append(cert.toString());
//        }
//        obj.insert("certs", lst);
//        arr.append(obj);
//    }

//    objMain.insert("profiles", arr);

//    return objMain;
//}

//QJsonArray ProfileManager::profilesArray()
//{
//    auto arr = QJsonArray();

//    foreach (auto itr , _profiles) {
//        QJsonObject obj = QJsonObject();
//        obj.insert("name", itr->name());
//        obj.insert("profile", itr->profile());
//        obj.insert("address", itr->defaultAddress());
//        //obj.insert("typeOperation", itr->typeOperation());
//        obj.insert("uuid", itr->uuid().toString());
//        QJsonArray lst = QJsonArray();
//        for (auto cert : itr->serificates()) {
//            lst.append(cert.toString());
//        }
//        obj.insert("certs", lst);
//        arr.append(obj);
//    }

//    return arr;
//}

//void ProfileManager::setCache(const QJsonObject &value)
//{

//    auto itr = value.find("mozilla");
//    if(itr != value.end()){
//        auto obj = itr.value().toObject();
//        if(!obj.empty()){
//            itr = obj.find("profilesPath");
//            if(itr->isString()){
//                _mozillaProfilesFile = itr.value().toString();
//            }
//            itr = obj.find("mozillaExeFile");
//            if(itr->isString()){
//                _mozillaExeFile = itr.value().toString();
//            }
//            itr = obj.find("bindCertificates");
//            if(itr->isBool()){
//                _bindCertificates = itr.value().toBool();
//            }
//        }
//    }

//    itr = value.find("profiles");

//    if(itr != value.end()){

//        auto arr = itr.value().toArray();

//        if(!arr.isEmpty()){
//            for (auto itr : arr) {
//                if(itr.isObject()){
//                    auto item = itr.toObject();
//                    auto profile = new UserProfile();
//                    QString name = item.value("name").toString();
//                    profile->setName(name);
//                    QString _profile = item.value("profile").toString();
//                    profile->setProfile(_profile);
//                    QString address = item.value("address").toString();
//                    profile->setDefaultAddress(address);
//                    QUuid uuid = QUuid::fromString(item.value("uuid").toString());
//                    profile->setUuid(uuid);
//                    auto certs = item.value("certs");
//                    if(certs.isArray()){
//                        auto arrLst = certs.toArray();
//                        QList<QUuid> lst;
//                        for(auto _uuid : arrLst){
//                            lst.append(QUuid::fromString(_uuid.toString()));
//                        }
//                        profile->setSertificates(lst);
//                    }
//                    _profiles.insert(uuid, profile);
//                }

//            }
//        }
//    }
//}

QString ProfileManager::model()
{
    const QStringList _columns = {"Empty", "name", "profile", "address", "uuid", "cert"};
    auto objMain = QJsonObject();
    auto columns = QJsonArray();
    auto rows = QJsonArray();

    foreach(auto _col, _columns){
        columns.append(_col);
    }

    objMain.insert("columns", columns);

    foreach (auto itr , _profiles) {
        auto obj = itr->to_modelObject();
        rows.append(obj);
    }

    objMain.insert("rows", rows);

    return QJsonDocument(objMain).toJson();
}

QJsonObject ProfileManager::to_profiles_table()
{
    const QStringList _columns = {"Empty", "name", "profile", "address", "uuid", "cert"};
    auto objMain = QJsonObject();
    auto columns = QJsonArray();
    auto rows = QJsonArray();

    foreach(auto _col, _columns){
        columns.append(_col);
    }

    objMain.insert("columns", columns);

    foreach (auto itr , _profiles) {
        auto obj = itr->to_modelObject();
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

//bool ProfileManager::bindCertificates()
//{
//    return _bindCertificates;
//}

//void ProfileManager::setBindCertificates(bool value)
//{
//    _bindCertificates = value;
//}

//void ProfileManager::getConf()
//{
//    QFile conf(_homePath + "/mozilla.json");

//    if(conf.open(QIODevice::ReadOnly)){
//       QJsonDocument doc = QJsonDocument::fromJson(conf.readAll());
//       QJsonObject obj = doc.object();
//       if(!obj.empty()){
//           auto itr = obj.find("profilesPath");
//           if(itr->isString()){
//               _mozillaProfilesFile = itr.value().toString();
//           }
//           itr = obj.find("mozillaExeFile");
//           if(itr->isString()){
//               _mozillaExeFile = itr.value().toString();
//           }
//           itr = obj.find("bindCertificates");
//           if(itr->isBool()){
//               _bindCertificates = itr.value().toBool();
//           }
//       }
//       conf.close();
//    }
//}

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
