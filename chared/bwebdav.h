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
    bool createDirectorySynch(const QString& name);

    const QString rootDirName = "wsdata";
    bool exists(const QString& roomToken);
    void uploadFile(const QString& roomToken, const QString& fileName, const QString &ref);
    void downloadFile(const QString& roomToken, const QString& fileName, const QString &ref, const QString& outputPath);

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
    QMap<QNetworkReply*, QIODevice*> m_inDataDevices;

    void updateSettings();
    bool verifyRootDir();
    QString getUrlCloud(bool hostOnly, bool fullPath);

    QNetworkRequest getRequest(const QString& addPath = "");
    QNetworkReply* put(const QString& filePath, QIODevice* data);
    QNetworkReply* put(const QString& filePath, const QByteArray& data);
    QNetworkReply* get(const QString& path);
    QNetworkReply* get(const QString& path, QIODevice* data);
    QNetworkReply* get(const QString& path, QIODevice* data, quint64 fromRangeInBytes);



signals:
    void verifyRootDirResult(bool result, bool isConnection);
    void createDir(bool result, const QString& name);
    void uploadFinished();

protected slots:
//    void replyReadyRead();
//    void replyFinished(QNetworkReply*);
//    void replyDeleteLater(QNetworkReply*);
//    void replyError(QNetworkReply::NetworkError);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
};
#endif //WS_SOLUTION_BWEBDAV_H
