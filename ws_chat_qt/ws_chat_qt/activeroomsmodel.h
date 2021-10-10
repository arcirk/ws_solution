#ifndef ACTIVEROOMSMODEL_H
#define ACTIVEROOMSMODEL_H

#include <QAbstractListModel>
#include "qmlmessage.h"

class ActiveRoomsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ActiveRoomsModel(QObject *parent = nullptr);

    enum {
        DoneRole = Qt::UserRole,
        TokenRole,
        SenderRole,
        RecipientRole,
        MessagesRole
    };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
    //QList<QmlMessage*> m_messages;
    int _rowCount;
};

#endif // ACTIVEROOMSMODEL_H
