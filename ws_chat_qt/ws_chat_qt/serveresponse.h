#ifndef SERVERESPONSE_H
#define SERVERESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class ServeResponse
{
public:
    ServeResponse(const QString& resp);

    QString message;
    QString command;
    QString result;
    bool isParse;

    static QJsonDocument parseResp(const QString& resp);

    static QString base64_decode(const std::string& resp);

private:
    void parse(const QString& resp);
};

#endif // SERVERESPONSE_H
