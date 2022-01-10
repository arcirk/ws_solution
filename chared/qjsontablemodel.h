//
// Created by arcady on 10.11.2021.
//

#ifndef WS_SOLUTION_QJSONTABLEMODEL_H
#define WS_SOLUTION_QJSONTABLEMODEL_H
#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSortFilterProxyModel>

class QJsonTableModel : public QAbstractTableModel{

    Q_OBJECT
    Q_PROPERTY(QString jsonText READ jsonText WRITE setJsonText NOTIFY jsonTextChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;

    explicit QJsonTableModel(QObject * parent = nullptr);

    bool setJson( const QJsonDocument& json );

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int getColumnIndex(const QString& name);

    QString jsonText() const;
    void setJsonText(const QString& source);

    Q_INVOKABLE void reset();

signals:
    void jsonTextChanged();
private:
    bool setJson( const QJsonArray& array );
    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    Header m_header{};
    QJsonArray m_json;
    QJsonArray _header;
};
#endif //WS_SOLUTION_QJSONTABLEMODEL_H
