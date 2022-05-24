#ifndef REGISTRY_H
#define REGISTRY_H

#include <QSettings>
#include <QObject>
#include <QProcess>
#include "user.h"

class Registry : public QObject
{
    Q_OBJECT
public:
    explicit Registry(QObject *parent = nullptr);

    const QString rootCertNode = "\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users";

    static QStringList currentUserContainers(const QString& sid);
    //QString currentSID(const QString& name );
    //QString currentWindowsName();

    static QStringList  currentUser(CertUser * usr);

    static bool deleteContainer(CertUser * usr, const QString& containerName);
    QStringList localUsers();

    void usersFromRegistry(const QString rootNode, const QString& group, QStringList lst = QStringList{});

};

#endif // REGISTRY_H