#include "serveresponse.h"
#include <QHeaderView>

ServeResponse::ServeResponse(const QString& resp)
{

    isParse = false;
    parse(resp);
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

        if (itemVal){
            itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(index, 1, itemVal);
        }

        index++;
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

                if (itemVal){
                    itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    table->setItem(index, header[col], itemVal);
                }
            }

            index++;
        }
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

void ServeResponse::set_header_tree(QTreeWidget *tree, const QJsonObject& jsonObject) {

    tree->setColumnCount((int)jsonObject.count());
    auto * item = new QTreeWidgetItem();
    int index = 0;
    for (auto col = jsonObject.constBegin(); col != jsonObject.constEnd(); ++col)
    {
        item->setText(index, col.key());
        item->setToolTip(index, col.key());
        index++;
    }
    tree->setHeaderItem(item);
}

int ServeResponse::get_index_member(const QJsonObject &jsonObject, const QString &memberKey) {
    int result = -1;
    int index = 0;
    for (auto col = jsonObject.constBegin(); col != jsonObject.constEnd(); ++col)
    {
        if (col.key() == memberKey){
            return index;
        }
        index++;
    }
    return index;
}

QSortFilterProxyModel * ServeResponse::get_proxyModel(QJsonDocument &doc, QMap<QString, int> header) {

    if (doc.isNull())
        return nullptr;

    QJsonArray array = doc.array();
    int rowCount = (int)array.count();
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

    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
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

QSortFilterProxyModel * ServeResponse::get_proxyModel(const QString& resp) {

    QJsonDocument doc = parseResp(resp);

    if (doc.isNull())
        return nullptr;

    return get_proxyModel(doc, QMap<QString, int>());
}

QMap<QString, int> ServeResponse::get_header(QJsonObject *obj, QString firstCol) {

    QList<QString> list;
    QMap<QString, int> header;
    int index = 0;

    for (auto col = obj->constBegin(); col != obj->constEnd(); col++)
    {
        if (firstCol.isEmpty()){
            header.insert(col.key(), index);
            index++;
        }else{
            if (col.key() != firstCol)
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
