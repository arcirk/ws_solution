#ifndef KEYSCONTAINER_H
#define KEYSCONTAINER_H

#include <QObject>
#include <QSettings>
#include <sqlinterface.h>
#include <QJsonObject>
#include <QJsonValue>

class KeysContainer;

typedef std::function<void(const QByteArray&)> set_keys;
typedef std::function<QByteArray()> get_keys;

class KeysContainer : public QObject
{
    Q_OBJECT
public:
    explicit KeysContainer(QObject *parent = nullptr);
    explicit KeysContainer(const QString& sid, const QString& localName, SqlInterface * db, QObject *parent = nullptr);

    enum JsonFormat{
        nameData = 0,
        forDatabase,
        serialization
    };

    void fromFolder(const QString& folder);
    void fromRegistry(const QString& name, const QString& sid);

    bool isValid();

    void setName(const QString& value);
    QString name();

    QByteArray header_key();
    QByteArray masks_key();
    QByteArray masks2_key();
    QByteArray name_key();
    QByteArray primary_key();
    QByteArray primary2_key();

    void set_header_key(const QByteArray& value);
    void set_masks_key(const QByteArray& value);
    void set_masks2_key(const QByteArray& value);
    void set_name_key(const QByteArray& value);
    void set_primary_key(const QByteArray& value);
    void set_primary2_key(const QByteArray& value);

    void fromDatabase();
    bool toDataBase();
    QSettings toQSettings();
    void fromQSettings(const QSettings& value);

    QByteArray toByteArray();
    QJsonObject toJsonObject(JsonFormat format, const QUuid& uuid = QUuid());

    QString path();

    QString containerInfo();
    void parseCsptestInfo(const QString& info);

private:
    QString _path;
    QByteArray _header_key;
    QByteArray _masks_key;
    QByteArray _masks2_key;
    QByteArray _name_key;
    QByteArray _primary_key;
    QByteArray _primary2_key;

    SqlInterface * _db;
    QString _name;

    bool _isValid;


    std::map<std::string, set_keys> set_function();
    set_keys get_set_function(int index);
    get_keys get_get_function(int index);

signals:

};

#endif // KEYSCONTAINER_H
