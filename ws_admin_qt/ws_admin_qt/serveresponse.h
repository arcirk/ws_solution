#ifndef SERVERESPONSE_H
#define SERVERESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

class ServeResponse
{
public:
    ServeResponse(const QString& resp);

    QString message;
    QString command;
    QString result;
    bool isParse;

    bool read_server_response(const QString& json);

    static QString getStringMember(QJsonDocument& doc, const QString& key);
    static QString getStringMember(QJsonObject &obj, const QString& key);

    //static QVariant<QString int> getMember(QJsonDocument& doc, const QString& key);

    static QJsonDocument parseResp(const QString& resp);

private:

    void parse(const QString& resp);

};

#endif // SERVERESPONSE_H
