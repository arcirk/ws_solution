//
// Created by arcady on 03.11.2021.
//

#ifndef WS_SOLUTION_GROUPTREEMODEL_H
#define WS_SOLUTION_GROUPTREEMODEL_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>

class GroupTreeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    explicit GroupTreeModel(const Header& header, QObject * parent = 0);

    bool setJson( QJsonDocument& json );
    bool setJson( const QJsonArray& array );

    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    Header m_header;
    QJsonArray m_json;
};


#endif //WS_SOLUTION_GROUPTREEMODEL_H
