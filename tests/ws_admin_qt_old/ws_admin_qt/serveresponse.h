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
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QAbstractTableModel>

class ServeResponse
{
public:
    ServeResponse(const QString& resp);

    QString message;
    QString command;
    QString result;
    bool isParse;

    static QJsonDocument parseResp(const QString& resp);

    static void loadTableFromJson(QTableWidget* table, const QJsonObject& json);
    static void loadTableFromJson(QTableWidget* table, const QJsonArray& arr);
    static void loadTableFromJson(QTableWidget *table, const QJsonArray& array, const QMap<QString, int> &header);

    static void set_header_tree(QTreeWidget* tree, const QJsonObject& jsonObject);
    static int get_index_member(const QJsonObject& jsonObject, const QString& memberKey);

    static QSortFilterProxyModel * get_proxyModel(const QString& resp);
    static QSortFilterProxyModel *get_proxyModel(QJsonDocument &doc, QMap<QString, int> header);

    static QMap<QString, int> get_header(QJsonObject *obj, QString firstCol);

private:

    void parse(const QString& resp);

};

#endif // SERVERESPONSE_H
