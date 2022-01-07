//
// Created by arcady on 04.01.2022.
//

#ifndef WS_SOLUTION_BWEBDAV_H
#define WS_SOLUTION_BWEBDAV_H

#include <QObject>
#include <QNetworkReply>
#include "clientsettings.h"
#include <QNetworkAccessManager>

class bWebDav : public QObject {
Q_OBJECT
public:
    explicit bWebDav(QObject *parent = nullptr, const QString& confFileName = "");

    void davError(QNetworkReply::NetworkError type);
    void verify();
    void createDirectory(const QString& name);

    const QString rootDirName = "wsdata";

private:
    ClientSettings settings;
    QNetworkAccessManager * gManager;

    QString m_host;
    QString m_user;
    QString m_password;
    QString m_rootDir;
    QString m_rootPath;
    bool m_ssl;
    QString _confFileName;


    void updateSettings();
    bool verifyRootDir();
    QString getUrlCloud(bool hostOnly, bool fullPath);

    QNetworkRequest getRequest(const QString& addPath = "");
    void put(const QString& roomToken, const QString& addPath);
    void get(const QString& roomToken, const QString& fileName);

signals:
    void verifyRootDirResult(bool result, bool isConnection);
    void createDir(bool result, const QString& name);
};
#endif //WS_SOLUTION_BWEBDAV_H
