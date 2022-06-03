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
    //QTemporaryFile file("ini");
    QTemporaryDir temp;
    if(!temp.isValid())
        return false;

    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);

    QString tempFile = QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".ini");
    QSettings settings = QSettings(tempFile, QSettings::IniFormat, this);
//        qDebug() << __FUNCTION__ << settings.status();
//        qDebug() << __FUNCTION__ << settings.isWritable();
//        qDebug() << __FUNCTION__<< tempFile;

    settings.beginGroup(name());
    settings.setValue("header.key", header_key());
    settings.setValue("masks.key", masks_key());
    settings.setValue("masks2.key", masks2_key());
    settings.setValue("name.key", name_key());
    settings.setValue("primary.key", primary_key());
    settings.setValue("primary2.key", primary2_key());
    settings.endGroup();
    settings.sync();

    if(settings.status() != QSettings::NoError){
        qDebug() << __FUNCTION__ << settings.status();
        return false;
    }

    QFile fdata(tempFile);
   if(fdata.open(QIODevice::ReadOnly)){
        QByteArray data = fdata.readAll();
        fdata.close();

        QUuid _uuid = QUuid::createUuid();
        QString uuid = _uuid.toString().mid(1, 36);

        QSqlQuery query(_db->getDatabase());
        query.prepare("INSERT INTO [dbo].[Containers] ([Ref], [FirstField] ,[data]) "
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

    }else
       return false;


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
    if(_isValid){
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
    settings.setValue("header.key", header_key());
    settings.setValue("masks.key", masks_key());
    settings.setValue("masks2.key", masks2_key());
    settings.setValue("name.key", name_key());
    settings.setValue("primary.key", primary_key());
    settings.setValue("primary2.key", primary2_key());
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

std::map<std::string, set_keys> KeysContainer::set_function()
{
    std::map<std::string, set_keys> f;

    f.emplace(KeyFiles[0], &KeysContainer::set_header_key);
    f.emplace(KeyFiles[1], &KeysContainer::set_masks_key);
    f.emplace(KeyFiles[2], &KeysContainer::set_masks2_key);
    f.emplace(KeyFiles[3], &KeysContainer::set_name_key);
    f.emplace(KeyFiles[4], &KeysContainer::set_primary_key);
    f.emplace(KeyFiles[5], &KeysContainer::set_primary2_key);

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
