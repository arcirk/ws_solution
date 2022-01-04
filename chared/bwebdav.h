//
// Created by arcady on 04.01.2022.
//

#ifndef WS_SOLUTION_BWEBDAV_H
#define WS_SOLUTION_BWEBDAV_H

#include <QObject>
#include <QNetworkReply>
#include "clientsettings.h"

class bWebDav : public QObject {
Q_OBJECT
public:
    explicit bWebDav(QObject *parent = nullptr, const QString& confFileName = "");

    void davError(QNetworkReply::NetworkError type);
    void verify();

private:
    QString m_host;
    QString m_user;
    QString m_password;
    QString m_rootDir;
    QString m_rootPath;
    bool m_ssl;
    ClientSettings settings;

    void updateSettings();
    bool verifyRootDir();
    QString getUrlCloud(bool hostOnly, bool fullPath);

signals:
    void verifyRootDirResult(bool result, bool isConnection);
};
#endif //WS_SOLUTION_BWEBDAV_H
