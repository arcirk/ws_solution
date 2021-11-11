//
// Created by arcady on 10.11.2021.
//

#include "../include/qproxymodel.h"
#include "../include/qjsontablemodel.h"
#include <QJsonDocument>
#include <QJsonObject>

QProxyModel::QProxyModel(QObject *parent)
: QSortFilterProxyModel(parent)
{

}

bool QProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {

    auto model = (QJsonTableModel*)sourceModel();

    if (m_filter.size() > 0){
        QMapIterator<QString, QVariant> i(m_filter);
        while (i.hasNext()){
            i.next();
            int col = model->getColumnIndex(i.key());
            QModelIndex index = model->index(sourceRow, col, sourceParent);

            if (i.value().typeId() == QMetaType::QString){
                if (i.value().toString() != model->data(index).toString())
                    return false;
            }
            else if (i.value().typeId() == QMetaType::Int){
                if (i.value().toInt() != model->data(index).toInt())
                    return false;
            }
            else if (i.value().typeId() == QMetaType::Bool){
                if (i.value().toBool() != model->data(index).toBool())
                    return false;
            }
            else if (i.value().typeId() == QMetaType::Double){
                if (i.value().toDouble() != model->data(index).toDouble())
                    return false;
            }
        }

    }

    return true;
}

QString QProxyModel::filter() {
    return m_FilterText;
}

void QProxyModel::setFilter(const QString &filterText) {

    m_FilterText = filterText;
}

void QProxyModel::set_filter() {

    m_filter.clear();
    auto doc = QJsonDocument::fromJson(m_FilterText.toUtf8());
    if (doc.isNull())
        return;

    QJsonObject obj = doc.object();

    if (obj.isEmpty())
        return;

    for (auto itr = obj.begin(); itr < obj.end(); ++itr) {
        QString key = itr.key();
        QVariant value = itr->toVariant();

        m_filter.insert(key, value);

    }
}