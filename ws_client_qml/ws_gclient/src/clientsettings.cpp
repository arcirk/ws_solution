//#include "../include/stdfx.h"
#include "../include/clientsettings.h"
#include <iws_client.h>

#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>

ClientSettings::ClientSettings()
    :ServerHost("localhost")
    , ServerPort(8080)
    , RootUser("admin")
    , ServerBinDir("host")
    , ServerName("NoName")
    , ServerStatus("Не подключен")
    , AppName("qt_client")

{
    Hash = QString::fromStdString(IClient::get_hash("admin", "admin"));
    fileName = "config.json";
    UseLocalWebDavDirectory = false;
    WebdavSSL = false;
}

bool ClientSettings::init(){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "config.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        ServerHost ="localhost";
        ServerPort = 8080;
        RootUser = "admin";
        ServerName = "NoName";
        AppName = "qt_client";
        AutoConnect = false;
        SaveHash = false;
        UseLocalWebDavDirectory = false;
        WebdavSSL = false;
      return false;
    }

    QByteArray saveData = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));

    m_currentJsonObject = jsonDocument.object();

    auto iter = m_currentJsonObject.find("ServerHost");
    if (iter != m_currentJsonObject.end()){
        ServerHost = iter.value().toString();
    }
    iter = m_currentJsonObject.find("ServerPort");
    if (iter != m_currentJsonObject.end()){
        ServerPort = iter.value().toInt();
    }
    iter = m_currentJsonObject.find("RootUser");
    if (iter != m_currentJsonObject.end()){
        RootUser = iter.value().toString();
    }
    iter = m_currentJsonObject.find("Hash");
    if (iter != m_currentJsonObject.end()){
        Hash = iter.value().toString();
    }
    iter = m_currentJsonObject.find("ServerBinDir");
    if (iter != m_currentJsonObject.end()){
        ServerBinDir = iter.value().toString();
    }
    iter = m_currentJsonObject.find("ServerName");
    if (iter != m_currentJsonObject.end()){
        ServerName = iter.value().toString();
    }
    iter = m_currentJsonObject.find("AutoConnect");
    if (iter != m_currentJsonObject.end()){
        AutoConnect = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("SaveHash");
    if (iter != m_currentJsonObject.end()){
        SaveHash = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("pathToClient");
    if (iter != m_currentJsonObject.end()){
        pathToClient = iter.value().toString();
    }
    iter = m_currentJsonObject.find("UseLocalWebDavDirectory");
    if (iter != m_currentJsonObject.end()){
        UseLocalWebDavDirectory = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("LocalWebDavDirectory");
    if (iter != m_currentJsonObject.end()){
        LocalWebDavDirectory = iter.value().toString();
    }
    iter = m_currentJsonObject.find("WebdavSSL");
    if (iter != m_currentJsonObject.end()){
        WebdavSSL = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("WebdavHost");
    if (iter != m_currentJsonObject.end()){
        WebdavHost = iter.value().toString();
    }
    iter = m_currentJsonObject.find("WebdavUser");
    if (iter != m_currentJsonObject.end()){
        WebdavUser = iter.value().toString();
    }
    iter = m_currentJsonObject.find("WebdavPwd");
    if (iter != m_currentJsonObject.end()){
        WebdavPwd = iter.value().toString();
    }
    AppName = "qt_client";

    return true;
}

void ClientSettings::save_settings(){

    if(!SaveHash)
        Hash = "";

    QJsonObject m_currentJsonObject = QJsonObject();
    m_currentJsonObject.insert("ServerHost", ServerHost);
    m_currentJsonObject.insert("ServerPort", ServerPort);
    m_currentJsonObject.insert("RootUser", RootUser);
    m_currentJsonObject.insert("Hash", Hash);
    m_currentJsonObject.insert("ServerBinDir", ServerBinDir);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("AppName", AppName);
    m_currentJsonObject.insert("pathToClient", pathToClient);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
    m_currentJsonObject.insert("UseLocalWebDavDirectory", UseLocalWebDavDirectory);
    m_currentJsonObject.insert("WebdavHost", WebdavHost);
    m_currentJsonObject.insert("WebdavUser", WebdavUser);
    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
    QString saveFileName = fileName;
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    // Записываем текущий объект Json в файл
    jsonFile.write(QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented));
    jsonFile.close();   // Закрываем файл
}

QString ClientSettings::getJson() {
    QJsonObject m_currentJsonObject = QJsonObject();
    m_currentJsonObject.insert("ServerHost", ServerHost);
    m_currentJsonObject.insert("ServerPort", ServerPort);
    m_currentJsonObject.insert("RootUser", RootUser);
    m_currentJsonObject.insert("Hash", Hash);
    m_currentJsonObject.insert("ServerBinDir", ServerBinDir);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("pathToClient", pathToClient);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
    m_currentJsonObject.insert("UseLocalWebDavDirectory", UseLocalWebDavDirectory);
    m_currentJsonObject.insert("WebdavHost", WebdavHost);
    m_currentJsonObject.insert("WebdavUser", WebdavUser);
    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
    return QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented);
}

QJsonObject ClientSettings::getJsonObject() {

    QJsonObject m_currentJsonObject = QJsonObject();
    m_currentJsonObject.insert("ServerHost", ServerHost);
    m_currentJsonObject.insert("ServerPort", ServerPort);
    m_currentJsonObject.insert("RootUser", RootUser);
    m_currentJsonObject.insert("Hash", Hash);
    m_currentJsonObject.insert("ServerBinDir", ServerBinDir);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("ServerStatus", ServerStatus);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("pathToClient", pathToClient);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
    m_currentJsonObject.insert("UseLocalWebDavDirectory", UseLocalWebDavDirectory);
    m_currentJsonObject.insert("WebdavHost", WebdavHost);
    m_currentJsonObject.insert("WebdavUser", WebdavUser);
    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
    return m_currentJsonObject;

}

void ClientSettings::setSettingsFileName(const QString &fname) {
    fileName = fname;
}
