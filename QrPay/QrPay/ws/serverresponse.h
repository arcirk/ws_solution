#ifndef SERVERRESPONSE_H
#define SERVERRESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class ServerResponse
{
public:
    ServerResponse(const QString& resp);

    QString message;
    QString command;
    QString result;
    bool isParse;
    QString recipient;
    QString token;
    QString uuid;
    QString uuid_session;
    QString contentType;
    QString recipientName;
    QString app_name;
    QString ip_address;
    QString host_name;
    QString user_name;

    static QString base64_decode(const QByteArray& resp);
    static QString base64_encode(const QByteArray& resp);


    QString to_string() const;

private:
    void parse(const QString& resp);


};

#endif // SERVERESPONSE_H
