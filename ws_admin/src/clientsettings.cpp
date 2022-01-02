//#include "../include/stdfx.h"
#include "../include/clientsettings.h"
#include <iws_client.h>

#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QApplication>
#include <QTextStream>
#include <QProcess>

ClientSettings::ClientSettings()
    :ServerHost("localhost")
    , ServerPort(8080)
    , RootUser("admin")
    , ServerName("NoName")
    , AppName("admin_console")

{
    Hash = QString::fromStdString(IClient::get_hash("admin", "admin"));
    fileName = "conf.json";
    isLocalInstallation = false;
    pwdEdit = false;
    password = "";
    UseLocalWebDAvDirectory = false;
    WebdavSSL = false;
}

ClientSettings &ClientSettings::operator=(const ClientSettings &from)
{
    if (this == &from)
    {
        return *this;
    }

    ServerHost = from.ServerHost;
    ServerPort = from.ServerPort;
    RootUser = from.RootUser;
    Hash = from.Hash;
    ServerName = from.ServerName;
    AppName = from.AppName;
    AutoConnect = from.AutoConnect;
    SaveHash = from.SaveHash;
    ServerWorkingDirectory = from.ServerWorkingDirectory;
    isLocalInstallation = from.isLocalInstallation;
    RunAsService = from.RunAsService;
    pwdEdit = from.pwdEdit;
    password = from.password;
    UseLocalWebDAvDirectory = from.UseLocalWebDAvDirectory;
    LocalWebDavDirectory = from.LocalWebDavDirectory;
    WebdavHost = from.WebdavHost;
    WebdavUser = from.WebdavUser;
    WebdavPwd = from.WebdavPwd;
    WebdavSSL = from.WebdavSSL;

    return *this;
}



