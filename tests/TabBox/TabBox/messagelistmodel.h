#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QAbstractTableModel>
#include <QJsonDocument>
#include <QJsonArray>

class MessageListModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString userUuid READ userUuid WRITE setUserUuid NOTIFY UserUuidChanged)
    Q_PROPERTY(QString companionUuid READ companionUuid WRITE setCompanionUuid NOTIFY companionUuidChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    explicit MessageListModel(const Header& header, QObject * parent = 0);

    //bool setJson( QJsonDocument* json );
    //bool setJson( const QJsonArray& array );

    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void sendMessage(const QString &message);

    void setUserUuid(QString uuid);
    void setCompanionUuid(QString uuid);
    QString userUuid();
    QString companionUuid();

    Q_INVOKABLE void clearRows();

    void addDocument(QJsonDocument doc, int itemIndex);
    //QJsonDocument* getDocument(int index);
    Q_INVOKABLE void setDocument(int index);

private:

    int currentIndex;

    Header m_header;

    //QMap<int, QJsonDocument*> mDocs;
    QMap<int, QJsonArray> m_arrMsg;
    //QJsonArray m_json;

    QString m_userUuid;
    QString m_companionUuid;
    QString m_Token;

signals:
    void UserUuidChanged(QString uuid);
    void companionUuidChanged(QString uuid);

public slots:
    void onNewMessage(QString message);

};

#endif // MESSAGELISTMODEL_H
