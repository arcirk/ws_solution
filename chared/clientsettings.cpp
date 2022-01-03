//#include "../include/stdfx.h"
#include "clientsettings.h"
//#include <iws_client.h>

#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QApplication>
#include <QTextStream>
#include <QProcess>

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

const QString bFieldsStr[] = {
        "ServerHost",
        "ServerPort",
        "User",
        "Hash",
        "ServerName",
        "AppName",
        "AutoConnect",
        "SaveHash",
        "ServerWorkingDirectory",
        "ClientWorkingDirectory",
        "isLocalInstallation",
        "RunAsService",
        "UseLocalWebDavDirectory",
        "LocalWebDavDirectory",
        "WebDavHost",
        "WebDavUser",
        "WebDavPwd",
        "WebDavSSL"};

QString ClientSettings::get_home(){
#ifdef _WINDOWS
    return getenv("APPDATA");
#else
    //return getenv("HOME");
    return QDir::homePath();
#endif
}

QString ClientSettings::get_conf_directory() {
    return get_home() + QDir::separator() + get_conf_dirname();
}

QString ClientSettings::get_conf_dirname() {
#ifdef _WINDOWS
    return "arcirk";
#else
    return ".arcirk";
#endif
}

bool ClientSettings::verify_directory(const QString& dir_path) {

    QDir p(dir_path);

    if (!p.exists()) {
        return p.mkpath(dir_path);
    }

    return true;
}

ClientSettings::ClientSettings()
{
    pwdEdit = false;

    QString file_name = "conf.json";

    init(false);

    output_directory = QCoreApplication::applicationDirPath() + QDir::separator() + "config";

    bool result = verify_directory(output_directory);

    if(!result){
        qDebug() << "Error verify config directory!";
        return;
    }

    if(!file_name.isEmpty())
        output_filename = file_name;

    QString fileName = output_directory + QDir::separator() + output_filename;

    if (!QFile(fileName).exists())
        save();
    else
        parse();
}

ClientSettings::ClientSettings(const QString &file_name, bool public_settings) {

    pwdEdit = false;

    init(public_settings);

    if(!public_settings){
        output_directory = get_conf_directory() + QDir::separator() + "config";
    }else
        output_directory = QCoreApplication::applicationDirPath() + QDir::separator() + "config";

    bool result = verify_directory(output_directory);

    if(!result){
        qDebug() << "Error verify config directory!";
        return;
    }

    if(!file_name.isEmpty())
        output_filename = file_name;

    QString fileName = output_directory + QDir::separator() + output_filename;

    if (!QFile(fileName).exists())
        save();
    else
        parse();
}

QVariant & ClientSettings::operator[] (bConfFieldsWrapper index) {
    return m_vec[index];
}

QVariant const &ClientSettings::operator[](bConfFieldsWrapper index) const {
    return m_vec[index];
}

void ClientSettings::init(bool server) {

    m_vec.clear();
    std::vector<bConfFieldsWrapper> m_bool;
    m_bool.push_back(bConfFieldsWrapper::AutoConnect);
    m_bool.push_back(bConfFieldsWrapper::RunAsService);
    m_bool.push_back(bConfFieldsWrapper::SaveHash);
    m_bool.push_back(bConfFieldsWrapper::UseLocalWebDavDirectory);
    m_bool.push_back(bConfFieldsWrapper::WebDavSSL);
    m_bool.push_back(bConfFieldsWrapper::isLocalInstallation);

    int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);

    for (int i = 0; i < fCount; ++i) {
        auto ind = (bConfFieldsWrapper)i;
        //std::string key = bFieldsStr[i];

        if (ind == bConfFieldsWrapper::ServerPort){
            //m_doc.addMember(content_value(key, 0));
            m_vec.push_back(0);
        }
        else{
            if(std::find(m_bool.begin(), m_bool.end(), ind) == m_bool.end())
                m_vec.push_back("");
            else
                m_vec.push_back(false);
        }
    }

    m_vec[bConfFieldsWrapper::ServerHost] = "localhost";
    m_vec[bConfFieldsWrapper::ServerPort] = 8080;
    m_vec[bConfFieldsWrapper::User] = "admin";
    m_vec[bConfFieldsWrapper::ServerName] = "NoName";
    m_vec[bConfFieldsWrapper::AppName] = "unknown application";
    if(server)
        m_vec[bConfFieldsWrapper::ServerWorkingDirectory] = QCoreApplication::applicationDirPath();
    else
        m_vec[bConfFieldsWrapper::ClientWorkingDirectory] = QCoreApplication::applicationDirPath();
}

