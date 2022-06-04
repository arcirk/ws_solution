#include "keyscontainer.h"
#include <QSettings>
#include <QFile>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QUuid>
#include <QSqlError>

#ifdef _WINDOWS
    #pragma warning(disable:4100)
#endif

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
    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, localName);
    QSettings reg = QSettings(root, QSettings::NativeFormat);
    _header_key = reg.value("header.key").toByteArray();
    _masks_key = reg.value("masks.key").toByteArray();
    _masks2_key = reg.value("masks2.key").toByteArray();
    _name_key = reg.value("name.key").toByteArray();
    _primary_key = reg.value("primary.key").toByteArray();
    _primary2_key = reg.value("primary2.key").toByteArray();
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

QByteArray KeysContainer::header_key()
{
    return _header_key;
}

QByteArray KeysContainer::masks_key()
{
    return _masks_key;
}

QByteArray KeysContainer::masks2_key()
{
    return _masks2_key;
}

QByteArray KeysContainer::name_key()
{
    return _name_key;
}

QByteArray KeysContainer::primary_key()
{
    return _primary_key;
}

QByteArray KeysContainer::primary2_key()
{
    return _primary2_key;
}

void KeysContainer::set_header_key(const QByteArray &value)
{

}

void KeysContainer::set_masks_key(const QByteArray &value)
{
    _masks_key = value;
}

void KeysContainer::set_masks2_key(const QByteArray &value)
{
    _masks2_key = value;
}

void KeysContainer::set_name_key(const QByteArray &value)
{
    _name_key = value;
}

void KeysContainer::set_primary_key(const QByteArray &value)
{
    _primary_key = value;
}

void KeysContainer::set_primary2_key(const QByteArray &value)
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

    QTemporaryDir temp;
    if(!temp.isValid())
        return false;


    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);
    QByteArray data = toByteArray();
    if(data == "")
        return false;

    QSqlQuery query(_db->getDatabase());
    query.prepare("INSERT INTO [dbo].[Containers] ([Ref], [FirstField], [data]) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(uuid);
    query.addBindValue(name().trimmed());
    query.addBindValue(data);
    query.exec();

    if (query.lastError().type() != QSqlError::NoError)
    {
        qDebug() << __FUNCTION__ << query.lastError();
        return false;
    }

    return true;

}

QSettings KeysContainer::toQSettings()
{
    return QSettings{};
}

void KeysContainer::fromQSettings(const QSettings &value)
{

}

QByteArray KeysContainer::toByteArray()
{
    if(!_isValid){
        qCritical() << __FUNCTION__ <<  "Данные не инициализированы!";
        return "";
    }

    if(_name.isEmpty()){
        qCritical() << __FUNCTION__ <<  "Не указано наименование контейнера!";
        return "";
    }

    QTemporaryDir temp;
    if(!temp.isValid())
        return "";

    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);

    QString tempFile = QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".ini");
    QSettings settings = QSettings(tempFile, QSettings::IniFormat, this);

    settings.beginGroup(name());
    for (int i = 0; i < KeyFiles.size(); ++i) {
        settings.setValue(KeyFiles[i], get_get_function(i)());
    }
    settings.endGroup();
    settings.sync();

    if(settings.status() != QSettings::NoError){
        qCritical() << __FUNCTION__ << settings.status();
        return "";
    }

    QFile fdata(tempFile);
    if(fdata.open(QIODevice::ReadOnly)){
        QByteArray data = fdata.readAll();
        fdata.close();
        fdata.remove();
        return data;
    }

    return "";
}

QJsonObject KeysContainer::toJsonObject(JsonFormat format, const QUuid& uuid)
{

    QJsonObject obj = QJsonObject();
    if(format == nameData){
        QByteArray data = toByteArray();
        if(data == "")
            return obj;

        obj.insert("name", name());
        obj.insert("data",  QJsonValue::fromVariant(data));

    }else if(format == forDatabase){
        QUuid _uuid = uuid;
        if(_uuid.isNull()){
            _uuid = QUuid::createUuid();
        }

        QString sz_uuid = _uuid.toString();
        sz_uuid = sz_uuid.mid(1, sz_uuid.length() - 2);

        QByteArray data = toByteArray();
        if(data == "")
            return obj;

        obj.insert("Ref", sz_uuid);
        obj.insert("name", name());
        obj.insert("data",  QJsonValue::fromVariant(data));

    }else if(format == serialization){
        for(int i = 0; i < KeyFiles.size(); ++i){
            QString key = KeyFiles[i];
            QByteArray data = get_get_function(i)();
            QJsonValue val = QJsonValue::fromVariant(data);
            obj.insert(key, val);
        }
    }

    return obj;

}

QString KeysContainer::path()
{
    return _path;
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
            f = std::bind(&KeysContainer::header_key, this);
            break;
        }
        case 1:{
            f = std::bind(&KeysContainer::masks_key, this);
            break;
        }
        case 2:{
            f = std::bind(&KeysContainer::masks2_key, this);
            break;
        }
        case 3:{
            f = std::bind(&KeysContainer::name_key, this);
            break;
        }
        case 4:{
            f = std::bind(&KeysContainer::primary_key, this);
            break;
        }
        case 5:{
            f = std::bind(&KeysContainer::primary2_key, this);
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
        QFile file(folder + QDir::separator() + key);
        if(file.open(QIODevice::ReadOnly)){
            func[key.toStdString()](file.readAll());
        }else
            return;
    }
    _isValid = true;
}

void KeysContainer::fromRegistry(const QString &name, const QString &sid)
{
    _isValid = false;
    auto func = set_function();


    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, name);
    QSettings reg = QSettings(root, QSettings::NativeFormat);
    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        func[key.toStdString()](reg.value(key).toByteArray());
        auto funcGet = get_get_function(i);
        QByteArray data = funcGet();
        if(data.isEmpty())
            return;
    }

//    _header_key = reg.value("header.key").toByteArray();
//    _masks_key = reg.value("masks.key").toByteArray();
//    _masks2_key = reg.value("masks2.key").toByteArray();
//    _name_key = reg.value("name.key").toByteArray();
//    _primary_key = reg.value("primary.key").toByteArray();
//    _primary2_key = reg.value("primary2.key").toByteArray();
    _path = root;
    _name = name;
    _isValid = true;
}

bool KeysContainer::isValid()
{
    return _isValid;
}
