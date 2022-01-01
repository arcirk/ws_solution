#include "../include/webdav.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QDir>


QWebDav::QWebDav(QObject *parent)
        : QObject{parent}
{

    m_ssl = false;
    m_rootPath = "remote.php/dav/files";
    m_rootDir = "wsdata";

    getSettings();

}

void QWebDav::setHost(const QString &val)
{
    m_host = val;
}

void QWebDav::setUser(const QString &val)
{
    m_user = val;
}

void QWebDav::setPassword(const QString &val)
{
    m_password = val;
}

void QWebDav::setRootDir(const QString &val)
{
    m_rootDir = val;
}

void QWebDav::setRootPath(const QString &val)
{
    m_rootPath = val;
}

void QWebDav::setSsl(bool val)
{
    m_ssl = val;
}

QString QWebDav::currentUrl()
{
    return m_currentUrl;
}

void QWebDav::setCurrentUrl(const QString &url)
{
    m_currentUrl = url;
}

QString QWebDav::host()
{
    return m_host;
}

QString QWebDav::user()
{
    return m_user;
}

QString QWebDav::password()
{
    return m_password;
}

QString QWebDav::rootDir()
{
    return m_rootDir;
}

QString QWebDav::rootPath()
{
    return m_rootPath;
}

bool QWebDav::ssl()
{
    return m_ssl;
}

QString QWebDav::getUrlCloud(bool hostOnly, bool fullPath)
{
    if(m_user.isEmpty() || m_host.isEmpty() || m_password.isEmpty())
        return QString();

    QString chame = "http";

    if(m_ssl)
        chame.append("s");

    QString url;

    if(!hostOnly){
        if(fullPath){
            url = QString("%1://%2:%3@%4/%5/%6/%7/").arg(chame, m_user, m_password, m_host, rootPath(), m_user, rootDir());
        }else
            url = QString("%1://%2:%3@%4/").arg(chame, m_user, m_password, m_host);
    }else{
        if(fullPath){
            url =  QString("%1://%2/%3/%4/%5/").arg(chame, m_host, rootPath(), m_user, rootDir());
        }else {
            url =  QString("%1://%2/").arg(chame, m_host);
        }
    }



    return url;
}

void QWebDav::verifyRootDir()
{

    QNetworkAccessManager *gManager = new QNetworkAccessManager;
//    QFile file("/home/arcady/tmp/qt_tests/webdav/build-webdavtests-Desktop_Qt_6_2_1_GCC_64bit-Debug/ssl/arcirk_ru.crt");
//    file.open(QIODevice::ReadOnly);
//    QByteArray bytes = file.readAll();

    QNetworkRequest gRequest;

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0OrLater);

    gRequest.setSslConfiguration(config);

    //QString url_s = getUrlCloud(false);
    // QUrl url(url_s);
    QUrl url(m_currentUrl);

    gRequest.setUrl(url);
    gRequest.setRawHeader("Connection", "keep-alive");

    //QString authorization = QString("%1:%2").arg(m_user, m_password).toLocal8Bit().toBase64();
    QString concatenated = m_user + ":" + m_password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    gRequest.setRawHeader("Authorization", headerData.toLocal8Bit());
    gRequest.setRawHeader("Depth", "1");
    gRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    auto m_reply = gManager->sendCustomRequest(gRequest, "PROPFIND");

//    QNetworkAccessManager nam;
//    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
//    QNetworkRequest req;
//    req.setUrl(QUrl(getUrlCloud(false)));
//    req.setRawHeader("Depth", "1");
//    req.setRawHeader("Authorization", QString("Basic %1").arg(authorization).toLocal8Bit());
//    req.setRawHeader("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");

//    auto reply = nam.sendCustomRequest(req, "PROPFIND");
//    qDebug() << reply->readAll();


    QObject::connect(m_reply, &QNetworkReply::finished, [=]() {
        //qDebug().noquote().nospace() << m_reply->readAll();
        //m_reply->deleteLater();
//        qDebug() <<"\n"<<"in sendReplyFinished";
//        qDebug() <<m_reply->error()<<"\n";

        if(m_reply->error() == QNetworkReply::NoError){
//            QString ans = m_reply->readAll();
//            qDebug() <<ans;
            qDebug() << "root dir exists";
        }else if(m_reply->error() == QNetworkReply::ContentNotFoundError){
            delete gManager;
            createDirectory("", "");
        }
        else{
            davError(m_reply->error());
        }

    });
//    QObject::connect(reply, &QNetworkReply::errorOccurred, [=]() {
//        qDebug().noquote().nospace() << m_reply->readAll();
//        m_reply->deleteLater();
//      });

}

