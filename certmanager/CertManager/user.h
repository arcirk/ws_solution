#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QUuid>
#include <certificate.h>
#include "keyscontainer.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace boost { namespace uuids { class uuid; } }

class CertUser : public QObject
{
    Q_OBJECT
public:
    explicit CertUser(QObject *parent = nullptr);

    void setDomain(const QString& value);
    void setRef(const QString& value);
    void setName(const QString& value);
    void setSid(const QString& value);

    void setUuid(const QUuid& value);
    void setOnline(bool value);

    QString domain();
    QString name();
    QString sid();
    QString ref();
    QUuid uuid();

    void eraseData();

    QStringList containers();

    void setContainers(const QStringList& value);

    bool online();
    bool thisIsTheUser(const QString& usr, const QString& host);


    QString modelContainersText();
    KeysContainer *container(const QString& key);
    QStringList getRigstryData();
    QStringList getDivaceData();


    QMap<QString, Certificate*>& certificates();
    QString available_certificates();
    void set_available_certificates(const QJsonObject& certsTable);

    QString modelCertificatesText();
    QJsonObject certModel();
    void certsFromModel(const QJsonObject& certs);

    KeysContainer *cntDetails(const QString& name);

    QString sha1Hash();

private:
    QString _domain;
    QString _name;
    QString _sid;
    QString _ref;
    QUuid _uuid;
    bool _online;

    QStringList _containers;

    QMap<QString, KeysContainer*> m_cnt;
    QMap<QString, Certificate*> m_cert;
    QJsonObject m_available_certs;

//    QJsonTableModel* _model;
//    QProxyModel * _proxyModel;
//    QJsonTableModel* _modelCerts;

signals:

};

#endif // USER_H
