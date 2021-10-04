#ifndef GROUPSMODEL_H
#define GROUPSMODEL_H

#include <QAbstractListModel>

struct groupe_info{
    QString uuid;
    QString name;
    QString parent;

   explicit groupe_info() = default;
   explicit groupe_info(QString _uuid, QString _text, QString _parent)
        : uuid(_uuid),
          name(_text),
          parent(_parent)
    {

    }
    ~groupe_info(){};
};

class GroupsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
            UUIDRole = Qt::UserRole + 1,
            TextRole,
            ParentRole
        };

    explicit GroupsModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    void setRows();


private:
    QList<groupe_info*> m_data;

};

#endif // GROUPSMODEL_H
