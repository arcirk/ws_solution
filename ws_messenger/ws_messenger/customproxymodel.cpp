#include "customproxymodel.h"
#include "groupsmodel.h"

CustomProxyModel::CustomProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void CustomProxyModel::setFilter(QString uuid_parent)
{
    this->setFilterFixedString(uuid_parent);
    this->setFilterRole(GroupsModel::Roles::ParentRole);
}
