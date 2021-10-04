#ifndef CUSTOMPROXYMODEL_H
#define CUSTOMPROXYMODEL_H

#include <QSortFilterProxyModel>

class CustomProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:

    explicit CustomProxyModel(QObject *parent = nullptr);

    void setFilter(QString uuid_parent);
};

#endif // CUSTOMPROXYMODEL_H
