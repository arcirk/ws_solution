#include "../include/serveresponse.h"

#include <iws_client.h>

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>
#include <QTableWidgetItem>

ServeResponse::ServeResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

QString ServeResponse::base64_decode(const std::string &resp)
{
    std::string json;
    try {
        json = IClient::base64_decode(resp);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    return QString::fromStdString(json);
}

void ServeResponse::parse(const QString& resp){


    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    if(!_doc.isNull())
    {
        if(_doc.isObject())
        {
            QJsonObject obj = _doc.object();
            auto _message = obj.find("message");
            auto _result = obj.find("result");
            auto _command = obj.find("command");
            auto _recipient = obj.find("uuid_channel");
            auto _uuid = obj.find("user_uuid");
            auto _contentType = obj.find("contentType");
            auto _recipientName = obj.find("channel_name");

             if(_message->isString()){
                 message = _message.value().toString();
             }
             if(_command->isString()){
                 command = _command.value().toString();
             }
             if(_result->isString()){
                 result = _result.value().toString();
             }
             if(_recipient->isString()){
                 recipient = _recipient.value().toString();
             }
             if(_uuid->isString()){
                 uuid = _uuid.value().toString();
             }
             if(_contentType->isString()){
                 contentType = _contentType.value().toString();
             }
             if(_contentType->isString()){
                 recipientName = _recipientName.value().toString();
             }
            isParse = true;
        }
    }

}

void ServeResponse::debugSaveResponse(const QString &filename, const QString &json)
{
    //ToDo: временная процедура
    QString saveFileName = filename + ".json";
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument _doc(QJsonDocument::fromJson(json.toUtf8()));
    jsonFile.write(_doc.toJson(QJsonDocument::Indented));
    jsonFile.close();

}

QString ServeResponse::to_string() const
{
//    QJsonDocument doc;
//    QJsonObject msg = QJsonObject();
//    msg.insert("FirstField", "f3ccb2f2-d431-11e9-ab42-08606e7d17fa");
//    msg.insert("Ref", "d3db7a39-c2b1-4ccb-ab67-a4f82f85a25d");
//    msg.insert("SecondField", "d81ade18-fb07-11e2-b8bf-08606e7d17fa");
//    msg.insert("_id", 0);
//    msg.insert("date", 1630075939);
//    msg.insert("message", sz_msg);
//    msg.insert("token", "036a82ff52b0e5725e202ff17a5f82b1a1226141");
//    msg.insert("contentType", "HTML");
    return QString();
}

QJsonDocument ServeResponse::parseResp(const QString &resp){

    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    return _doc;

}
QMap<QString, int> ServeResponse::get_header(QJsonArray *columns){

    QMap<QString, int> header;

    for (int i = 0; i < columns->count(); ++i) {
        header.insert(columns->at(i).toString(), i);
    }

    return header;
}

QMap<QString, int> ServeResponse::get_header(QJsonObject *obj, QString defaultColumn) {

    QList<QString> list;
    QMap<QString, int> header;
    int index = 0;

    for (auto col = obj->constBegin(); col != obj->constEnd(); col++)
    {
        if (defaultColumn.isEmpty()){
            header.insert(col.key(), index);
            index++;
        }else{
            if (col.key() != defaultColumn)
                list.push_back(col.key());
            else
                list.insert(0, col.key());
        }
    }

    if (!list.empty()){
        for (int j = 0; j < list.count(); ++j) {
            header.insert(list.at(j), j);
        }
    }

    return header;
}

QSortFilterProxyModel * ServeResponse::get_proxyModel(QJsonArray &rows, QMap<QString, int> header) {

    if (rows.isEmpty())
        return nullptr;

    //QJsonArray array = doc.array();
    int rowCount = (int)rows.count();
    int colCount = (int)header.size();
    auto * model = new QStandardItemModel(rowCount, colCount);
    int index = 0;

    foreach ( const auto& Key, header.keys() )
    {
        const auto& Value = header[Key];
        model->setHorizontalHeaderItem(Value, new QStandardItem(Key));
    }

    index = 0;

    auto proxyModel = new QSortFilterProxyModel();

    for (auto it = rows.constBegin(); it != rows.constEnd(); ++it)
    {
        QJsonObject currentObject = it->toObject();
        for (auto col = currentObject.constBegin(); col != currentObject.constEnd(); ++col){
            const auto& indexCol = header[col.key()];;
            QStandardItem *itemVal = nullptr;
            if (col.value().isString())
                itemVal = new QStandardItem(col.value().toString());
            else if (col.value().isDouble())
                itemVal = new QStandardItem(QString::number(col.value().toInt()));
            else if (col.value().isBool())
                itemVal = new QStandardItem(QString::number(col.value().toBool()));
            model->setItem(index, indexCol, itemVal);

        }
        index++;
    }
    proxyModel->setSourceModel(model);

    return proxyModel;
}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonObject& json) {

    table->setColumnCount(2);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Свойство"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Значение"));

    int index = 0;

    table->setRowCount((int)json.count());

    for (auto it = json.constBegin(); it != json.constEnd(); ++it)
    {
        auto itemKey = new QTableWidgetItem(it.key());
        itemKey->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(index, 0, itemKey);

        QTableWidgetItem *itemVal = nullptr;
        if (it.value().isString())
            itemVal = new QTableWidgetItem(it.value().toString());
        else if (it.value().isDouble())
            itemVal = new QTableWidgetItem(QString::number(it.value().toInteger()));
        else if (it.value().isBool())
            itemVal = new QTableWidgetItem(QString::number(it.value().toBool()));

        if (itemVal){
            itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(index, 1, itemVal);
        }

        index++;
    }

}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonArray& array) {

    auto rowCount = array.count();
    table->setRowCount((int)rowCount);

    int colCount = 0;

    int index = 0;
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if(it->isObject()){
            QJsonObject currentObject = it->toObject();
            int indexCol = 0;
            if (index == 0){
                colCount = (int)currentObject.count();
                table->setColumnCount(colCount);
                for (auto col = currentObject.constBegin(); col != currentObject.constEnd(); ++col)
                {
                    table->setHorizontalHeaderItem(indexCol, new QTableWidgetItem(col.key()));
                    indexCol++;
                }
                table->setColumnCount(colCount);

            }else
            {
                if (colCount != currentObject.count() ){
                    table->clear();
                    table->setRowCount(0);
                    table->setColumnCount(0);
                    return;
                }
            }
            indexCol = 0;
            for (auto rowVal = currentObject.constBegin(); rowVal != currentObject.constEnd(); ++rowVal)
            {
                QTableWidgetItem *itemVal = nullptr;
                if (rowVal.value().isString())
                    itemVal = new QTableWidgetItem(rowVal.value().toString());
                else if (rowVal.value().isDouble())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toInteger()));
                else if (rowVal.value().isBool())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toBool()));

                if (itemVal){
                    itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    table->setItem(index, indexCol, itemVal);
                }
                indexCol++;
            }
            index++;
        }
    }

}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonArray& array, const QMap<QString, int>&header){

    auto rowCount = array.count();
    table->setRowCount((int)rowCount);
    table->setColumnCount((int)header.count());

    for (auto col = header.constBegin(); col != header.constEnd(); ++col)
    {
        const QString& key = col.key();
        table->setHorizontalHeaderItem(col.value(), new QTableWidgetItem(key));
    }

    int index = 0;
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if(it->isObject()){
            QJsonObject currentObject = it->toObject();
            for (const auto& col : header.keys())
            {
                const QString& key = col;
                auto rowVal = currentObject.find(key);

                QTableWidgetItem *itemVal = nullptr;
                if (rowVal.value().isString())
                    itemVal = new QTableWidgetItem(rowVal.value().toString());
                else if (rowVal.value().isDouble())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toInteger()));
                else if (rowVal.value().isBool())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toBool()));

                if (itemVal){
                    itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    table->setItem(index, header[col], itemVal);
                }
            }

            index++;
        }
    }

}
