//
// Created by arcady on 04.01.2022.
//
#include "bwebdav.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

bWebDav::bWebDav(QObject *parent, const QString& confFileName)
        : QObject{parent}
{
    m_ssl = false;
    m_rootPath = "remote.php/dav/files";
    m_rootDir = "wsdata";

    settings = ClientSettings(confFileName, false, true);

    updateSettings();
}

void bWebDav::updateSettings() {

    m_host = settings[bConfFieldsWrapper::WebDavHost].toString();
    m_user = settings[bConfFieldsWrapper::WebDavUser].toString();
    m_password = settings[bConfFieldsWrapper::WebDavPwd].toString();
    m_ssl = settings[bConfFieldsWrapper::WebDavSSL].toBool();

}

bool bWebDav::verifyRootDir() {

    auto *gManager = new QNetworkAccessManager;
    QNetworkRequest gRequest;

    if (m_ssl){
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1_0OrLater);
        gRequest.setSslConfiguration(config);
    }

    QString dirPath = getUrlCloud(true, true);
    qDebug() << dirPath;
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
    if(m_user.isEmpty() || m_host.isEmpty() || m_password.isEmpty())
        return {};

    QString chame = "http";

    if(m_ssl)
        chame.append("s");

    QString url;

    std::string m_password_decode = ClientSettings::crypt(m_password.toStdString(), "my_key");

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
