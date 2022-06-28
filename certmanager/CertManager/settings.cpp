#include "settings.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

Settings::Settings(QObject *parent, const QString& parentFolder)
    : QObject{parent}
{

    _useSettingsFromHttp = false;

    _cacheDirectory = parentFolder;
    if(!_cacheDirectory.isEmpty()){
        _cacheDirectory = _cacheDirectory + QDir::separator();
    }

    QFile conf = QFile(_cacheDirectory + "conf.json");

    _charset = "CP866";
    _method = 0;
    _launch_mode = ws_srv;

    if(conf.open(QIODevice::ReadOnly)){

        auto doc = QJsonDocument::fromJson(conf.readAll());
        auto obj = doc.object();
        setJsonObject(obj);
    }else{
        save();
    }

    if(_charset.isEmpty())
        _charset = "CP866";
}

Settings::Settings(const QJsonObject &object, const QString& cacheDirectory, QObject *parent)
    : QObject{parent}
{
    _useSettingsFromHttp = false;
    _cacheDirectory = cacheDirectory;
    _charset = "CP866";
    _method = 0;
    _launch_mode = ws_srv;

    if(!object.isEmpty())
        setJsonObject(object);
    else
        save();

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

bool Settings::useSettingsFromHttp()
{
    return _useSettingsFromHttp;
}

void Settings::setUseSettingsFromHttp(bool value)
{
    _useSettingsFromHttp = value;
}

bool Settings::customWsUser()
{
    return _customWsUser;
}

void Settings::setCustomWsUser(bool value)
{
    _customWsUser = value;
}

void Settings::save()
{
    QFile conf = QFile(_cacheDirectory +  "/conf.json");

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
        obj.insert("useSettingsFromHttp", _useSettingsFromHttp);
        obj.insert("customWsUser", _customWsUser);
        doc.setObject(obj);

        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

        conf.close();
    }
}

QJsonObject Settings::to_object()
{
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
    obj.insert("useSettingsFromHttp", _useSettingsFromHttp);
    obj.insert("customWsUser", _customWsUser);
    return obj;
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

QString Settings::readFromUserCache(const QJsonObject &cache)
{

}

void Settings::setJsonObject(const QJsonObject &obj)
{
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
    itr = obj.find("useSettingsFromHttp");
    if(itr != obj.end())
        setUseSettingsFromHttp(itr.value().toBool());
    itr = obj.find("customWsUser");
    if(itr != obj.end())
        setCustomWsUser(itr.value().toBool());
}

