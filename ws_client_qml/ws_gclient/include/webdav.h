#ifndef WEBDAV_H
#define WEBDAV_H

#include <QObject>
#include <QNetworkReply>

class QWebDav : public QObject
{
Q_OBJECT
public:
    explicit QWebDav(QObject *parent = nullptr);

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged);
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged);
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged);
    Q_PROPERTY(QString rootDir READ rootDir WRITE setRootDir NOTIFY rootDirChanged);
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath NOTIFY rootPathChanged);
    Q_PROPERTY(bool ssl READ ssl WRITE setSsl NOTIFY sslChanged)

    void setHost(const QString& val);
    void setUser(const QString& val);
    void setPassword(const QString& val);
    void setRootDir(const QString& val);
    void setRootPath(const QString& val);
    void setSsl(bool val);

    QString currentUrl();
    void setCurrentUrl(const QString& url);

    QString host();
    QString user();
    QString password();
    QString rootDir();
    QString rootPath();
    bool ssl();

    Q_INVOKABLE QString getUrlCloud(bool hostOnly, bool fullPath);
    Q_INVOKABLE void verifyRootDir();
    Q_INVOKABLE void getSettings();
    Q_INVOKABLE void saveSettings();

    Q_INVOKABLE void uploadFile(const QString& token, const QString& file);

    void davError(QNetworkReply::NetworkError type);
    void createDirectory(const QString& dirName, const QString& path);

private:
    QString m_host;
    QString m_user;
    QString m_password;
    QString m_rootDir;
    QString m_rootPath;
    bool m_ssl;
    QString m_currentUrl;



signals:

    void hostChanged();
    void userChanged();
    void passwordChanged();
    void rootDirChanged();
    void rootPathChanged();
    void sslChanged();



};

#endif // WEBDAV_H