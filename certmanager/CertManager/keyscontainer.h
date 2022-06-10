#ifndef KEYSCONTAINER_H
#define KEYSCONTAINER_H

#include <QObject>
#include <QSettings>
#include <sqlinterface.h>
#include <QJsonObject>
#include <QJsonValue>
#include "lib/WinReg.hpp"
#include "converter.h"
#include "commandline.h"

class KeysContainer;

typedef std::function<void(const ByteArray&)> set_keys;
typedef std::function<void(ByteArray&)> get_keys;

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

    enum VolumeType{
        FAT12 = 0,
        REGISTRY,
        HDIMAGE
    };
    const QStringList VolumeTypeString{
        "FAT12",
        "REGISTRY",
        "HDIMAGE"
    };

    void fromFolder(const QString& folder);
    void fromRegistry();
    void fromJson(const QByteArray& data);

    QByteArray toBase64();

    bool isValid();
    void setWindowsSid(const QString& value);
    void setName(const QString& value);
    QString name();
    QString nameBase64();
    void setVolume(VolumeType value);
    VolumeType volume();
    void setVolumePath(const QString& value);
    QString volumePath();
    void setKeyName(const QString& value);
    QString keyName();
    QString fullKeyName();
    bool sync();
    void bindRegistryPath(const QString& sid);

    void header_key(ByteArray& value);
    void masks_key(ByteArray& value);
    void masks2_key(ByteArray& value);
    void name_key(ByteArray& value);
    void primary_key(ByteArray& value);
    void primary2_key(ByteArray& value);

    void set_header_key(const ByteArray& value);
    void set_masks_key(const ByteArray& value);
    void set_masks2_key(const ByteArray& value);
    void set_name_key(const ByteArray& value);
    void set_primary_key(const ByteArray& value);
    void set_primary2_key(const ByteArray& value);


    bool toDataBase();
    QSettings toQSettings();
    void fromQSettings(const QSettings& value);

    ByteArray toByteArhive();

    QJsonObject toJsonObject(JsonFormat format, const QUuid& uuid = QUuid());

    QString path();
    void setPath(const QString& sid, const QString& containerName);

    QString containerInfo();
    void parseCsptestInfo(const QString& info);

    bool syncRegystry();
    bool syncVolume();

    bool removeContainer(const QString& sid, const QString& containerName);

    void parseAdressKey(const QString& key);

    static QString stringFromBase64(const QString &value);

private:
    QString _path;

    ByteArray _header_key;
    ByteArray _masks_key;
    ByteArray _masks2_key;
    ByteArray _name_key;
    ByteArray _primary_key;
    ByteArray _primary2_key;


    SqlInterface * _db;
    QString _name;
    VolumeType _valume;
    QString _volumePath;
    QString _device;
    QString _key_name; //123456@
    QMap<VolumeType, QString> m_volume;
    QString _sid;

    bool _isValid;

    const QString sample_volume = "\\\\.\\%1\\";
    const QString sample_full_key = "\\\\.\\%1\\%2";

    std::map<std::string, set_keys> set_function();
    set_keys get_set_function(int index);
    get_keys get_get_function(int index);

//    static void readFile(const std::string& filename, ByteArray& result);
//    static void writeFile(const std::string& filename, ByteArray& fileBytes);

signals:

};

#endif // KEYSCONTAINER_H
