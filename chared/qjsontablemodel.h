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
#include <QIcon>

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
    void setColumnAliases(const QMap<QString, QString>& columnAliases);

    Q_INVOKABLE void reset();

    void setRowsIcon(const QIcon& ico);
    void setIcon(const QModelIndex& index, const QIcon& ico);
    void setRowKey(const QPair<QString, QString>& key);
    QPair<QString, QString> rowKey(int index) const;
    void setFormatColumn(int column, const QString& fmt);

    void removeRow(int row);
    void addRow(const QJsonObject& row);
    int row(const QPair<QString, QString>& key);

signals:
    void jsonTextChanged();
private:
    bool setJson( const QJsonArray& array );
    static QString fromBase64(const QString& str);
    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    Header m_header{};
    QJsonArray m_json;
    QJsonArray _header;
    QMap<QString, QString> m_colAliases;
    QIcon _rowsIcon;
    QMap<QPair<int,int>, QIcon> m_rowIcon;
    QList<QPair<QString, QString>> m_rowKeys;
   // QList<QModelIndex> m_fmtText;
    QMap<int, QString> m_fmtText;

};
#endif //WS_SOLUTION_QJSONTABLEMODEL_H
