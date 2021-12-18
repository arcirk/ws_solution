#ifndef SERVERESPONSE_H
#define SERVERESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSortFilterProxyModel>

class ServeResponse
{
public:
    ServeResponse(const QString& resp);

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

    static QJsonDocument parseResp(const QString& resp);

    static QString base64_decode(const std::string& resp);
    static QMap<QString, int> get_header(QJsonObject *obj, QString defaultColumn);
    static QMap<QString, int> get_header(QJsonArray *columns);
    static QSortFilterProxyModel * get_proxyModel(QJsonArray &rows, QMap<QString, int> header);

    static void debugSaveResponse(const QString& filename, const QString& json = "");

    QString to_string() const;

private:
    void parse(const QString& resp);


};

#endif // SERVERESPONSE_H
