#include "keyscontainer.h"
#include <QSettings>
#include <QFile>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QUuid>
#include <QSqlError>


//#include <cstdio>
//#include <cstdlib>

#ifdef _WINDOWS
    #pragma warning(disable:4100)
    //#include <Windows.h>
    #pragma comment(lib, "advapi32")
#endif

#include <fstream>
#include <iostream>
#include <string>

using std::wcout;
using std::wstring;
using std::vector;

QStringList KeyFiles = {
    "header.key",
    "masks.key",
    "masks2.key",
    "name.key",
    "primary.key",
    "primary2.key"
};

KeysContainer::KeysContainer(QObject *parent)
    : QObject{parent}
{
    _isValid = false;
}

KeysContainer::KeysContainer(const QString& sid, const QString& localName, SqlInterface * db, QObject *parent)
    : QObject{parent}
{
//    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, localName);
//    QSettings reg = QSettings(root, QSettings::NativeFormat);
//    _header_key = reg.value("header.key").toBYTESArray();
//    _masks_key = reg.value("masks.key").toBYTESArray();
//    _masks2_key = reg.value("masks2.key").toBYTESArray();
//    _name_key = reg.value("name.key").toBYTESArray();
//    _primary_key = reg.value("primary.key").toBYTESArray();
//    _primary2_key = reg.value("primary2.key").toBYTESArray();
    _name = localName;

    _db = db;

    _isValid = true;
}

void KeysContainer::setName(const QString &value)
{
    _name = value;
}

QString KeysContainer::name()
{
    return _name;
}

void KeysContainer::header_key(BYTES &value)
{
    value = _header_key;
}

void KeysContainer::masks_key(BYTES &value)
{
    value = _masks_key;
}

void KeysContainer::masks2_key(BYTES &value)
{
    value = _masks2_key;
}

void KeysContainer::name_key(BYTES &value)
{
    value = _name_key;
}

void KeysContainer::primary_key(BYTES &value)
{
    value = _primary_key;
}

void KeysContainer::primary2_key(BYTES &value)
{
    value = _primary2_key;
}

void KeysContainer::set_header_key(const BYTES &value)
{
    _header_key = value;
}

void KeysContainer::set_masks_key(const BYTES &value)
{
    _masks_key = value;
}

void KeysContainer::set_masks2_key(const BYTES &value)
{
    _masks2_key = value;
}

void KeysContainer::set_name_key(const BYTES &value)
{
    _name_key = value;
}

void KeysContainer::set_primary_key(const BYTES &value)
{
    _primary_key = value;
}

void KeysContainer::set_primary2_key(const BYTES &value)
{
    _primary2_key = value;
}

void KeysContainer::fromDatabase()
{

}

bool KeysContainer::toDataBase()
{
    if(!_db){
        qCritical() << __FUNCTION__ <<  "База данных не открыта!";
        return false;

    }

    if(!_db->getDatabase().isOpen()){
        qCritical() << __FUNCTION__ <<  "База данных не открыта!";
        return false;

    }

//    QTemporaryDir temp;
//    if(!temp.isValid())
//        return false;


//    QString uuid = QUuid::createUuid().toString();
//    uuid = uuid.mid(1, uuid.length() - 2);
//    QByteArray data = toByteArhive();
//    if(data == "")
//        return false;

//    QSqlQuery query(_db->getDatabase());
//    query.prepare("INSERT INTO [dbo].[Containers] ([Ref], [FirstField], [data]) "
//                  "VALUES (?, ?, ?)");
//    query.addBindValue(uuid);
//    query.addBindValue(name().trimmed());
//    query.addBindValue(data);
//    query.exec();

//    if (query.lastError().type() != QSqlError::NoError)
//    {
//        qDebug() << __FUNCTION__ << query.lastError();
//        return false;
//    }

    return true;

}

QSettings KeysContainer::toQSettings()
{
    return QSettings{};
}

void KeysContainer::fromQSettings(const QSettings &value)
{

}

BYTES KeysContainer::toByteArhive()
{
    if(!_isValid){
        qCritical() << __FUNCTION__ <<  "Данные не инициализированы!";
        return {};
    }

    if(_name.isEmpty()){
        qCritical() << __FUNCTION__ <<  "Не указано наименование контейнера!";
        return {};
    }

    QTemporaryDir temp;
    if(!temp.isValid())
        return {};

    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);

    QString tempFile = QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".zip");

//    QSettings settings = QSettings(tempFile, QSettings::IniFormat, this);

//    settings.beginGroup(name());
//    for (int i = 0; i < KeyFiles.size(); ++i) {
//        get_keys fun = get_get_function(i);
//        BYTES data;
//        get_get_function(i)(data);
//        BYTES * buffer = new
//        settings.setValue(KeyFiles[i], get_get_function(i)());
//    }
//    settings.endGroup();
//    settings.sync();

