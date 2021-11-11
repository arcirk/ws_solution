#ifndef SELECTEDUSERSMODEL_H
#define SELECTEDUSERSMODEL_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include "messagelistmodel.h"

//class MessageListModel;

class SelectedUsersModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    explicit SelectedUsersModel(const Header& header, QObject * parent = 0);

    bool setJson( const QJsonDocument& json );
    bool setJson( const QJsonArray& array );

    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int addRow(const QString& uuid, const QString &name);

    void setCurrentIndex(int index);

    int currentIndex();

    Q_INVOKABLE void remove(int index);



private:
    Header m_header;
    QJsonArray m_json;
    int m_currentIndex;
    int iCount;

signals:
    void currentIndexChanged(int index);
};

#endif // SELECTEDUSERSMODEL_H
