#ifndef REGISTRY_H
#define REGISTRY_H

#include <QSettings>
#include <QObject>
#include <QProcess>
#include "user.h"

namespace srtmnr {


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

    static QStringList outputCmd(const QString cmd);

    static std::string exec(const char* cmd);
    static std::string execute( std::string cmd );

    static bool importKeysFromLocalCatalog(CertUser * usr, const QString& folder, const QString& name, QString& error);

};
};
#endif // REGISTRY_H
