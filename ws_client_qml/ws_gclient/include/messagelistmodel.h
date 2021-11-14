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
    //Q_PROPERTY(QString companionUuid READ companionUuid WRITE setCompanionUuid NOTIFY companionUuidChanged)
    //Q_PROPERTY(QString jsonText READ jsonText WRITE setJsonText NOTIFY jsonTextChanged)
    Q_PROPERTY(QString currentRecipient READ currentRecipient WRITE setCurrentRecipient NOTIFY currentRecipientChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;
    explicit MessageListModel(QObject * parent = 0);

    //bool setJson( const QJsonDocument& json );




    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void sendMessage(const QString &message);
    //Q_INVOKABLE bool isMessagesLoaded()

    void setUserUuid(QString uuid);
    //void setCompanionUuid(QString uuid);
    QString userUuid();
    QString companionUuid();

    Q_INVOKABLE void clearRows();


    Q_INVOKABLE void addDocument(const QString& json);
    Q_INVOKABLE void setDocument(const QString& uuid);
    Q_INVOKABLE void remove(const QString& uuid);
    Q_INVOKABLE void addMessage(const QString& msg, const QString& uuid, const QString& recipient);

//    QString jsonText() const;
//    void setJsonText(const QString& source);

    QString currentRecipient();

    void setCurrentRecipient(const QString& uuid);

private:

    bool setJson( const QJsonArray& array );

    QString m_currentRecipient;

    Header m_header{};
    QJsonArray m_json;
    QJsonArray _header;


    QMap<QString, QJsonArray> m_arrMsg;

    //QMap<int, QJsonDocument*> mDocs;

    //QJsonArray m_json;

    QString m_userUuid;
    QString m_companionUuid;
    QString m_Token;

    void addDocument(QJsonDocument doc, const QString& uuid);
    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;

    QString getRoleName(int role) const;

signals:
    void UserUuidChanged(QString uuid);
   //void companionUuidChanged(QString uuid);
    void currentRecipientChanged();
    void jsonTextChanged();

    void getMessagesForRecipient(const QString& uuid);


public slots:
    void onNewMessage(QString message);

};

#endif // MESSAGELISTMODEL_H
