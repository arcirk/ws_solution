//
// Created by arcady on 08.01.2022.
//

#ifndef WS_SOLUTION_QWEBDAV_H
#define WS_SOLUTION_QWEBDAV_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "clientsettings.h"

class QWebdav : public QNetworkAccessManager {
    Q_OBJECT
public:
    explicit QWebdav(QObject* parent = nullptr, const QString& confFileName = "");
    ~QWebdav() override;
    QNetworkReply* get(const QString& path);
    QNetworkReply* get(const QString& path, QIODevice* data);
    QNetworkReply* get(const QString& path, QIODevice* data, quint64 fromRangeInBytes);
    void downloadFile(const QString &roomToken, const QString &fileName, const QString &ref, const QString& outputDir);

    QNetworkReply* put(const QString& path, QIODevice* data);
    QNetworkReply* put(const QString& path, const QByteArray& data );
    void uploadFile(const QString &roomToken, const QString &fileName, const QString &ref);

    QNetworkReply* mkdir(const QString& dir );
    bool mkdirSynch(const QString& dir );

    QNetworkReply* copy(const QString& pathFrom, const QString& pathTo, bool overwrite = false);
    QNetworkReply* move(const QString& pathFrom, const QString& pathTo, bool overwrite = false);
    QNetworkReply* remove(const QString& path );

    //synch
    bool exists(const QString& path);
    bool rootExists();
    bool isConnections();

    const QString rootDirName = "wsdata";

    void  setUser(const QString& usr);
    void  setPassword(const QString& pwd);
    void  setHost(const QString& host);
    void  setSsl(bool ssl);

    void resetSettings();

protected slots:
    void replyReadyRead();
    void replyFinished(QNetworkReply*);
    void replyDeleteLater(QNetworkReply*);
    void replyError(QNetworkReply::NetworkError);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);

signals:
    void errorChanged(QNetworkReply::NetworkError type, const QString& error);
    void downloadFinished();
    void uploadFinished();
    void downloadError();
    void uploadError();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
private:
    QString m_host;
    QString m_user;
    QString m_password;
    QString m_rootDir;
    QString m_rootPath;
    bool m_ssl;
    QString _confFileName;
    ClientSettings settings;
    QMap<QNetworkReply*, QIODevice*> m_outDataDevices;
    QMap<QNetworkReply*, QIODevice*> m_inDataDevices;


    QNetworkRequest getRequest(const QString &addPath, bool fullPath = true);
    QString getUrlCloud(bool hostOnly, bool fullPath);
};


#endif //WS_SOLUTION_QWEBDAV_H
