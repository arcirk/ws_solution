#ifndef LISTMESSAGESMODEL_H
#define LISTMESSAGESMODEL_H

#include <QAbstractTableModel>
#include <QJsonDocument>

class ListMessagesModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString recipient READ recipient WRITE setRecipient NOTIFY recipientChanged)

public:
    explicit ListMessagesModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString recipient() const;
    void setRecipient(const QString &recipient);

    enum StructMeassage{
        FirstField = 0,
        Ref,
        SecondField,
        _id,
        date,
        message,
        token
    };

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void sendMessage(const QString &recipient, const QString &msg);

signals:
    void recipientChanged();

private:
    QMap<StructMeassage, QString> msgFields{
        QPair<StructMeassage, QString>(FirstField, "FirstField"),
        QPair<StructMeassage, QString>(Ref, "Ref"),
        QPair<StructMeassage, QString>(SecondField, "SecondField"),
        QPair<StructMeassage, QString>(_id, "_id"),
        QPair<StructMeassage, QString>(date, "date"),
        QPair<StructMeassage, QString>(message, "message"),
        QPair<StructMeassage, QString>(token, "token")
    };
    QString m_recipient;

    void loadListMessages();

    QJsonDocument jsonDocument;

};


#endif // LISTMESSAGESMODEL_H