void ClientSettings::save() {

    QJsonDocument m_doc{};
    QJsonObject obj = QJsonObject();

    int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);

    for (int i = 0; i < fCount; ++i) {
        QString key = bFieldsStr[i];
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

    QString fileName = output_directory + QDir::separator() + output_filename;

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }
    jsonFile.write(QJsonDocument(m_doc).toJson(QJsonDocument::Indented));
    jsonFile.close();
}

bool ClientSettings::parse() {

    //QJsonDocument m_doc{};
    QString fileName = output_directory + QDir::separator() + output_filename;

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly)){
        return false;
    }
    QByteArray saveData = jsonFile.readAll();
    QJsonDocument m_doc(QJsonDocument::fromJson(saveData));

    if(!m_doc.isObject())
        return false;

    QJsonObject obj = m_doc.object();

    int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);
    for (int i = 0; i < fCount; ++i) {
        QString key = bFieldsStr[i];
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

    return true;
}

//
//ClientSettings &ClientSettings::operator=(const ClientSettings &from)
//{
//    if (this == &from)
//    {
//        return *this;
//    }
//
//    ServerHost = from.ServerHost;
//    ServerPort = from.ServerPort;
//    RootUser = from.RootUser;
//    Hash = from.Hash;
//    ServerName = from.ServerName;
//    AppName = from.AppName;
//    AutoConnect = from.AutoConnect;
//    SaveHash = from.SaveHash;
//    ServerWorkingDirectory = from.ServerWorkingDirectory;
//    isLocalInstallation = from.isLocalInstallation;
//    RunAsService = from.RunAsService;
//    pwdEdit = from.pwdEdit;
//    password = from.password;
//    UseLocalWebDAvDirectory = from.UseLocalWebDAvDirectory;
//    LocalWebDavDirectory = from.LocalWebDavDirectory;
//    WebdavHost = from.WebdavHost;
//    WebdavUser = from.WebdavUser;
//    WebdavPwd = from.WebdavPwd;
//    WebdavSSL = from.WebdavSSL;
//
//    return *this;
//}


