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
#include "sqlqueryinterface.h"

class KeysContainer;

typedef std::function<void(const ByteArray&)> set_keys;
typedef std::function<void(ByteArray&)> get_keys;



#define FAT12_          "FAT12"
#define REGISTRY_       "REGISTRY"
#define HDIMAGE_        "HDIMAGE"
#define DATABASE_       "DATABASE"
#define REMOTEBASE_     "REMOTEBASE"

class KeysContainer : public QObject
{
    Q_OBJECT
public:
    explicit KeysContainer(QObject *parent = nullptr);
    //explicit KeysContainer(const QString& sid, const QString& localName, SqlInterface * db, QObject *parent = nullptr);

//    enum JsonFormat{
//        nameData = 0,
//        forDatabase,
//        serialization
//    };

    enum TypeOfStorgare{
        storgareTypeRegistry = 0,
        storgareTypeLocalVolume,
        storgareTypeDatabase,
        storgareTypeRemoteBase,
        storgareTypeUnknown
    };

//    const QStringList VolumeTypeString{
//        "FAT12",
//        "REGISTRY",
//        "HDIMAGE",
//        "DATABASE"
//    };

    TypeOfStorgare typeOfStorgare();
    static TypeOfStorgare typeOfStorgare(const QString& source);

    void fromFolder(const QString& folder);
    void fromContainerName(const QString& cntName);
    void fromRegistry(const QString& sid, const QString& name);
    void fromJson(const QByteArray& data);
    void readData(const QString& sid = "", const QString& name = "");

    QByteArray toBase64();
    bool sync(const QString& sid = "");
    bool sync(TypeOfStorgare vType, const QString& newStorgare, const QString& sid = "");
    bool syncRegystry(const QString& sid);
    bool syncVolume(const QString& dir = "");
    bool isValid();
    QString bindName() const;

    QString originalName() const;
    void setOriginalName(ByteArray name_key_data);
    void setNewOriginalName(const QString& new_name);
    static QString readOriginalName(const QString& pathToStorgare);

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

    void erase();

    QString subject() const;
    QString issuer() const;
    //QString container() const;
    QString notValidBefore() const;
    QString notValidAfter() const;
    QString parentUser() const;
    QString nameInStorgare() const;

    void setName(const QString& value);
    void setVolume(const QString& value);
    void setVolumePath(const QString& value);
    void setKeyName(const QString& value);
    void setRef(const QString& value);
    void setStorgare(const QString& value);
    void setCache(const QJsonObject& obj);

    QString name() const;
    QString volume() const;
    QString volumePath() const;
    QString keyName() const;
    QString ref() const;
    QString storgare() const;
    QJsonObject cache() const;

    QString containerInfo();    
    QJsonObject parseCsptestInfo(const QString& info);
    void infoFromDataBaseJson(const QString& json);

    bool removeContainerFromRegistry(const QString& sid, const QString& containerName);
    QBSqlQuery getSqlQueryObject(QBSqlCommand command);

private:
    QString _path;

    ByteArray _header_key;
    ByteArray _masks_key;
    ByteArray _masks2_key;
    ByteArray _name_key;
    ByteArray _primary_key;
    ByteArray _primary2_key;

    QString _subject;
    QString _issuer;
    QString _container;
    QString _notValidBefore;
    QString _notValidAfter;
    QString _parentUser;
    QJsonObject _cache;
    QString _ref;

    //ContainerName _cntName;

    QString _name; // @OOO
    QString _key_name; //123456@
    QString _volume; //FAT12_D
    QString _storgare;// //./FAT12_D/
    QString _volumePath; //D:/
    QString _nameInStorgare; //полное имя контейнера без форматирования
    QString _originalName;
    //QString _device;

    //QMap<VolumeType, QString> m_volume;


    bool _isValid;
    SqlInterface * _db;



    //const QString sample_volume = "\\\\.\\%1\\";
    //const QString sample_full_key = "\\\\.\\%1\\%2";

    std::map<std::string, set_keys> set_function();
    set_keys get_set_function(int index);
    get_keys get_get_function(int index);
    static QString stringFromBase64(const QString &value);

//    static void readFile(const std::string& filename, ByteArray& result);
//    static void writeFile(const std::string& filename, ByteArray& fileBytes);

signals:

};

#endif // KEYSCONTAINER_H
