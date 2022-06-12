#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QUuid>
#include <certificate.h>
#include "keyscontainer.h"

namespace boost { namespace uuids { class uuid; } }

//enum UserStorgareType{
//    StorgareRegistry,
//    StorgareVolume
//};

class CertUser : public QObject
{
    Q_OBJECT
public:
    explicit CertUser(QObject *parent = nullptr);

    void setDomain(const QString& value);
    void setRef(const QString& value);
    void setName(const QString& value);
    void setSid(const QString& value);

    void setTreeItem(QTreeWidgetItem * item);
    void setUuid(const QUuid& value);
    void setOnline(bool value);
    QTreeWidgetItem * treeItem();

    QStringList getRigstryData();
    QStringList getDivaceData();

    QString modelContainersText();

    KeysContainer *container(const QString& key);

    QString domain();
    QString name();
    QString sid();
    QString ref();
    QUuid uuid();

    QStringList containers();

    void setContainers(const QStringList& value);

    bool online();
    bool thisIsTheUser(const QString& usr, const QString& host);

    QMap<QString, Certificate*>& certificates();

    KeysContainer *cntDetails(const QString& name);

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

    QTreeWidgetItem * _treeItem;



signals:

};

#endif // USER_H
