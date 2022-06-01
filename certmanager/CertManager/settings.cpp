#include "settings.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

settings::settings(QObject *parent)
    : QObject{parent}
{

    QFile conf = QFile("conf.json");

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
    }

    if(_charset.isEmpty())
        _charset = "CP866";
}

void settings::setUser(const QString &usr)
{
    _user = usr;
}

void settings::setServer(const QString &val)
{
    _server = val;
}

void settings::setPwd(const QString &val)
{
    _pwd = val;
}

void settings::setLanchMode(launchMode value)
{
    _launch_mode = value;
}

launchMode settings::launch_mode()
{
    return _launch_mode;
}

QString settings::user()
{
    return _user;
}

QString settings::server()
{
    return _server;
}

QString settings::pwd()
{
    return _pwd;
}

void settings::save()
{
    QFile conf = QFile("conf.json");

    if(conf.open(QIODevice::WriteOnly)){

        auto doc = QJsonDocument{};
        auto obj = QJsonObject{};
        obj.insert("user", _user);
        obj.insert("server", _server);
        obj.insert("pwd", _pwd);
        obj.insert("method", _method);
        obj.insert("charset", _charset);
        obj.insert("launch_mode", _launch_mode);
        doc.setObject(obj);

        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

        conf.close();
    }
}

QString settings::charset()
{
    return _charset;
}

void settings::setCharset(const QString &cp)
{
    if(cp == "CP833")
        return;
    _charset = cp;
}

int settings::method()
{
    return _method;
}

void settings::setMethod(int m)
{
    _method = m;
}

