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

    m_cnt.clear();

    foreach(auto cntName, value){
        if(cntName.isEmpty())
            continue;
        auto _cnt = new KeysContainer(this);
        _cnt->fromContainerName(cntName);
        m_cnt.insert(cntName, _cnt);
    }
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
    foreach(auto str , _containers){
        if(str.isEmpty())
            continue;
        if(str.indexOf(ex) > 0)
            result.append(str.replace("\r", ""));
    }
    return result;
}

QStringList CertUser::getDivaceData()
{
    QStringList result;
    QRegularExpression ex("REGISTRY");
    foreach(auto str , _containers){
        if(str.isEmpty())
            continue;
        if(str.indexOf(ex) < 0)
            result.append(str.replace("\r", ""));
    }
    return result;
}

QString CertUser::modelContainersText()
{
    auto doc = QJsonDocument();
    auto obj = QJsonObject();
    auto arrCols = QJsonArray();
    auto arrRows = QJsonArray();

    arrCols.append("Empty");
    arrCols.append("volume");
    arrCols.append("name");
    arrCols.append("subject");
    arrCols.append("issuer");
    arrCols.append("notValidBefore");
    arrCols.append("notValidAfter");
    arrCols.append("parentUser");
    arrCols.append("nameInStorgare");
    obj.insert("columns", arrCols);


    foreach(auto itr, m_cnt){
        auto row = QJsonObject();
        row.insert("Empty", QString());
        row.insert("volume",itr->volume());
        row.insert("name", itr->fullName());
        row.insert("subject", itr->subject());
        row.insert("issuer", itr->issuer());
        row.insert("notValidBefore", itr->notValidBefore());
        row.insert("notValidAfter", itr->notValidAfter());
        row.insert("parentUser", itr->parentUser());
        row.insert("nameInStorgare", itr->nameInStorgare());
        arrRows.append(row);
    }

    obj.insert("rows", arrRows);

    doc.setObject(obj);

    return doc.toJson();
}

KeysContainer *CertUser::container(const QString &key)
{
    auto itr = m_cnt.find(key);
    if(itr != m_cnt.end()){
        return itr.value();
    }else
        return nullptr;
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

QMap<QString, Certificate *>& CertUser::certificates()
{
    return m_cert;
}

KeysContainer *CertUser::cntDetails(const QString &name)
{
    auto itr = m_cnt.find(name);
    if(itr != m_cnt.end())
        return  itr.value();

    return nullptr;
}