//
//bool ClientSettings::init(){
//
//    QDir dirConf(QApplication::applicationDirPath() + "/config");
//    if (!dirConf.exists())
//        dirConf.mkdir(QApplication::applicationDirPath() + "/config");
//
//    if(fileName.isEmpty())
//        fileName = "conf.json";
//
//    QString fpath = QApplication::applicationDirPath() + "/config/" + fileName;
//
//    QJsonObject m_currentJsonObject = QJsonObject();
//
//    QFileInfo fileInfo(fpath);
//    QDir::setCurrent(fileInfo.path());
//
//    QFile jsonFile(fpath);
//    if (!jsonFile.open(QIODevice::ReadOnly))
//    {
//        ServerHost ="localhost";
//        ServerPort = 8080;
//        RootUser = "admin";
//        ServerName = "NoName";
//        AppName = "admin_console";
//        AutoConnect = false;
//        isLocalInstallation = false;
//        RunAsService = false;
//        SaveHash = false;
//        pwdEdit = false;
//        password = "";
//        UseLocalWebDAvDirectory = false;
//        WebdavSSL = false;
//        return false;
//    }
//
//    QByteArray saveData = jsonFile.readAll();
//    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
//
//    m_currentJsonObject = jsonDocument.object();
//
//    auto iter = m_currentJsonObject.find("ServerHost");
//    if (iter != m_currentJsonObject.end()){
//        ServerHost = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("ServerPort");
//    if (iter != m_currentJsonObject.end()){
//        ServerPort = iter.value().toInt();
//    }
//    iter = m_currentJsonObject.find("RootUser");
//    if (iter != m_currentJsonObject.end()){
//        RootUser = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("Hash");
//    if (iter != m_currentJsonObject.end()){
//        Hash = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("ServerWorkingDirectory");
//    if (iter != m_currentJsonObject.end()){
//        ServerWorkingDirectory = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("ServerName");
//    if (iter != m_currentJsonObject.end()){
//        ServerName = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("AutoConnect");
//    if (iter != m_currentJsonObject.end()){
//        AutoConnect = iter.value().toBool();
//    }
//    iter = m_currentJsonObject.find("SaveHash");
//    if (iter != m_currentJsonObject.end()){
//        SaveHash = iter.value().toBool();
//    }
//    iter = m_currentJsonObject.find("isLocalInstallation");
//    if (iter != m_currentJsonObject.end()){
//        isLocalInstallation = iter.value().toBool();
//    }
//    iter = m_currentJsonObject.find("RunAsService");
//    if (iter != m_currentJsonObject.end()){
//        RunAsService = iter.value().toBool();
//    }
//    iter = m_currentJsonObject.find("UseLocalWebDAvDirectory");
//    if (iter != m_currentJsonObject.end()){
//        UseLocalWebDAvDirectory = iter.value().toBool();
//    }
//    iter = m_currentJsonObject.find("LocalWebDavDirectory");
//    if (iter != m_currentJsonObject.end()){
//        LocalWebDavDirectory = iter.value().toString();
//    }
//
//    iter = m_currentJsonObject.find("WebdavHost");
//    if (iter != m_currentJsonObject.end()){
//        WebdavHost = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("WebdavUser");
//    if (iter != m_currentJsonObject.end()){
//        WebdavUser = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("WebdavPwd");
//    if (iter != m_currentJsonObject.end()){
//        WebdavPwd = iter.value().toString();
//    }
//    iter = m_currentJsonObject.find("WebdavSSL");
//    if (iter != m_currentJsonObject.end()){
//        WebdavSSL = iter.value().toBool();
//    }
//
//    AppName = "admin_console";
//
//    jsonFile.close();
//
//    return true;
//}
//
//void ClientSettings::save_settings(){
//
//    if(!SaveHash)
//        Hash = "";
//
//    QJsonObject m_currentJsonObject = QJsonObject();
//    m_currentJsonObject.insert("ServerHost", ServerHost);
//    m_currentJsonObject.insert("ServerPort", ServerPort);
//    m_currentJsonObject.insert("RootUser", RootUser);
//    m_currentJsonObject.insert("Hash", Hash);
//    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
//    m_currentJsonObject.insert("ServerName", ServerName);
//    m_currentJsonObject.insert("AutoConnect", AutoConnect);
//    m_currentJsonObject.insert("SaveHash", SaveHash);
//    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
//    m_currentJsonObject.insert("AppName", AppName);
//    m_currentJsonObject.insert("RunAsService", RunAsService);
//    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
//    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
//    m_currentJsonObject.insert("WebdavHost", WebdavHost);
//    m_currentJsonObject.insert("WebdavUser", WebdavUser);
//    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
//    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
//    //qDebug() << QApplication::applicationDirPath() ;
//
//    QDir dirConf(QApplication::applicationDirPath() + "/config");
//    if (!dirConf.exists())
//        dirConf.mkdir(QApplication::applicationDirPath()+ "/config");
//
//    if(fileName.isEmpty())
//        fileName = "conf.json";
//
//    QString saveFileName = QApplication::applicationDirPath() + "/config/" + fileName;
//
//    QFileInfo fileInfo(saveFileName);
//    QDir::setCurrent(fileInfo.path());
//
//    QFile jsonFile(saveFileName);
//    if (!jsonFile.open(QIODevice::WriteOnly))
//    {
//        return;
//    }
//
//    jsonFile.write(QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented));
//    jsonFile.close();
//}
//
//QString ClientSettings::getJson() {
//    QJsonObject m_currentJsonObject = QJsonObject();
//    m_currentJsonObject.insert("ServerHost", ServerHost);
//    m_currentJsonObject.insert("ServerPort", ServerPort);
//    m_currentJsonObject.insert("RootUser", RootUser);
//    m_currentJsonObject.insert("Hash", Hash);
//    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
//    m_currentJsonObject.insert("ServerName", ServerName);
//    m_currentJsonObject.insert("AutoConnect", AutoConnect);
//    m_currentJsonObject.insert("SaveHash", SaveHash);
//    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
//    m_currentJsonObject.insert("RunAsService", RunAsService);
//    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
//    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
//    m_currentJsonObject.insert("WebdavHost", WebdavHost);
//    m_currentJsonObject.insert("WebdavUser", WebdavUser);
//    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
//    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
//    return QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented);
//}
//
QJsonObject ClientSettings::getJsonObject() {

    QString fileName = output_directory + QDir::separator() + output_filename;

    QFile jsonFile(fileName);
    if (!jsonFile.open(QIODevice::ReadOnly)){
        return {};
    }
    QByteArray saveData = jsonFile.readAll();
    QJsonDocument m_doc(QJsonDocument::fromJson(saveData));

    if(!m_doc.isNull())
        return {};

    return m_doc.object();

//    QJsonObject m_currentJsonObject = QJsonObject();
//    m_currentJsonObject.insert("ServerHost", ServerHost);
//    m_currentJsonObject.insert("ServerPort", ServerPort);
//    m_currentJsonObject.insert("RootUser", RootUser);
//    m_currentJsonObject.insert("Hash", Hash);
//    m_currentJsonObject.insert("ServerWorkingDirectory", ServerWorkingDirectory);
//    m_currentJsonObject.insert("ServerName", ServerName);
//    m_currentJsonObject.insert("ServerStatus", ServerStatus);
//    m_currentJsonObject.insert("AutoConnect", AutoConnect);
//    m_currentJsonObject.insert("SaveHash", SaveHash);
//    m_currentJsonObject.insert("isLocalInstallation", isLocalInstallation);
//    m_currentJsonObject.insert("RunAsService", RunAsService);
//    m_currentJsonObject.insert("UseLocalWebDAvDirectory", UseLocalWebDAvDirectory);
//    m_currentJsonObject.insert("LocalWebDavDirectory", LocalWebDavDirectory);
//    m_currentJsonObject.insert("WebdavHost", WebdavHost);
//    m_currentJsonObject.insert("WebdavUser", WebdavUser);
//    m_currentJsonObject.insert("WebdavPwd", WebdavPwd);
//    m_currentJsonObject.insert("WebdavSSL", WebdavSSL);
//    return m_currentJsonObject;
//
}
//
//void ClientSettings::setSettingsFileName(const QString &fname) {
//    fileName = fname;
//}

bool ClientSettings::createDaemonFile()
{
    QString ServerWorkingDirectory = m_vec[bConfFieldsWrapper::ServerWorkingDirectory].toString();

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

void* ClientSettings::_crypt(void* data, unsigned data_size, void* key, unsigned key_size)
{
    assert(data && data_size);
    if (!key || !key_size) return data;

    auto* kptr = (uint8_t*)key; // начало ключа
    uint8_t* eptr = kptr + key_size; // конец ключа

    // отксоривание
    for (auto* dptr = (uint8_t*)data; data_size--; dptr++)
    {
        *dptr ^= *kptr++;
        if (kptr == eptr) kptr = (uint8_t*)key; // переход на начало ключа
    }
    return data;
}

std::string ClientSettings::crypt(const std::string &source, const std::string& key) {

    int n = (int)source.length();
    char text[n + 1];
    std::strcpy(text, source.c_str());
    int n1 = (int)key.length();
    char pass[n1 + 1];
    std::strcpy(pass, key.c_str());

    _crypt(text, ARR_SIZE(text), pass, ARR_SIZE(pass));

    return text;
}