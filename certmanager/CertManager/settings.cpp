#include "settings.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

settings::settings(QObject *parent)
    : QObject{parent}
{

    QFile conf = QFile("conf.json");

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
    }

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

        doc.setObject(obj);

        conf.write(QJsonDocument(doc).toJson(QJsonDocument::Indented));

        conf.close();
    }
}

