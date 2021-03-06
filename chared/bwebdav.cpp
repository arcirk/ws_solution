//
// Created by arcady on 04.01.2022.
//
#include "bwebdav.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QFile>
#include <QDir>

bWebDav::bWebDav(QObject *parent, const QString& confFileName)
        : QObject{parent},
          _confFileName(confFileName)
{
    gManager = new QNetworkAccessManager(this);

    m_ssl = false;
    m_rootPath = "remote.php/dav/files";
    m_rootDir = rootDirName;

    updateSettings();
}

void bWebDav::updateSettings() {

    settings = ClientSettings(_confFileName, false, true);
    m_host = settings[bConfFieldsWrapper::WebDavHost].toString();
    m_user = settings[bConfFieldsWrapper::WebDavUser].toString();
    m_password = settings[bConfFieldsWrapper::WebDavPwd].toString();
    m_ssl = settings[bConfFieldsWrapper::WebDavSSL].toBool();

}

bool bWebDav::verifyRootDir() {

    QNetworkRequest gRequest = getRequest();

    QByteArray verb("PROPFIND");
    auto m_reply = gManager->sendCustomRequest(gRequest, verb);
    QObject::connect(m_reply, &QNetworkReply::finished, [=]() {
        if(m_reply->error() == QNetworkReply::NoError){
            QString ans = m_reply->readAll();
            qDebug() << qPrintable(ans);
            qDebug() << "каталог существует!";
            emit verifyRootDirResult(true, true);
        }
        else if (m_reply->error() == QNetworkReply::ContentNotFoundError){
            emit verifyRootDirResult(false, true);
            qDebug() << "каталог не найден!";
        }
        else
            davError(m_reply->error());
    });

    return false;
}

QString bWebDav::getUrlCloud(bool hostOnly, bool fullPath) {

    updateSettings();

    if(m_user.isEmpty() || m_host.isEmpty() || m_password.isEmpty())
        return {};

    QString chame = "http";

    if(m_ssl)
        chame.append("s");

    QString url;
    std::string std_pwd = m_password.toStdString();

    std::string m_password_decode = ClientSettings::crypt(std_pwd, "my_key");

    if(!hostOnly){
        if(fullPath){
            url = QString("%1://%2:%3@%4/%5/%6/%7/").arg(chame, m_user, QString::fromStdString(m_password_decode), m_host, m_rootPath, m_user, m_rootDir);
        }else
            url = QString("%1://%2:%3@%4/").arg(chame, m_user, QString::fromStdString(m_password_decode), m_host);
    }else{
        if(fullPath){
            url =  QString("%1://%2/%3/%4/%5/").arg(chame, m_host, m_rootPath, m_user, m_rootDir);
        }else {
            url =  QString("%1://%2/").arg(chame, m_host);
        }
    }

    return url;

}

void bWebDav::davError(QNetworkReply::NetworkError type)
{
    qDebug() << type;
}

void bWebDav::verify() {
    verifyRootDir();
}

void bWebDav::createDirectory(const QString &name) {

    QNetworkRequest gRequest = getRequest(name);

    QByteArray verb("MKCOL");
    auto m_reply = gManager->sendCustomRequest(gRequest, verb);
    QObject::connect(m_reply, &QNetworkReply::finished, [=]() {

        if(m_reply->error() == QNetworkReply::NoError){
            qDebug() << "create directory:ok";
            emit createDir(true, name);
        }
        else
            davError(m_reply->error());
    });

}

QNetworkRequest bWebDav::getRequest(const QString& addPath) {

    QNetworkRequest gRequest;

    if (m_ssl){
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1_0OrLater);
        gRequest.setSslConfiguration(config);
    }

    QString dirPath = getUrlCloud(true, true);
    if (!addPath.isEmpty())
        dirPath.append(addPath);

    qDebug() << "bWebDav::getRequest::dirPath: " << dirPath;
    QUrl url(dirPath);

    gRequest.setUrl(url);
    gRequest.setRawHeader("Connection", "keep-alive");

    std::string m_password_decode = ClientSettings::crypt(m_password.toStdString(), "my_key");
    QString concatenated = m_user + ":" + QString::fromStdString(m_password_decode);
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    gRequest.setRawHeader("Authorization", headerData.toLocal8Bit());
    gRequest.setRawHeader("Depth", "1");
    gRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");

    return gRequest;
}

QNetworkReply* bWebDav::put(const QString& filePath, QIODevice* data)
{

    QNetworkRequest gRequest = getRequest(filePath);

    QNetworkReply* reply = gManager->put(gRequest, data);

    return reply;
}

QNetworkReply* bWebDav::put(const QString& filePath, const QByteArray& data)
{
    QNetworkRequest gRequest = getRequest(filePath);

    QNetworkReply* reply = gManager->put(gRequest, data);

    return reply;
}

//синхронный вызов
bool bWebDav::exists(const QString &roomToken) {

    QEventLoop loop;

    QNetworkRequest gRequest = getRequest(roomToken);

    QByteArray verb("PROPFIND");

    loop.connect(gManager, SIGNAL(finished(QNetworkReply*)),SLOT(quit()));

    auto m_reply = gManager->sendCustomRequest(gRequest, verb);

    loop.exec();

    //qDebug() << "exit loop";

    return m_reply->error() == QNetworkReply::NoError;
}

