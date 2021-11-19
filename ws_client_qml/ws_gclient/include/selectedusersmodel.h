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
    //Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString jsonText READ jsonText WRITE setJsonText NOTIFY jsonTextChanged)
    Q_PROPERTY(QString userUuid READ userUuid WRITE setUserUuid NOTIFY jsonUserUuidChanged)
    Q_PROPERTY(QString currentRow READ currentRow WRITE setCurrentRow NOTIFY currentRowChanged)

public:
    typedef QMap<QString,QString> Heading;
    typedef QVector<Heading> Header;

    explicit SelectedUsersModel(QObject * parent = nullptr);

    bool setJson( const QJsonDocument& json );

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;


    Q_INVOKABLE void removeRow(const QString& uuid);
    Q_INVOKABLE void addRow(const QString& uuid, const QString &name, bool unActive = false);
    Q_INVOKABLE QModelIndex item(const QString& uuid, int col = 0);
    Q_INVOKABLE void setRowValue(QModelIndex &index, const QVariant& value);
    Q_INVOKABLE int getColumnIndex(const QString& name);
    Q_INVOKABLE void saveDraft(const QString& uuid, const QString& source);
    Q_INVOKABLE QString getDraft();

    Q_INVOKABLE void setCountUnReadMessage(const QString& uuid, bool noReset = false);
    Q_INVOKABLE void setUnReadMessages(const QString& resp);

    Q_INVOKABLE void resetStatusActiveUsers(const QString& resp);
    Q_INVOKABLE bool isAlreadyAdded(const QString& uuid);
    Q_INVOKABLE void setStatusUser(const QString& resp, bool value);
    Q_INVOKABLE void setStatusUser(const QString& resp);
    Q_INVOKABLE void updateUserData(const QString& resp);

    QString jsonText() const;
    void setJsonText(const QString& source);

    QString userUuid();
    void setUserUuid(const QString& uuid);

    QString currentRow();
    void setCurrentRow(const QString& uuid);



private:
    Header m_header{};
    QJsonArray m_json;
    QJsonArray _header;
    QString _user_uuid;
    QString m_currentRow;


    bool setJson( const QJsonArray& array );
    virtual QJsonObject getJsonObject( const QModelIndex &index ) const;
    void getHeaderJsonObject();

    void resetCountUnReadMsg();

    bool is_already_added(const QString& uuid);

signals:
    //void currentIndexChanged(int index);
    void jsonTextChanged();
    void jsonUserUuidChanged();
    void currentRowChanged();
    void draftChanged();
    void getStatusAddedUser(const QString& userUuid);
    void requestSaveCache();

};

#endif // SELECTEDUSERSMODEL_H
