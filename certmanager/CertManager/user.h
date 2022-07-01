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
    void setCache(const QJsonObject& value);
    void setUuid(const QUuid& value);
    void setOnline(bool value);

    QString domain() const;
    QString name() const;
    QString sid() const;
    QString ref() const;
    QUuid uuid() const;
    QJsonObject cache();


    //QJsonObject cacheObject();

    void eraseData();

    QStringList containers();
    QStringList mozillaProfiles();

    void setContainers(const QStringList& value);
    void setMozillaProfiles(const QStringList& value);

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

    QString sha1Hash() const;

private:
    QString _domain;
    QString _name;
    QString _sid;
    QString _ref;
    QUuid _uuid;
    bool _online;
    QJsonObject _cache;

    QStringList _containers;
    QStringList _mozillaProfiles;

    QMap<QString, KeysContainer*> m_cnt;
    QMap<QString, Certificate*> m_cert;
    QJsonObject m_available_certs;



signals:

};

#endif // USER_H
