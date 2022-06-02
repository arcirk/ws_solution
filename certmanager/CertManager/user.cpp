#include "user.h"
#include "registry.h"

CertUser::CertUser(QObject *parent)
    : QObject{parent}
{
    _domain = "";
    _name = "";
    _sid = "";
    _treeItem = nullptr;
    _uuid = QUuid();

}

void CertUser::setDomain(const QString &value)
{
    _domain = value;
}

void CertUser::setRef(const QString &value)
{
    _ref = value;
}

void CertUser::setName(const QString &value)
{
    _name = value;
}

void CertUser::setSid(const QString &value)
{
    _sid = value;
}

void CertUser::setContainers(const QStringList &value)
{
    _containers = value;
}

void CertUser::setTreeItem(QTreeWidgetItem *item)
{
    _treeItem = item;
}

void CertUser::setUuid(const QUuid &value)
{
    _uuid = value;
}

void CertUser::setOnline(bool value)
{
    _online = value;
}

QTreeWidgetItem *CertUser::treeItem()
{
    return _treeItem;
}

QStringList CertUser::getRigstryData()
{
    QStringList result;
    QRegularExpression ex("REGISTRY");
    for(auto str : _containers){
        if(str.isEmpty())
            continue;
        if(str.indexOf(ex) > 0)
            result.append(str);
    }
    return result;
}

QStringList CertUser::getDivaceData()
{
    QStringList result;
    QRegularExpression ex("REGISTRY");
    for(auto str : _containers){
        if(str.isEmpty())
            continue;
        if(str.indexOf(ex) < 0)
            result.append(str);
    }
    return result;
}

QString CertUser::domain()
{
    return _domain;
}

QString CertUser::name()
{
    return _name;
}

QString CertUser::sid()
{
    return _sid;
}

QString CertUser::ref()
{
    return _ref;
}

QUuid CertUser::uuid()
{
    return _uuid;
}

QStringList CertUser::containers()
{
    return _containers;
}

bool CertUser::online()
{
    return _online;
}

bool CertUser::thisIsTheUser(const QString &usr, const QString &host)
{
    if(usr.trimmed().toLower() == _name.trimmed().toLower()
            && host.trimmed().toLower() == _domain.trimmed().toLower())
        return true;
    else
        return false;
}

void CertUser::init()
{


}
