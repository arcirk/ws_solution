//
// Created by arcady on 08.01.2022.
//
#include "qwebdav.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QEventLoop>
#include <QFile>
#include <QDir>

QWebdav::QWebdav(QObject *parent, const QString &confFileName)
: QNetworkAccessManager(parent) ,
  _confFileName(confFileName)
{
    m_ssl = false;
    m_rootPath = "remote.php/dav/files";
    m_rootDir = rootDirName;

    connect(this, &QWebdav::finished, this, &QWebdav::replyFinished);

    resetSettings();
}

void QWebdav::resetSettings() {

    settings = ClientSettings(_confFileName, false, true);
    m_host = settings[bConfFieldsWrapper::WebDavHost].toString();
    m_user = settings[bConfFieldsWrapper::WebDavUser].toString();
    m_password = settings[bConfFieldsWrapper::WebDavPwd].toString();
    m_ssl = settings[bConfFieldsWrapper::WebDavSSL].toBool();

}

QWebdav::~QWebdav() {

}

QNetworkReply *QWebdav::get(const QString &path) {

    QNetworkRequest gRequest = getRequest(path);
    return QNetworkAccessManager::get(gRequest);
}

QNetworkReply *QWebdav::get(const QString &path, QIODevice *data) {
    return get(path, data, 0);
}

QNetworkReply *QWebdav::get(const QString &path, QIODevice *data, quint64 fromRangeInBytes) {

    QNetworkRequest gRequest = getRequest(path);

    if (fromRangeInBytes>0) {
        // RFC2616 http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        QByteArray fromRange = "bytes=" + QByteArray::number(fromRangeInBytes) + "-";   // byte-ranges-specifier
        gRequest.setRawHeader("Range",fromRange);
    }

    QNetworkReply* reply = QNetworkAccessManager::get(gRequest);

    m_inDataDevices.insert(reply, data);

    connect(reply, &QNetworkReply::readyRead, this, &QWebdav::replyReadyRead);
    connect(reply, &QNetworkReply::errorOccurred, this, &QWebdav::replyError);
    connect(reply, &QNetworkReply::downloadProgress, this, &QWebdav::onDownloadProgress);

    return reply;
}

QNetworkReply *QWebdav::put(const QString &path, QIODevice *data) {

    QNetworkRequest gRequest = getRequest(path);

    QNetworkReply* reply = QNetworkAccessManager::put(gRequest, data);
    connect(reply, &QNetworkReply::uploadProgress, this, &QWebdav::onUploadProgress);
    connect(reply, &QNetworkReply::errorOccurred, this, &QWebdav::replyError);

    return reply;
}

QNetworkReply *QWebdav::put(const QString &path, const QByteArray &data) {

    QNetworkRequest gRequest = getRequest(path);

    QNetworkReply* reply = QNetworkAccessManager::put(gRequest, data);
    connect(reply, &QNetworkReply::uploadProgress, this, &QWebdav::onUploadProgress);
    connect(reply, &QNetworkReply::errorOccurred, this, &QWebdav::replyError);

    return reply;
}

QNetworkReply *QWebdav::mkdir(const QString &dir) {

    QNetworkRequest gRequest = getRequest(dir);
    QByteArray verb("MKCOL");
    return this->sendCustomRequest(gRequest, verb);
}

bool QWebdav::mkdirSynch(const QString &dir) {

    QEventLoop loop;
    QNetworkRequest gRequest = getRequest(dir);

    QByteArray verb("MKCOL");

    loop.connect(this, SIGNAL(finished(QNetworkReply*)),SLOT(quit()));
    auto m_reply = this->sendCustomRequest(gRequest, verb);
    loop.exec();

    bool result = m_reply->error() == QNetworkReply::NoError;
    if(!result)
        emit errorChanged(m_reply->error(), m_reply->errorString());

    return result;
}

QNetworkReply *QWebdav::copy(const QString &pathFrom, const QString &pathTo, bool overwrite) {
    return nullptr;
}

QNetworkReply *QWebdav::move(const QString &pathFrom, const QString &pathTo, bool overwrite) {
    return nullptr;
}

QNetworkReply *QWebdav::remove(const QString &path) {
    return nullptr;
}

void QWebdav::replyReadyRead() {
    auto reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply->bytesAvailable() < 256000)
        return;

    QIODevice* dataIO = m_inDataDevices.value(reply, 0);
    if(dataIO == nullptr)
        return;
    dataIO->write(reply->readAll());
}

void QWebdav::replyFinished(QNetworkReply * reply) {

    disconnect(reply, &QNetworkReply::redirectAllowed, this, &QWebdav::replyReadyRead);
    disconnect(reply, &QNetworkReply::errorOccurred, this, &QWebdav::replyError);

    QIODevice* dataIO = m_inDataDevices.value(reply, 0);
    if (dataIO != 0) {
        dataIO->write(reply->readAll());
        static_cast<QFile*>(dataIO)->flush();
        dataIO->close();
        delete dataIO;
    }
    m_inDataDevices.remove(reply);

    QMetaObject::invokeMethod(this,"replyDeleteLater", Qt::QueuedConnection, Q_ARG(QNetworkReply*, reply));

    qDebug() << "QWebdav::replyFinished";
}

