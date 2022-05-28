#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTreeWidgetItem>

class CertUser : public QObject
{
    Q_OBJECT
public:
    explicit CertUser(QObject *parent = nullptr);

    void setDomain(const QString& value);
    void setName(const QString& value);
    void setSid(const QString& value);
    void setContainers(const QStringList& value);
    void setTreeItem(QTreeWidgetItem * item);
    QTreeWidgetItem * treeItem();

    QString domain();
    QString name();
    QString sid();
    QStringList containers();

private:
    QString _domain;
    QString _name;
    QString _sid;
    QStringList _containers;
    QTreeWidgetItem * _treeItem;

    void init();

signals:

};

#endif // USER_H
