#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSortFilterProxyModel>

class UsersModel : QAbstractTableModel{

    Q_OBJECT
    Q_PROPERTY(QSortFilterProxyModel* subdivisions READ subdivisions WRITE setSubdivisions NOTIFY subdivisionsChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    explicit UsersModel(const Header& header, QObject * parent = 0);

    bool setJson( const QJsonDocument& json );
    bool setJson( const QJsonArray& array );

    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;


    QSortFilterProxyModel* subdivisions();
    void setSubdivisions(QSortFilterProxyModel* proxy);

private:
    Header m_header;
    QJsonArray m_json;
    QSortFilterProxyModel* _subdivisions;

signals:
    void subdivisionsChanged();
};


#endif // USERSMODEL_H