void QWebdav::replyDeleteLater(QNetworkReply * reply) {
    QIODevice *outDataDevice = m_outDataDevices.value(reply, 0);
    if (outDataDevice!=nullptr)
        outDataDevice->deleteLater();
    m_outDataDevices.remove(reply);
}

void QWebdav::replyError(QNetworkReply::NetworkError type) {

    auto reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (reply==nullptr)
        return;

    if ( reply->error() == QNetworkReply::OperationCanceledError) {
        QIODevice* dataIO = m_inDataDevices.value(reply, 0);
        if (dataIO!=nullptr) {
            delete dataIO;
            m_inDataDevices.remove(reply);
        }
        return;
    }

    emit errorChanged(type, reply->errorString());
}

void QWebdav::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    //qDebug() << "QWebdav::downloadProgress() = " << bytesReceived << " in " << bytesReceived;
    emit downloadProgress(bytesReceived, bytesTotal);
}

void QWebdav::onUploadProgress(qint64 bytesSent, qint64 bytesTotal) {
    //qDebug() << "QWebdav::uploadProgress() = " << bytesSent << " in " << bytesTotal;
    emit uploadProgress(bytesSent, bytesTotal);
}

QString QWebdav::getUrlCloud(bool hostOnly, bool fullPath) {

    //resetSettings();

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
            url = QString("%1://%2:%3@%4/%5/%6/").arg(chame, m_user, QString::fromStdString(m_password_decode), m_host, m_rootPath, m_user);
    }else{
        if(fullPath){
            url =  QString("%1://%2/%3/%4/%5/").arg(chame, m_host, m_rootPath, m_user, m_rootDir);
        }else {
            url =  QString("%1://%2/%3/%4/").arg(chame, m_host, m_rootPath, m_user);
        }
    }

    return url;

}

QNetworkRequest QWebdav::getRequest(const QString &addPath, bool fullPath) {

    QNetworkRequest gRequest;

    if (m_ssl){
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1_0OrLater);
        gRequest.setSslConfiguration(config);
    }

    QString dirPath = getUrlCloud(true, fullPath);
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

bool QWebdav::exists(const QString &path) {
    QEventLoop loop;

    QNetworkRequest gRequest = getRequest(path, true);

    QByteArray verb("PROPFIND");

    loop.connect(this, SIGNAL(finished(QNetworkReply*)),SLOT(quit()));

    auto m_reply = this->sendCustomRequest(gRequest, verb);

    loop.exec();

    return m_reply->error() == QNetworkReply::NoError;
}

bool QWebdav::rootExists() {
    return exists("");
}

bool QWebdav::isConnections() {

    QEventLoop loop;

    QNetworkRequest gRequest = getRequest("", false);

    QByteArray verb("PROPFIND");

    loop.connect(this, SIGNAL(finished(QNetworkReply*)),SLOT(quit()));

    auto m_reply = this->sendCustomRequest(gRequest, verb);

    loop.exec();

    return m_reply->error() == QNetworkReply::NoError;
}

void QWebdav::setUser(const QString &usr) {
    m_user = usr;
}

void QWebdav::setPassword(const QString &pwd) {
    m_password = QString::fromStdString(ClientSettings::crypt(pwd.toStdString(), "my_key"));
}

void QWebdav::setHost(const QString &host) {
    m_host = host;
}

void QWebdav::setSsl(bool ssl) {
    m_ssl = ssl;
}

void QWebdav::uploadFile(const QString &roomToken, const QString &fileName, const QString &ref) {

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
    connect(reply, &QNetworkReply::finished, [=]() {

        if(reply->error() == QNetworkReply::NoError){
            emit uploadFinished();
        }
        else
            emit uploadError();

    });
}

void
QWebdav::downloadFile(const QString &roomToken, const QString &fileName, const QString &ref, const QString &outputDir) {

    if(roomToken.isEmpty() || fileName.isEmpty() || outputDir.isEmpty()){
        emit downloadError();
        return;
    }

    QString _fp = outputDir;

    if(_fp.startsWith("file:///"))
        _fp = QUrl(outputDir).toLocalFile();

    if(!_fp.isEmpty()){
        _fp = QDir::toNativeSeparators(_fp);
    }
    _fp.append(QDir::separator());
    _fp.append(fileName);
    auto file = new QFile(_fp, this);
    if (!file->open(QIODevice::WriteOnly)){
        qDebug() << "error open file for write";
        emit downloadError();
        return;
    }

    QString _fileName = roomToken;
    _fileName.append("/");
    _fileName.append(fileName);

    if((!ref.isEmpty())){
        _fileName.append(".");
        _fileName.append(ref);
    }

    auto reply = get(_fileName, file, 0);
    connect(reply, &QNetworkReply::finished, [=]() {

        if(reply->error() == QNetworkReply::NoError){
            //qDebug() << "bWebDav::downloadFile: finished ";
            emit downloadFinished();
        }
        else
            emit downloadError();
            //qDebug() << "bWebDav::downloadFile: error ";
    });
}
