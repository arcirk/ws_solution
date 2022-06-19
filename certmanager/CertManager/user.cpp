#include "user.h"
#include "registry.h"
//#include <qjsontablemodel.h>
//#include <qproxymodel.h>

CertUser::CertUser(QObject *parent)
    : QObject{parent}
{
    _domain = "";
    _name = "";
    _sid = "";
    _uuid = QUuid();
//    _model = nullptr;
//    _proxyModel = nullptr;

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
    _containers.clear();
    foreach(auto n, value){
        QString __name = n;
        __name.replace("\r", "");
        if(!__name.isEmpty())
            _containers.append(__name);
    }

    m_cnt.clear();
    foreach(auto n, _containers){
        auto _cnt = new KeysContainer(this);

        _cnt->fromContainerName(n);
        m_cnt.insert(n, _cnt);
    }
}

void CertUser::setUuid(const QUuid &value)
{
    _uuid = value;
}

void CertUser::setOnline(bool value)
{
    _online = value;
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
    arrCols.append("SecondField");
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
        row.insert("name", itr->originalName());
        row.insert("SecondField", itr->bindName());
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

QString CertUser::modelCertificatesText()
{
    return QJsonDocument(certModel()).toJson();
}

QJsonObject CertUser::certModel()
{
    auto objCols = QJsonArray();
    objCols.append("Empty");
    objCols.append("subject");
    objCols.append("issuer");
    objCols.append("notValidBefore");
    objCols.append("notValidAfter");
    objCols.append("parentUser");
    objCols.append("container");
    objCols.append("serial");
    objCols.append("sha1");

    auto objRows = QJsonArray();
    foreach(auto itr, certificates()){
        auto row = QJsonObject();
        row.insert("Empty", QString());
        row.insert("subject", itr->subject());
        row.insert("issuer", itr->issuer());
        row.insert("notValidBefore", itr->notValidBefore());
        row.insert("notValidAfter", itr->notValidAfter());
        row.insert("parentUser", itr->parentUser());
        row.insert("container", itr->container());
        row.insert("serial", itr->serial());
        row.insert("sha1", itr->sha1Hash());
        objRows.append(row);
    }

    auto objMain = QJsonObject();
    objMain.insert("columns", objCols);
    objMain.insert("rows", objRows);

    return objMain;

}

void CertUser::certsFromModel(const QJsonObject &certs)
{
    m_cert.clear();
    auto objRows = certs.value("rows").toArray();
    if(objRows.isEmpty())
        return;
    foreach(auto item, objRows){
        auto row = item.toObject();
        auto cert = new Certificate(this);
        cert->fromModelObject(row);
        if(cert->serial().isEmpty()){
            delete cert;
            continue;
        }
        m_cert.insert(cert->serial(), cert);
    }
}


KeysContainer *CertUser::cntDetails(const QString &name)
{
    auto itr = m_cnt.find(name);
    if(itr != m_cnt.end())
        return  itr.value();

    return nullptr;
}

//QProxyModel *CertUser::modelContainers()
//{
//    return _proxyModel;
//}

//QJsonTableModel *CertUser::modelCertificates()
//{
//    return _modelCerts;
//}

//void CertUser::setModel()
//{
//    if(_model)
//        _model = new QJsonTableModel(this);
//    _model->setJsonText(modelContainersText());
//    _model->reset();

//    if(_proxyModel)
//        _proxyModel = new QProxyModel(this);
//    _proxyModel->setSourceModel(_model);

//    if(_modelCerts)
//        _modelCerts = new QJsonTableModel(this->parent());
//    _modelCerts->setJsonText(modelCertificatesText());
//    _modelCerts->reset();
//}