bool ClientSettings::init(){

    QDir dirConf(QApplication::applicationDirPath() + "/config");
    if (!dirConf.exists())
        dirConf.mkdir(QApplication::applicationDirPath() + "/config");

    if(fileName.isEmpty())
        fileName = "conf.json";

    QString fpath = QApplication::applicationDirPath() + "/config/" + fileName;

    QJsonObject m_currentJsonObject = QJsonObject();

    QFileInfo fileInfo(fpath);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(fpath);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        ServerHost ="localhost";
        ServerPort = 8080;
        RootUser = "admin";
        ServerName = "NoName";
        AppName = "admin_console";
        AutoConnect = false;
        isLocalInstallation = false;
        RunAsService = false;
        SaveHash = false;
        pwdEdit = false;
        password = "";
        UseLocalWebDAvDirectory = false;
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
    iter = m_currentJsonObject.find("ServerWorkingDirectory");
    if (iter != m_currentJsonObject.end()){
        ServerWorkingDirectory = iter.value().toString();
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
    iter = m_currentJsonObject.find("isLocalInstallation");
    if (iter != m_currentJsonObject.end()){
        isLocalInstallation = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("RunAsService");
    if (iter != m_currentJsonObject.end()){
        RunAsService = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("UseLocalWebDAvDirectory");
    if (iter != m_currentJsonObject.end()){
        UseLocalWebDAvDirectory = iter.value().toBool();
    }
    iter = m_currentJsonObject.find("LocalWebDavDirectory");
    if (iter != m_currentJsonObject.end()){
        LocalWebDavDirectory = iter.value().toString();
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
    iter = m_currentJsonObject.find("WebdavSSL");
    if (iter != m_currentJsonObject.end()){
        WebdavSSL = iter.value().toBool();
    }

    AppName = "admin_console";

    jsonFile.close();

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
    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
    m_currentJsonObject.insert("AppName", AppName);
    m_currentJsonObject.insert("RunAsService", RunAsService);
    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
    m_currentJsonObject.insert("WebdavHost", WebdavHost);
    m_currentJsonObject.insert("WebdavUser", WebdavUser);
    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
    //qDebug() << QApplication::applicationDirPath() ;

    QDir dirConf(QApplication::applicationDirPath() + "/config");
    if (!dirConf.exists())
        dirConf.mkdir(QApplication::applicationDirPath()+ "/config");

    if(fileName.isEmpty())
        fileName = "conf.json";

    QString saveFileName = QApplication::applicationDirPath() + "/config/" + fileName;

    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    jsonFile.write(QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented));
    jsonFile.close();
}

QString ClientSettings::getJson() {
    QJsonObject m_currentJsonObject = QJsonObject();
    m_currentJsonObject.insert("ServerHost", ServerHost);
    m_currentJsonObject.insert("ServerPort", ServerPort);
    m_currentJsonObject.insert("RootUser", RootUser);
    m_currentJsonObject.insert("Hash", Hash);
    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
    m_currentJsonObject.insert("RunAsService", RunAsService);
    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
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
    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
    m_currentJsonObject.insert("ServerName", ServerName);
    m_currentJsonObject.insert("ServerStatus", ServerStatus);
    m_currentJsonObject.insert("AutoConnect", AutoConnect);
    m_currentJsonObject.insert("SaveHash", SaveHash);
    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
    m_currentJsonObject.insert("RunAsService", RunAsService);
    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
    m_currentJsonObject.insert("WebdavHost", WebdavHost);
    m_currentJsonObject.insert("WebdavUser", WebdavUser);
    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
    return m_currentJsonObject;

}

void ClientSettings::setSettingsFileName(const QString &fname) {
    fileName = fname;
}

bool ClientSettings::createDaemonFile()
{
    if(ServerWorkingDirectory.isEmpty()){
        qDebug() << "Не указана рабочая директория сервера!";
        return false;
    }

    QFile srv(ServerWorkingDirectory + "/ws_server");

    if(!srv.exists()){
        qDebug() << "Исполняемый файл сервера не найден!";
        return false;
    }

    QFile daemonFile(QApplication::applicationDirPath() + "/config/ws_server.service");

    if(daemonFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream writeStream(&daemonFile);
        writeStream << "[Unit]\n";
        writeStream << "Description= arcadics websocket service\n";
        writeStream << "After=network.target\n";
        writeStream << "[Service]\n";
        writeStream << "Type=simple\n";
        writeStream << "ExecStart=" + srv.fileName() + "\n";
        writeStream << "WorkingDirectory=" + ServerWorkingDirectory + "\n";
        writeStream << "[Install]\n";
        writeStream << "WantedBy=multi-user.target\n";
        daemonFile.close();
    }else
    {
        qDebug() << "Ошибка создания файла службы!";
        return false;
    }

    return true;
}

bool ClientSettings::installService(const QString &pwd) {

    if(createDaemonFile()) {
        return installDaemonFile(pwd);
    }

    return false;
}

bool ClientSettings::startService(const QString &pwd)
{

    QString cmd = "systemctl start ws_server";

    QProcess commandProc;
    QString commandLine;

    commandLine = "echo " + pwd + " | sudo -S " + cmd + "\n";

    commandProc.start("/bin/bash");
    commandProc.write(commandLine.toUtf8());

    bool result = false;

    if(!commandProc.waitForStarted(2000)) //default wait time 30 sec
    {
        qDebug() << "Cannot execute:" << cmd;
    }

    commandProc.waitForFinished(2000);

    commandProc.setProcessChannelMode(QProcess::MergedChannels);

    QString err = QString::fromStdString(commandProc.readAllStandardError().toStdString());
    bool errPass = err.contains("password was provided") ==  0;
    if(errPass){
        if(commandProc.exitStatus() == QProcess::NormalExit
           && commandProc.exitCode() == QProcess::NormalExit){
            qDebug() << "Success";
            result = true;
        } else {
            qDebug() << "Cannot copy file" << cmd;
        }
    }else{
        qDebug() << err;
    }

    commandProc.close();

    return result;
}

bool ClientSettings::stopService(const QString &pwd)
{
    QString cmd = "systemctl stop ws_server";

    QProcess commandProc;
    QString commandLine;

    commandLine = "echo " + pwd + " | sudo -S " + cmd + "\n";

    commandProc.start("/bin/bash");
    commandProc.write(commandLine.toUtf8());

    bool result = false;

    if(!commandProc.waitForStarted(2000)) //default wait time 30 sec
    {
        qDebug() << "Cannot execute:" << cmd;
    }

    commandProc.waitForFinished(2000);

    commandProc.setProcessChannelMode(QProcess::MergedChannels);

    QString err = QString::fromStdString(commandProc.readAllStandardError().toStdString());
    bool errPass = err.contains("password was provided") ==  0;
    if(errPass){
        if(commandProc.exitStatus() == QProcess::NormalExit
           && commandProc.exitCode() == QProcess::NormalExit){
            qDebug() << "Success";
            result = true;
        } else {
            qDebug() << "Cannot copy file" << cmd;
        }
    }else{
        qDebug() << err;
    }

    commandProc.close();

    return result;
}

bool ClientSettings::statusService()
{
    QString cmd = "systemctl status ws_server";

    QProcess commandProc;
    QString commandLine;

    commandLine = cmd + "\n";

    commandProc.start("/bin/bash");
    commandProc.write(commandLine.toUtf8());

    bool result = false;

    if(!commandProc.waitForStarted(1000)) //default wait time 30 sec
    {
        qDebug() << "Cannot execute:" << cmd;
    }

    commandProc.waitForFinished(1000);

    commandProc.setProcessChannelMode(QProcess::MergedChannels);

    if(commandProc.exitStatus() == QProcess::NormalExit
       && commandProc.exitCode() == QProcess::NormalExit){;
        QString r = QString::fromStdString(commandProc.readAllStandardOutput().toStdString());
        qDebug() << qPrintable(r);
        result = r.contains("active (running)") !=  0;
    } else {
        qDebug() << qPrintable(QString::fromStdString(commandProc.readAllStandardError().toStdString()));
    }

    commandProc.close();

    return result;
}

bool ClientSettings::installDaemonFile(const QString &pwd) {

    QFile daemonFile(QApplication::applicationDirPath() + "/config/ws_server.service");
    if(!daemonFile.exists()){
        qDebug() << "Файл службы не найден!";
        return false;
    }
    QString cmd = "/bin/cp " + daemonFile.fileName() +  " /etc/systemd/system/ws_server.service";
    QString cmd1 = "systemctl enable ws_server";

    QProcess commandProc;
    QString commandLine;

    commandLine = "echo " + pwd + " | sudo -S " + cmd + "\n" + cmd1;

    commandProc.start("/bin/bash");
    commandProc.write(commandLine.toUtf8());

    bool result = false;

    if(!commandProc.waitForStarted(2000)) //default wait time 30 sec
    {
        qDebug() << "Cannot execute:" << cmd;
    }

    commandProc.waitForFinished(2000);

    commandProc.setProcessChannelMode(QProcess::MergedChannels);

    QString err = QString::fromStdString(commandProc.readAllStandardError().toStdString());
    bool errPass = err.contains("password was provided") ==  0;
    if(errPass){
        if(commandProc.exitStatus() == QProcess::NormalExit
           && commandProc.exitCode() == QProcess::NormalExit){
            qDebug() << "Success";
            result = true;
        } else {
            qDebug() << "Cannot copy file" << cmd;
        }
    }else{
        qDebug() << err;
    }

    commandProc.close();

    return result;
}
