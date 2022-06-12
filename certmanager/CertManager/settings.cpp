#include "settings.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

Settings::Settings(QObject *parent, const QString& parentFolder)
    : QObject{parent}
{

    _parentFolder = parentFolder;
    if(!_parentFolder.isEmpty()){
        if(_parentFolder.right(1) != QDir::separator())
            _parentFolder = _parentFolder + QDir::separator();

        QDir dir(_parentFolder);
        if(!dir.exists())
            _parentFolder = "";
    }

    QFile conf = QFile(_parentFolder + "conf.json");

    _charset = "CP866";
    _method = 0;
    _launch_mode = ws_srv;

    if(conf.open(QIODevice::ReadOnly)){

        auto doc = QJsonDocument::fromJson(conf.readAll());
        auto obj = doc.object();
        auto itr = obj.find("user");
        if(itr != obj.end())
            setUser(itr.value().toString());
        itr = obj.find("server");
        if(itr != obj.end())
            setServer(itr.value().toString());
        itr = obj.find("pwd");
        if(itr != obj.end())
            setPwd(itr.value().toString());
        itr = obj.find("method");
        if(itr != obj.end())
            setMethod(itr.value().toInt());
        itr = obj.find("charset");
        if(itr != obj.end())
            setCharset(itr.value().toString());
        itr = obj.find("launch_mode");
        if(itr != obj.end())
            setLanchMode((launchMode)itr.value().toInt());
        itr = obj.find("httpHost");
        if(itr != obj.end())
            setHttpHost(itr.value().toString());
        itr = obj.find("httpUsr");
        if(itr != obj.end())
            setHttpUsr(itr.value().toString());
        itr = obj.find("httpPwd");
        if(itr != obj.end())
            setHttpPwd(itr.value().toString());
    }

    if(_charset.isEmpty())
        _charset = "CP866";
}

void Settings::setUser(const QString &usr)
{
    _user = usr;
}

void Settings::setServer(const QString &val)
{
    _server = val;
}

void Settings::setPwd(const QString &val)
{
    _pwd = val;
}

void Settings::setLanchMode(launchMode value)
{
    _launch_mode = value;
}

void Settings::setHttpHost(const QString &val)
{
    _httpHost = val;
}

void Settings::setHttpUsr(const QString &val)
{
    _httpUsr = val;
}

void Settings::setHttpPwd(const QString &val)
{
    _httpPwd = val;
}

launchMode Settings::launch_mode()
{
    return _launch_mode;
}

QString Settings::user() const
{
    return _user;
}

QString Settings::server() const
{
    return _server;
}

QString Settings::pwd() const
{
    return _pwd;
}

QString Settings::httpHost() const
{
    return _httpHost;
}

QString Settings::httpUsr() const
{
    return _httpUsr;
}

QString Settings::httpPwd() const
{
    return _httpPwd;
}

void Settings::save()
{
    QFile conf = QFile(_parentFolder + "conf.json");

    if(conf.open(QIODevice::WriteOnly)){

        auto doc = QJsonDocument{};
        auto obj = QJsonObject{};
        obj.insert("user", _user);
        obj.insert("server", _server);
        obj.insert("pwd", _pwd);
        obj.insert("method", _method);
        obj.insert("charset", _charset);
        obj.insert("launch_mode", _launch_mode);
        obj.insert("httpHost", _httpHost);
        obj.insert("httpUsr", _httpUsr);
        obj.insert("httpPwd", _httpPwd);
        doc.setObject(obj);

        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

        conf.close();
    }
}

QString Settings::charset()
{
    return _charset;
}

void Settings::setCharset(const QString &cp)
{
    if(cp == "CP833")
        return;
    _charset = cp;
}

int Settings::method()
{
    return _method;
}

void Settings::setMethod(int m)
{
    _method = m;
}