void QWebDav::getSettings()
{
    QJsonObject m_obj = QJsonObject();

    QString m_file = "webdav.json";

    QFileInfo fileInfo(m_file);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(m_file);
    if (!jsonFile.open(QIODevice::ReadOnly))
    {
        return;
    }

    QByteArray saveData = jsonFile.readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    jsonFile.close();

    m_obj = jsonDocument.object();

    auto iter = m_obj.find("DavAddress");
    if (iter != m_obj.end()){
        m_host = iter.value().toString();
    }
    iter = m_obj.find("DavUser");
    if (iter != m_obj.end()){
        m_user = iter.value().toString();
    }
    iter = m_obj.find("DavPassword");
    if (iter != m_obj.end()){
        m_password = iter.value().toString();
    }
    iter = m_obj.find("DavRootDir");
    if (iter != m_obj.end()){
        m_rootDir = iter.value().toString();
    }
    iter = m_obj.find("DavRootPath");
    if (iter != m_obj.end()){
        m_rootPath = iter.value().toString();
    }
    iter = m_obj.find("DavSSL");
    if (iter != m_obj.end()){
        m_ssl = iter.value().toBool();
    }

}

void QWebDav::saveSettings()
{
    QJsonObject m_obj = QJsonObject();

    QString m_file = "webdav.json";

    QFileInfo fileInfo(m_file);
    QDir::setCurrent(fileInfo.path());


    QFile jsonFile(m_file);
    if (!jsonFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        m_obj = QJsonObject();
    }else{
        QByteArray saveData = jsonFile.readAll();
        QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
        m_obj = jsonDocument.object();
    }

    auto iter = m_obj.find("DavAddress");
    if (iter != m_obj.end()){
        m_obj.value("DavAddress") = m_host;
    }else {
        m_obj.insert("DavAddress", m_host);
    }
    iter = m_obj.find("DavUser");
    if (iter != m_obj.end()){
        m_obj.value("DavUser") = m_user;
    }else {
        m_obj.insert("DavUser", m_user);
    }
    iter = m_obj.find("DavPassword");
    if (iter != m_obj.end()){
        m_obj.value("DavPassword") = m_password;
    }else {
        m_obj.insert("DavPassword", m_password);
    }
    iter = m_obj.find("DavRootDir");
    if (iter != m_obj.end()){
        m_obj.value("DavRootDir") = m_rootDir;
    }else {
        m_obj.insert("DavRootDir", m_rootDir);
    }
    iter = m_obj.find("DavRootPath");
    if (iter != m_obj.end()){
        m_rootPath = iter.value().toString();
        m_obj.value("DavRootPath") = m_rootPath;
    }else {
        m_obj.insert("DavRootPath", m_rootPath);
    }
    iter = m_obj.find("DavSSL");
    if (iter != m_obj.end()){
        m_obj.value("m_ssl") = m_ssl;
    }else {
        m_obj.insert("DavSSL", m_ssl);
    }

    jsonFile.write(QJsonDocument(m_obj).toJson(QJsonDocument::Indented));
    jsonFile.close();
}

void QWebDav::davError(QNetworkReply::NetworkError type)
{
    qDebug() << type;
}

void QWebDav::createDirectory(const QString &dirName, const QString &path)
{
    QNetworkAccessManager *gManager = new QNetworkAccessManager;
    QNetworkRequest gRequest;

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0OrLater);

    gRequest.setSslConfiguration(config);

    QString dirPath = getUrlCloud(false, true);//m_currentUrl;
    if(!path.isEmpty()){
        dirPath.append(path + "/");
    }
    if(!dirName.isEmpty()){
        dirPath.append(dirName);
    }

    qDebug() << dirPath;

    QUrl url(dirPath);

    //gManager-> setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);

    gRequest.setUrl(url);
    //gRequest.setRawHeader("Connection", "keep-alive");

//    QString concatenated = m_user + ":" + m_password;
//    QByteArray data = concatenated.toLocal8Bit().toBase64();
//   QString headerData = "Basic " + data;
//    gRequest.setRawHeader("Authorization", headerData.toLocal8Bit());
    //gRequest.setRawHeader("Depth", "1");
    //gRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    //gManager->sendCustomRequest(gRequest, "MKCOL");
    gRequest.setAttribute(QNetworkRequest::User, QVariant("mkcol"));
    QByteArray verb("MKCOL");
    auto m_reply = gManager->sendCustomRequest(gRequest, verb);
    QObject::connect(m_reply, &QNetworkReply::finished, [=]() {

        if(m_reply->error() == QNetworkReply::NoError){
//            QString ans = m_reply->readAll();
//            qDebug() <<ans;
            qDebug() << "create directory:ok";
        }else
            davError(m_reply->error());
    });
}