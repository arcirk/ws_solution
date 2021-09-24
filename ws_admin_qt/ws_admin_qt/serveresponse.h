#ifndef SERVERESPONSE_H
#define SERVERESPONSE_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>
#include <QTreeWidget>

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

    static QJsonDocument parseResp(const QString& resp);

    static void loadTableFromJson(QTableWidget* table, const QString& json);
    static void loadTableFromJson(QTableWidget* table, const QJsonObject& json);
    static void loadTableFromJson(QTableWidget* table, QJsonArray arr);

    static void set_header_tree(QTreeWidget* tree, const QJsonObject& jsonObject);
    static QTreeWidgetItem *  new_tree_item(const QJsonObject& jsonObject);
    static int get_index_member(const QJsonObject& jsonObject, const QString& memberKey);

private:

    void parse(const QString& resp);

};

#endif // SERVERESPONSE_H
