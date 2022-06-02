#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QUuid>

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
    void setContainers(const QStringList& value);
    void setTreeItem(QTreeWidgetItem * item);
    void setUuid(const QUuid& value);
    void setOnline(bool value);
    QTreeWidgetItem * treeItem();

    QStringList getRigstryData();
    QStringList getDivaceData();

    QString domain();
    QString name();
    QString sid();
    QString ref();
    QUuid uuid();
    QStringList containers();
    bool online();
    bool thisIsTheUser(const QString& usr, const QString& host);

private:
    QString _domain;
    QString _name;
    QString _sid;
    QString _ref;
    QUuid _uuid;
    bool _online;
    QStringList _containers;
    QTreeWidgetItem * _treeItem;

    void init();

signals:

};

#endif // USER_H