void bWebDav::uploadFile(const QString &roomToken, const QString &fileName, const QString &ref) {

    if(!exists(roomToken))
        return;

    if(fileName.isEmpty())
        return;

    QString _fp = fileName;

    if(_fp.startsWith("file:///"))
        _fp = QUrl(fileName).toLocalFile();

    if(!_fp.isEmpty()){
        _fp = QDir::toNativeSeparators(_fp);
    }

    QFile file(_fp);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray _data = file.readAll();

    QString name = QFileInfo(_fp).fileName();

    if(!ref.isEmpty()){
        name.append(".");
        name.append(ref);
    }

    auto reply = put(roomToken + "/" + name,_data);

    QObject::connect(reply, &QNetworkReply::uploadProgress, this, &bWebDav::uploadProgress);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {

        if(reply->error() == QNetworkReply::NoError){
            qDebug() << "bWebDav::uploadFile: finished ";
            emit uploadFinished();
        }
        else
            davError(reply->error());
    });


}

bool bWebDav::createDirectorySynch(const QString &name) {

    QEventLoop loop;
    QNetworkRequest gRequest = getRequest(name);

    QByteArray verb("MKCOL");

    loop.connect(gManager, SIGNAL(finished(QNetworkReply*)),SLOT(quit()));
    auto m_reply = gManager->sendCustomRequest(gRequest, verb);
    loop.exec();
    return m_reply->error() == QNetworkReply::NoError;

}
void bWebDav::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "QWebdav::downloadProgress() = " << bytesReceived << " in " << bytesReceived;
}

void bWebDav::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{

    qDebug() << "QWebdav::uploadProgress() = " << bytesSent << " in " << bytesTotal;

}

void bWebDav::downloadFile(const QString &roomToken, const QString &fileName, const QString &ref, const QString& outputPath) {

    if(!exists(roomToken))
        return;

    if(fileName.isEmpty())
        return;

    if(outputPath.isEmpty())
        return;

    QString _fileName = roomToken;
    _fileName.append("/");
    _fileName.append(fileName);

    if((!ref.isEmpty())){
        _fileName.append(".");
        _fileName.append(ref);
    }



    auto file = new QFile(_fileName, this);

    if (!file->open(QIODevice::WriteOnly)){
        qDebug() << "error open file for write";
        return;
    }

    auto reply = get(_fileName, file, 0);

    QObject::connect(reply, &QNetworkReply::uploadProgress, this, &bWebDav::uploadProgress);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {

        if(reply->error() == QNetworkReply::NoError){
            qDebug() << "bWebDav::uploadFile: finished ";
            emit uploadFinished();
        }
        else
            davError(reply->error());
    });
}
QNetworkReply* bWebDav::get(const QString& path)
{
//    QNetworkRequest req = buildRequest();
//
//    QUrl reqUrl(m_baseUrl);
//    reqUrl.setPath(absolutePath(path));
//
//    req.setUrl(reqUrl);
//
//    return QNetworkAccessManager::get(req);
}

QNetworkReply* bWebDav::get(const QString& path, QIODevice* data)
{
    return get(path, data, 0);
}

QNetworkReply* bWebDav::get(const QString& path, QIODevice* data, quint64 fromRangeInBytes) {

//    auto gRequest = getRequest(path);
//
//    //QNetworkRequest req = buildRequest();
//
//    //QUrl reqUrl(m_baseUrl);
//    //reqUrl.setPath(absolutePath(path));
//
//    //req.setUrl(reqUrl);
//
//    if (fromRangeInBytes>0) {
//        // RFC2616 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
//        QByteArray fromRange = "bytes=" + QByteArray::number(fromRangeInBytes) + "-";   // byte-ranges-specifier
//        gRequest.setRawHeader("Range",fromRange);
//    }
//
//    QNetworkReply* reply = gManager->get(gRequest);//QNetworkAccessManager::get(req);
//    m_inDataDevices.insert(reply, data);
//    connect(reply, &QNetworkReply::readyRead, this, &bWebDav::replyReadyRead);
//    connect(reply, &QNetworkReply::errorOccurred, this, &bWebDav::replyError);
//    connect(reply, &QNetworkReply::downloadProgress, this, &bWebDav::downloadProgress);
//
//    return reply;
}

//void QWebdav::replyReadyRead()
//{
//    auto reply = qobject_cast<QNetworkReply*>(QObject::sender());
//    if (reply->bytesAvailable() < 256000)
//        return;
//
//    QIODevice* dataIO = m_inDataDevices.value(reply, 0);
//    if(dataIO == nullptr)
//        return;
//    dataIO->write(reply->readAll());
//}
//
//void QWebdav::replyFinished(QNetworkReply* reply)
//{
//    disconnect(reply, &QNetworkReply::redirectAllowed, this, &bWebDav::replyReadyRead);
//    disconnect(reply, &QNetworkReply::errorOccurred, this, &bWebDav::replyError);
//
//    QIODevice* dataIO = m_inDataDevices.value(reply, 0);
//    if (dataIO != 0) {
//        dataIO->write(reply->readAll());
//        static_cast<QFile*>(dataIO)->flush();
//        dataIO->close();
//        delete dataIO;
//    }
//    m_inDataDevices.remove(reply);
//
//    QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));
//}
