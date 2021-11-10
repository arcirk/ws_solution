#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSortFilterProxyModel>
#include "usersproxymodel.h"

class UsersModel : public QAbstractTableModel{

    Q_OBJECT
    Q_PROPERTY(UsersProxyModel* subdivisions READ subdivisions NOTIFY subdivisionsChanged)
// WRITE setSubdivisions
public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    //explicit UsersModel(const Header& header, QObject * parent = nullptr);
    explicit UsersModel(QObject * parent = nullptr);

    bool setJson( const QJsonDocument& json );
    bool setJson( const QJsonArray& array );

    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;


    UsersProxyModel* subdivisions();
//    void setSubdivisions(UsersProxyModel* proxy);

    int getColumnIndex(const QString& name);

private:
    Header m_header;
    QJsonArray m_json;
    UsersProxyModel* _subdivisions;
    int filterIndex;
    QMap<QString,int> headerIndexes;

signals:
    void subdivisionsChanged();
};


#endif // USERSMODEL_H