//    if(settings.status() != QSettings::NoError){
//        qCritical() << __FUNCTION__ << settings.status();
//        return "";
//    }

//    QFile fdata(tempFile);
//    if(fdata.open(QIODevice::ReadOnly)){
//        QByteArray data = fdata.readAll();
//        fdata.close();
//        fdata.remove();
//        return data;
//    }

    return {};
}

QJsonObject KeysContainer::toJsonObject(JsonFormat format, const QUuid& uuid)
{

    QJsonObject obj = QJsonObject();
//    if(format == nameData){
//        Q_BYTEArray data = to_BYTEArray();
//        if(data == "")
//            return obj;

//        obj.insert("name", name());
//        obj.insert("data",  QJsonValue::fromVariant(data));

//    }else if(format == forDatabase){
//        QUuid _uuid = uuid;
//        if(_uuid.isNull()){
//            _uuid = QUuid::createUuid();
//        }

//        QString sz_uuid = _uuid.toString();
//        sz_uuid = sz_uuid.mid(1, sz_uuid.length() - 2);

//        Q_BYTEArray data = to_BYTEArray();
//        if(data == "")
//            return obj;

//        obj.insert("Ref", sz_uuid);
//        obj.insert("name", name());
//        obj.insert("data",  QJsonValue::fromVariant(data));

//    }else if(format == serialization){
//        for(int i = 0; i < KeyFiles.size(); ++i){
//            QString key = KeyFiles[i];
//            Q_BYTEArray data = get_get_function(i)();
//            QJsonValue val = QJsonValue::fromVariant(data);
//            obj.insert(key, val);
//        }
//    }

    return obj;

}

QString KeysContainer::path()
{
    return _path;
}

void KeysContainer::setPath(const QString &sid, const QString& containerName)
{
    _path = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, containerName);
}

void KeysContainer::parseCsptestInfo(const QString &info)
{
    int ind = info.indexOf("KP_CERTIFICATE:");
    if(ind > 0){
        int pKey = info.indexOf("PrivKey", ind);
        QString _info = info.mid(ind, pKey);
    }
}

bool KeysContainer::syncRegystry()
{
    if(!isValid()){
        qCritical() << __FUNCTION__ << "класс не инициализирован!";
        return false;
    }
//    auto reg = QSettings(_path, QSettings::NativeFormat);
//    for(int i = 0; i < KeyFiles.size(); ++i){
//        QString key = KeyFiles[i];
//        auto funcGet = get_get_function(i);
//        Q_BYTEArray data = funcGet();
//        if(data.isEmpty())
//            return false;
//        reg.setValue(key, data);
//    }

//    reg.sync();


    QString ph = _path;
    ph.replace("\\HKEY_LOCAL_MACHINE\\", "");
    winreg::RegKey regKey = winreg::CreateKey(HKEY_LOCAL_MACHINE, ph.toStdWString().c_str());


    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        auto funcGet = get_get_function(i);
        BYTES data;
        funcGet(data);
        if(data.size() == 0)
            return false;
        winreg::RegValue v(REG_BINARY);
        foreach(auto bt, data){
            v.Binary().push_back(bt);
        }

//        v.Binary().push_back(0x33);
//        v.Binary().push_back(0x44);
        SetValue(regKey.Get(), key.toStdWString().c_str(), v);
    }
    return true;
}

bool KeysContainer::removeContainer(const QString &sid, const QString &containerName)
{
    QString s_path = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys").arg(sid);
    QSettings reg = QSettings(s_path, QSettings::NativeFormat);
    reg.beginGroup(containerName);
    reg.remove("");
    reg.endGroup();
    return true;
}

std::map<std::string, set_keys> KeysContainer::set_function()
{
    std::map<std::string, set_keys> f;

    for (int i = 0; i < KeyFiles.size(); ++i) {
        f.emplace(KeyFiles[i].toStdString(), get_set_function(i));
    }

    return f;
}

set_keys KeysContainer::get_set_function(int index)
{
    set_keys f;
    switch (index) {
        case 0:{
            f = std::bind(&KeysContainer::set_header_key, this, std::placeholders::_1);
            break;
        }
        case 1:{
            f = std::bind(&KeysContainer::set_masks_key, this, std::placeholders::_1);
            break;
        }
        case 2:{
            f = std::bind(&KeysContainer::set_masks2_key, this, std::placeholders::_1);
            break;
        }
        case 3:{
            f = std::bind(&KeysContainer::set_name_key, this, std::placeholders::_1);
            break;
        }
        case 4:{
            f = std::bind(&KeysContainer::set_primary_key, this, std::placeholders::_1);
            break;
        }
        case 5:{
            f = std::bind(&KeysContainer::set_primary2_key, this, std::placeholders::_1);
            break;
        }
    }

    return f;
}

