#include "serveresponse.h"

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

QString ServeResponse::getStringMember(QJsonDocument &doc, const QString &key){

    if(doc.isObject())
    {
        QJsonObject obj = doc.object();
        auto _message = obj.find(key);

        if(_message->isString()){
            return _message.value().toString();
        }
    }

    return "";
}

QString ServeResponse::getStringMember(QJsonObject &obj, const QString &key){

    auto _message = obj.find(key);

    if(_message->isString()){
        return _message.value().toString();
    }

    return "";
}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QString &json) {

//    QJsonDocument result = parseResp(json);
//
//    if (result.isNull())
//        return;
//
//    if (result.isObject()){
//        table->clear();
//        table->setColumnCount(result.object().count());
//    }

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

void ServeResponse::loadTableFromJson(QTableWidget *table, QJsonArray array) {

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

QTreeWidgetItem *ServeResponse::new_tree_item(const QJsonObject& jsonObject) {
    return nullptr;
}
