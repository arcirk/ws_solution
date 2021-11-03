#include "serveresponse.h"

#include <iws_client.h>

#include <QStandardItemModel>


ServeResponse::ServeResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

QString ServeResponse::base64_decode(const std::string &resp)
{
    std::string json;
    try {
        json = arc_json::base64_decode(resp);
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

             if(_message->isString()){
                 message = _message.value().toString();
             }
             if(_command->isString()){
                 command = _command.value().toString();
             }
             if(_result->isString()){
                 result = _result.value().toString();
             }

            isParse = true;
        }
    }

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
                itemVal = new QStandardItem(QString::number(col.value().toInteger()));
            else if (col.value().isBool())
                itemVal = new QStandardItem(QString::number(col.value().toBool()));
            model->setItem(index, indexCol, itemVal);

        }
        index++;
    }
    proxyModel->setSourceModel(model);

    return proxyModel;
}
