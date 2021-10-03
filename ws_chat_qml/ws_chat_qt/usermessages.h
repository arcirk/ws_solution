#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#include <QAbstractListModel>

class UserMessagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UserMessagesModel(QObject *parent = nullptr);

    enum {
        DoneRole = Qt::UserRole,
        DateRole,
        Sender,
        Recipient,
        Message,
        Token
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::DisplayRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
};

#endif // USERMESSAGES_H
