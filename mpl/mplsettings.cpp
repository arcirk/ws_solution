#include "mplsettings.h"
#include <QJsonDocument>
#include <QDir>

enum launchMode{
    ws_srv = 0,
    mixed
};


QStringList bFields{
    "Password",
    "ServerHost",
    "ServerPort",
    "ServerUser",
    "charset",
    "customWsUser",
    "httpHost",
    "httpPwd",
    "httpUsr",
    "launch_mode",
    "method",
    "pwd",
    "server",
    "useSettingsFromHttp",
    "user",
    "mozillaExeFile",
    "profilesIniFile",
    "bindCertificates"
};

MplSettings::MplSettings(const QString& home, QObject *parent)
    : QObject{parent}
{
    _homeDir = home;

    QString fileName = QDir::toNativeSeparators(_homeDir + QDir::separator() + "mpl.json");

    m_vec.resize(bFields.size());

    if(QFile::exists(fileName))
        parse();
    else{
        m_vec[MplProfilesIniFile] = QDir::toNativeSeparators(QDir::homePath() + "/AppData/Roaming/Mozilla/Firefox/profiles.ini");
        m_vec[MplMozillaExeFile] = "C:/Program Files/Mozilla Firefox/firefox.exe";
        m_vec[MplHttpHost] = "http://192.168.10.12/trade/hs/http_trade";
        m_vec[MplCharset] = "CP866";
        m_vec[MplUseSettingsFromHttp] = false;
        m_vec[MplMethod] = 0;
        m_vec[MplCustomWsUser] = false;
        m_vec[Mpllaunch_mode] = ws_srv;
        m_vec[MplServerPort] = 8080;
        m_vec[MplServerHost] = "localhost";
        save();
    }
}

QVariant & MplSettings::operator[] (bMplSettings index) {
    return m_vec[index];
}

void MplSettings::save()
{
    QJsonDocument m_doc{};
    QJsonObject obj = QJsonObject();

    int fCount = bFields.size();

    for (int i = 0; i < fCount; ++i) {
        QString key = bFields[i];
        QVariant value = m_vec[i];
        if (value.userType() == QMetaType::QString)
            obj.insert(key, value.toString());
        else if (value.userType() == QMetaType::Int)
            obj.insert(key, value.toInt());
        else if (value.userType() == QMetaType::Bool)
            obj.insert(key, value.toBool());
        else
            obj.insert(key, "");
    }
    m_doc.setObject(obj);

    QString fileName = QDir::toNativeSeparators(_homeDir + QDir::separator() + "mpl.json");

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    jsonFile.write(QJsonDocument(m_doc).toJson(QJsonDocument::Indented));
    jsonFile.close();
}

bool MplSettings::parse()
{
    QString fileName = QDir::toNativeSeparators(_homeDir + QDir::separator() + "mpl.json");

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly)){
       return false;
    }
    QByteArray saveData = jsonFile.readAll();
    QJsonDocument m_doc(QJsonDocument::fromJson(saveData));

    if(!m_doc.isObject())
       return false;

    QJsonObject obj = m_doc.object();

    int fCount = bFields.size();

    for (int i = 0; i < fCount; ++i) {
       QString key = bFields[i];
       QJsonValue value = obj.value(key);
       QVariant val;
       if (value.isString())
           val = value.toString();
       else if (value.isDouble())
           val = value.toInt();
       else if (value.isBool())
           val = value.toBool();
       else
           val = "";
       m_vec[i] = val;
    }
    jsonFile.close();
    if(m_vec[MplCharset].toString().isEmpty())
        m_vec[MplCharset] = "CP866";
    if(m_vec[MplProfilesIniFile].toString().isEmpty())
        m_vec[MplProfilesIniFile] = QDir::toNativeSeparators(QDir::homePath() + "/AppData/Roaming/Mozilla/Firefox/profiles.ini");
    return true;
}

QString MplSettings::to_string() const
{
    QJsonDocument m_doc{};
    QJsonObject obj = QJsonObject();

    int fCount = bFields.size();

    for (int i = 0; i < fCount; ++i) {
        QString key = bFields[i];
        QVariant value = m_vec[i];
        if (value.userType() == QMetaType::QString)
            obj.insert(key, value.toString());
        else if (value.userType() == QMetaType::Int)
            obj.insert(key, value.toInt());
        else if (value.userType() == QMetaType::Bool)
            obj.insert(key, value.toBool());
        else
            obj.insert(key, "");
    }
    m_doc.setObject(obj);

    return QJsonDocument(obj).toJson(QJsonDocument::Indented);
}

QJsonObject MplSettings::to_object()
{
    QString fileName = QDir::toNativeSeparators(_homeDir + QDir::separator() + "mpl.json");

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly)){
        return {};
    }
    QByteArray saveData = jsonFile.readAll();
    QJsonDocument m_doc(QJsonDocument::fromJson(saveData));

    if(!m_doc.isObject())
        return {};

    return m_doc.object();
}

void MplSettings::fromObject(const QJsonObject &object)
{
    int fCount = bFields.size();

    for (int i = 0; i < fCount; ++i) {
       QString key = bFields[i];
       QJsonValue value = object.value(key);
       QVariant val;
       if (value.isString())
           val = value.toString();
       else if (value.isDouble())
           val = value.toInt();
       else if (value.isBool())
           val = value.toBool();
       else
           val = "";
       m_vec[i] = val;
    }

    if(m_vec[MplCharset].toString().isEmpty())
        m_vec[MplCharset] = "CP866";
    if(m_vec[MplProfilesIniFile].toString().isEmpty())
        m_vec[MplProfilesIniFile] = QDir::toNativeSeparators(QDir::homePath() + "/AppData/Roaming/Mozilla/Firefox/profiles.ini");
}

QVariant const &MplSettings::operator[](bMplSettings index) const {
    return m_vec[index];
}
