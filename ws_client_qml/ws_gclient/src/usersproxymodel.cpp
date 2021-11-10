//
// Created by arcady on 10.11.2021.
//
#include "../include/usersproxymodel.h"
#include "../include/usersmodel.h"
#include <QJsonObject>

UsersProxyModel::UsersProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
{

}

bool UsersProxyModel::filterAcceptsRow(int source_row,
                                  const QModelIndex &source_parent) const{

    auto * model = (UsersModel*)sourceModel();

    QMapIterator<QString, QVariant> i(m_filter);
    while (i.hasNext()) {
        i.next();
        int index = model->getColumnIndex(QString::fromStdString(i.key().toStdString()));
        QModelIndex modelIndex = model->index(source_row,
                                                  index, source_parent);

        switch (i.value().type()) {

            case QVariant::String:{
                if(sourceModel()->data(modelIndex).toString() != i.value().toString())
                    return false;
                break;
            }
                case QVariant::Int:{
                if(sourceModel()->data(modelIndex).toInt() != i.value().toInt())
                    return false;
                break;
            }

        }
    }

    int parentInd = model->getColumnIndex("Parent");
    int isGroupInd = model->getColumnIndex("IsGroup");

    QModelIndex indRef = model->index(source_row,
                                              parentInd, source_parent);
    QModelIndex indIsGroup = model->index(source_row,
                                            isGroupInd, source_parent);


    if(sourceModel()->data(indRef).toString() != m_ref)
        return false;

    if(sourceModel()->data(indIsGroup).toInt() != is_group)
        return false;

    return true;
}

void UsersProxyModel::setRef(const QString &ref) {

    m_ref = ref;

}

void UsersProxyModel::setIsGroup(int isGroup) {

    is_group = isGroup;

}

QVariant UsersProxyModel::headerData(int section, Qt::Orientation orientation,
                                int role) const {
    return sourceModel()->headerData(section, orientation,
                                     role);
}

void UsersProxyModel::setFilter(const QString &fiterText)
{

    m_filter.clear();

    QJsonDocument json = QJsonDocument::fromJson(fiterText.toUtf8());

    if(json.isNull())
        return;

    QJsonObject obj = json.object();

    for (auto itr = obj.begin(); itr < obj.end() ; ++itr) {
        m_filter.insert(QString::fromStdString(itr.key().toStdString()), itr.value().toVariant());
    }

}