get_keys KeysContainer::get_get_function(int index)
{
    get_keys f;
    switch (index) {
        case 0:{
            f = std::bind(&KeysContainer::header_key, this, std::placeholders::_1);
            break;
        }
        case 1:{
            f = std::bind(&KeysContainer::masks_key, this, std::placeholders::_1);
            break;
        }
        case 2:{
            f = std::bind(&KeysContainer::masks2_key, this, std::placeholders::_1);
            break;
        }
        case 3:{
            f = std::bind(&KeysContainer::name_key, this, std::placeholders::_1);
            break;
        }
        case 4:{
            f = std::bind(&KeysContainer::primary_key, this, std::placeholders::_1);
            break;
        }
        case 5:{
            f = std::bind(&KeysContainer::primary2_key, this, std::placeholders::_1);
            break;
        }
    }

    return f;
}

void KeysContainer::fromFolder(const QString &folder)
{

    auto func = set_function();
    _isValid = false;
    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        QString filename = QDir::toNativeSeparators(folder + QDir::separator() + key);
        if(!QFile::exists(filename))
            return;
        //QFile file(folder + QDir::separator() + key);
        //if(file.open(QIODevice::ReadOnly)){
           BYTES buffer;
           readFile(filename.toStdString(), buffer);
           func[key.toStdString()](buffer);
        //}else
        //    return;
    }
    _isValid = true;
}

void KeysContainer::fromRegistry(const QString &name, const QString &sid)
{
    _isValid = false;
    auto func = set_function();

    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, name);
//    QSettings reg = QSettings(root, QSettings::NativeFormat);
//    for(int i = 0; i < KeyFiles.size(); ++i){
//        QString key = KeyFiles[i];
//        func[key.toStdString()](reg.value(key).to_BYTEArray());
//        auto funcGet = get_get_function(i);
//        Q_BYTEArray data = funcGet();
//        if(data.isEmpty())
//            return;
//    }

    _path = root;
    _name = name;
    _isValid = true;
}

void KeysContainer::fromIni(const QByteArray &data)
{
    QTemporaryDir temp;
    if(!temp.isValid())
        return;

    _isValid = false;
    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);
    QString tmpFile = QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".ini");
    QFile f(tmpFile);
    f.open(QIODevice::WriteOnly);
    f.write(data);
    f.close();

    QSettings reg = QSettings(tmpFile, QSettings::IniFormat);
    auto keys = reg.allKeys();
    if(keys.size() == 0)
        return;
    QStringList m_name = keys[0].split("/");
    if(m_name.size() != 2)
        return;
    setName(m_name[0]);

    auto func = set_function();

    foreach(auto key , reg.allKeys()){
        QString fileName = key.right(key.length() - _name.length()  - 1);
        if(KeyFiles.indexOf(fileName) != -1){
            QByteArray data = reg.value("key").toByteArray();
            std::vector<unsigned char> buffer(
                        data.begin(), data.end());
            func[fileName.toStdString()](buffer);
        }
        qDebug() << fileName;
//        auto details = parseDeviceString("\\\\.\\TEMP\\" + )
    }

    f.remove();
    _isValid = true;
}

QJsonObject KeysContainer::parseDeviceString(const QString& key){
    QJsonObject obj = QJsonObject();
//    QStringList m_data = key.split("\\");

//    QString device =  m_data[3].replace("\r", "");
//    QString volume =  device;
//    volume.replace("FAT12_", "");
//    QString nameBase64 =  m_data[4].replace("\r", "");
//    QString name = fromBase64(nameBase64);
//    int ind =  name.indexOf("@");
//    QString key_name = name.left(ind);

//    obj.insert("device", device);
//    obj.insert("nameBase64", nameBase64);
//    obj.insert("name", name);
//    obj.insert("key_name", key_name);
//    obj.insert("volume", volume);

    return obj;
}

bool KeysContainer::isValid()
{
    return _isValid;
}

void KeysContainer::writeFile(const std::string& filename, BYTES& file_bytes){
    std::ofstream file(filename, std::ios::out|std::ios::binary);
    std::copy(file_bytes.cbegin(), file_bytes.cend(),
        std::ostream_iterator<unsigned char>(file));
}

void KeysContainer::readFile(const std::string &filename, BYTES &result)
{

    FILE * fp = fopen(filename.c_str(), "rb");

    fseek(fp, 0, SEEK_END);
    size_t flen= ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::vector<unsigned char> v (flen);

    fread(&v[0], 1, flen, fp);

    fclose(fp);

    result = v;

    qDebug() << __FUNCTION__ << result.size();

    //std::string data( v.begin(), v.end() );
}
