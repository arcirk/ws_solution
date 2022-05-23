#include "user.h"
#include "registry.h"

CertUser::CertUser(QObject *parent)
    : QObject{parent}
{
    _domain = "";
    _name = "";
    _sid = "";

}

void CertUser::setDomain(const QString &value)
{
    _domain = value;
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

QStringList CertUser::containers()
{
    return _containers;
}

void CertUser::init()
{
    auto reg = Registry();

}
