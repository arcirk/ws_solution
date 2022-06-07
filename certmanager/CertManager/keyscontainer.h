#ifndef KEYSCONTAINER_H
#define KEYSCONTAINER_H

#include <QObject>
#include <QSettings>
#include <sqlinterface.h>
#include <QJsonObject>
#include <QJsonValue>
#include "lib/WinReg.hpp"
#include "converter.h"

class KeysContainer;

typedef std::vector<BYTE> BYTES;
typedef std::function<void(const BYTES&)> set_keys;
typedef std::function<void(BYTES&)> get_keys;

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
    void fromIni(const QByteArray& data);

    bool isValid();

    void setName(const QString& value);
    QString name();

    void header_key(BYTES& value);
    void masks_key(BYTES& value);
    void masks2_key(BYTES& value);
    void name_key(BYTES& value);
    void primary_key(BYTES& value);
    void primary2_key(BYTES& value);

    void set_header_key(const BYTES& value);
    void set_masks_key(const BYTES& value);
    void set_masks2_key(const BYTES& value);
    void set_name_key(const BYTES& value);
    void set_primary_key(const BYTES& value);
    void set_primary2_key(const BYTES& value);

    void fromDatabase();
    bool toDataBase();
    QSettings toQSettings();
    void fromQSettings(const QSettings& value);

    BYTES toByteArhive();
    QJsonObject toJsonObject(JsonFormat format, const QUuid& uuid = QUuid());

    QString path();
    void setPath(const QString& sid, const QString& containerName);

    QString containerInfo();
    void parseCsptestInfo(const QString& info);

    bool syncRegystry();
    bool removeContainer(const QString& sid, const QString& containerName);

    QJsonObject parseDeviceString(const QString& key);

private:
    QString _path;
    BYTES _header_key;
    BYTES _masks_key;
    BYTES _masks2_key;
    BYTES _name_key;
    BYTES _primary_key;
    BYTES _primary2_key;

    SqlInterface * _db;
    QString _name;

    bool _isValid;


    std::map<std::string, set_keys> set_function();
    set_keys get_set_function(int index);
    get_keys get_get_function(int index);

    static void readFile(const std::string& filename, BYTES& result);
    static void writeFile(const std::string& filename, BYTES& fileBytes);

signals:

};

#endif // KEYSCONTAINER_H
