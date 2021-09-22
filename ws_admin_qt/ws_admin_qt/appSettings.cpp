#include "stdfx.h"
#include "AppSettings.h"
//#include "global.h"
//#include "iws_client.h"

#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>

appSettings::appSettings()
    :ServerHost("localhost")
    , ServerPort(8080)
    , RootUser("admin")
    , ServerBinDir("host")
    , ServerName("NoName")
{
    Hash = QString::fromStdString(IClient::get_hash("admin", "admin"));

}

bool appSettings::init(){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "config.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
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
        Hash = iter.value().toString();
    }
    iter = m_currentJsonObject.find("ServerName");
    if (iter != m_currentJsonObject.end()){
        ServerName = iter.value().toString();
    }
    return true;
}

void appSettings::save_settings(){

    QJsonObject m_currentJsonObject = QJsonObject();
    m_currentJsonObject.insert("ServerHost", ServerHost);
    m_currentJsonObject.insert("ServerPort", ServerPort);
    m_currentJsonObject.insert("RootUser", RootUser);
    m_currentJsonObject.insert("Hash", Hash);
    m_currentJsonObject.insert("ServerBinDir", ServerBinDir);
    m_currentJsonObject.insert("ServerName", ServerName);

    QString saveFileName = "config.json";
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

