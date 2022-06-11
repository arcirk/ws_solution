#include "keyscontainer.h"
#include <QSettings>
#include <QFile>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QSqlQuery>
#include <QUuid>
#include <QSqlError>
#include <QJsonDocument>
#include <QJniObject>
#include "commandline.h"
#include <QEventLoop>
#include <sqlqueryinterface.h>

//#include <cstdio>
//#include <cstdlib>

#ifdef _WINDOWS
    #pragma warning(disable:4100)
    //#include <Windows.h>
    //#pragma comment(lib, "advapi32")
#endif

//#include <fstream>
//#include <iostream>
//#include <string>

//using std::wcout;
//using std::wstring;
//using std::vector;

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
    _cache = QJsonObject();
}

void KeysContainer::setName(const QString &value)
{
    _name = value;
}

QString KeysContainer::name() const
{
    return _name;
}

void KeysContainer::setVolume(const QString& value)
{
    _volume = value;
}

QString KeysContainer::volume() const
{
    return _volume;
}

void KeysContainer::setVolumePath(const QString &value)
{
    _volumePath = value;
}

QString KeysContainer::volumePath() const
{
    return _volumePath;
}

void KeysContainer::setKeyName(const QString &value)
{
    _key_name = value;
}

QString KeysContainer::keyName() const
{
    return _key_name;
}

bool KeysContainer::sync(const QString& sid)
{
    if(_volume.indexOf("REGISTRY") != -1){
        return syncRegystry(sid);
    }else if(_volume.indexOf("FAT12") != -1){
        return syncVolume();
    }else if(_volume.indexOf("HDIMAGE") != -1){
        return false;
    }else
        return false;
}

void KeysContainer::setRef(const QString &value)
{
    _ref = value;
}

QString KeysContainer::ref() const
{
    return _ref;
}

QString KeysContainer::storgare() const{
    return _storgare;
}

QJsonObject KeysContainer::cache() const
{
    return _cache;
}

void KeysContainer::setStorgare(const QString &value){
    _storgare = value;
}

void KeysContainer::setCache(const QJsonObject &obj)
{
    _cache = obj;
    if(!obj.isEmpty()){
        QString val = obj.value("PrivKey").toString();
        QStringList l = val.split("-");
        if(l.size() == 2){
            _notValidBefore = l[0].trimmed();
            _notValidAfter = l[1].replace("(UTC)", "").trimmed();
        }
        auto subObj = obj.value("Issuer").toObject();
        _issuer = subObj.value("CN").toString().replace("\"", "");
        subObj = obj.value("Subject").toObject();
        _subject = subObj.value("CN").toString().replace("\"", "");
        _parentUser = subObj.value("SN").toString() + " " + subObj.value("G").toString();
    }
}

void KeysContainer::fromContainerName(const QString &cntName)
{
    _name = "";
    _key_name = "";
    _volume = "";
    _storgare = "";
    _volumePath = "";

    _nameInStorgare = cntName;
    QStringList m_data = cntName.split("\\");
    QString _nameTmp;

    if(m_data.size() > 3){

        _volume = m_data[3].replace("\r", "");
        _storgare = QString("\\\\.\\%1\\").arg(_volume);

        QStringList m_vol =  _volume.split("_");
        setVolumePath("");
        if(m_vol.size() > 1){
    #ifdef _WINDOWS
            setVolumePath(m_vol[1] + ":\\");
    #else
            setVolumePath(m_vol[1]);
    #endif
        }

        _nameTmp = stringFromBase64(m_data[4]);

     }else
        _nameTmp = cntName;

    QStringList m_nameTmp= _nameTmp.split("@");
    if(m_nameTmp.size() > 1){
       _key_name = m_nameTmp[0];
       _nameTmp = m_nameTmp[1];
    }else
        _name = _nameTmp;


    QStringList dateAndName = _nameTmp.split("-");
    if(dateAndName.size() == 4){
        _notValidAfter = dateAndName[0] + "." + dateAndName[1] + "." + dateAndName[2];
        _name = dateAndName[3];
    }

}

void KeysContainer::header_key(ByteArray &value)
{
    value = _header_key;
}

void KeysContainer::masks_key(ByteArray &value)
{
    value = _masks_key;
}

void KeysContainer::masks2_key(ByteArray &value)
{
    value = _masks2_key;
}

void KeysContainer::name_key(ByteArray &value)
{
    value = _name_key;
}

void KeysContainer::primary_key(ByteArray &value)
{
    value = _primary_key;
}

void KeysContainer::primary2_key(ByteArray &value)
{
    value = _primary2_key;
}

void KeysContainer::set_header_key(const ByteArray &value)
{
    _header_key = value;
}

void KeysContainer::set_masks_key(const ByteArray &value)
{
    _masks_key = value;
}

void KeysContainer::set_masks2_key(const ByteArray &value)
{
    _masks2_key = value;
}

void KeysContainer::set_name_key(const ByteArray &value)
{
    _name_key = value;
}

void KeysContainer::set_primary_key(const ByteArray &value)
{
    _primary_key = value;
}

void KeysContainer::set_primary2_key(const ByteArray &value)
{
    _primary2_key = value;
}

QJsonObject KeysContainer::parseCsptestInfo(const QString &info)
{
    QStringList lst = info.split("\n");
    QJsonObject obj = QJsonObject();
    foreach(auto line, lst){
        if(line.trimmed() == "")
            continue;
        QStringList source;
        if(line.left(QString("Valid").length()) == "Valid" || line.left(QString("PrivKey").length()) == "PrivKey"){
            int ind = line.indexOf(":");
            QString key = line.left(ind).trimmed();
            QString val = line.right(line.length() - ind - 1).trimmed();
            obj.insert(key, val);
            if(key == "PrivKey"){
                QStringList l = val.split("-");
                if(l.size() == 2){
                    _notValidBefore = l[0].trimmed();
                    _notValidAfter = l[1].replace("(UTC)", "").trimmed();
                }
            }
            continue;
        }else
            source = line.split(":");
        if(source.size() > 0){
            QJsonObject subObj = QJsonObject();
            //QString details = source[1];

            QStringList subLst = source[1].split(",");
            bool is_address = false;
            QString street;
            foreach(auto item, subLst){
                QStringList val = item.split("=");
                if(val.size() > 1){
                    if(val[0].trimmed() == "STREET"){
                        is_address = true;
                        street = val[1].trimmed();
                        continue;
                    }else{
                        if(is_address){
                            is_address = false;
                            subObj.insert("STREET", street);
                        }
                    }
                    subObj.insert(val[0].trimmed(), val[1].trimmed());
                }else{
                    if(is_address){
                        street.append(" " + val[0].trimmed());
                        continue;
                    }
                    continue;
                }

            }
            if(!subObj.isEmpty()){
                QString _key = source[0].trimmed();
                if(_key == "Issuer"){
                    _issuer = subObj.value("CN").toString().replace("\"", "");
                }else if(_key == "Subject"){
                    _subject = subObj.value("CN").toString().replace("\"", "");
                    _parentUser = subObj.value("SN").toString() + " " + subObj.value("G").toString();
                }
                obj.insert(source[0].trimmed(), subObj);
            }
        }
    }

    _cache = obj;

    return obj;
}

void KeysContainer::infoFromDataBaseJson(const QString &json)
{
    auto doc = QJsonDocument::fromJson(json.toUtf8());
    if(!doc.isEmpty()){
       setCache(doc.object());
    }
}

bool KeysContainer::syncRegystry(const QString& sid)
{
    if(!isValid()){
        qCritical() << __FUNCTION__ << "класс не инициализирован!";
        return false;
    }

    if(sid.isEmpty()){
        qCritical() << __FUNCTION__ << "Требуется SID пользователя!";
        return false;
    }

    if(_name.isEmpty()){
        qCritical() << __FUNCTION__ << "Не указано имя контейнера!";
        return false;
    }

    QString ph = QString("SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, _name);
    ph.replace("\\HKEY_LOCAL_MACHINE\\", "");
    winreg::RegKey regKey = winreg::CreateKey(HKEY_LOCAL_MACHINE, ph.toStdWString().c_str());


    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        auto funcGet = get_get_function(i);
        ByteArray data;
        funcGet(data);
        if(data.size() == 0)
            return false;
        winreg::RegValue v(REG_BINARY);
        foreach(auto bt, data){
            v.Binary().push_back(bt);
        }
        SetValue(regKey.Get(), key.toStdWString().c_str(), v);
    }
    return true;
}

bool KeysContainer::syncVolume()
{
    if(!isValid()){
        qCritical() << __FUNCTION__ << "класс не инициализирован!";
        return false;
    }

    QString folder = volumePath() + keyName() + ".000";
    QDir dir(folder);
    if(dir.exists()){
        qCritical() << __FUNCTION__ << "Каталог контейнера уже существует!";
        return false;
    }

    QDir d(volumePath());
    if(!d.mkdir(keyName() + ".000"))
        return false;

    qDebug() << folder;

    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        auto funcGet = get_get_function(i);
        ByteArray data;
        funcGet(data);
        if(data.size() == 0)
            return false;
        Base64Converter::writeFile(QDir::toNativeSeparators(dir.path() + QDir::separator() + key).toStdString(),
                  data);
    }
    return true;
}

bool KeysContainer::removeContainerFromRegistry(const QString &sid, const QString &containerName)
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
        f.insert(std::pair<std::string, set_keys>(KeyFiles[i].toStdString(), get_set_function(i)));
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
        ByteArray buffer;
        Base64Converter::readFile(filename.toStdString(), buffer);
        func[key.toStdString()](buffer);
    }



    _isValid = true;
}

void KeysContainer::fromRegistry(const QString& sid, const QString& name)
{
    _isValid = false;

    fromContainerName(name);

    auto func = set_function();

    QString ph = QString("SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys\\%2").arg(sid, name);
    winreg::RegKey reg = winreg::OpenKey(HKEY_LOCAL_MACHINE, ph.toStdWString().c_str(), KEY_READ);
    if(!reg.IsValid())
        return;
    for(int i = 0; i < KeyFiles.size(); ++i){
        QString key = KeyFiles[i];
        winreg::RegValue v = winreg::QueryValue(reg.Get(), key.toStdWString());
        func[key.toStdString()](v.Binary());
        ByteArray data;
        get_get_function(i)(data);
        if(data.size() == 0)
            return;
    }

    _isValid = true;
}

void KeysContainer::fromJson(const QByteArray &data)
{

    _isValid = false;

    auto doc = QJsonDocument::fromJson(data);
    auto obj = doc.object();

    auto func = set_function();

    foreach(auto key , KeyFiles){
        QString value = obj.value(key).toString();
        if(value.isEmpty())
            return;

        ByteArray _data = Base64Converter::base64_to_byte(value.toStdString());
        func[key.toStdString()](_data);
    }

    _isValid = true;
}

QByteArray KeysContainer::toBase64()
{
    if(!isValid()){
        qCritical() << __FUNCTION__ << "класс не инициализирован!";
        return "";
    }

    auto doc = QJsonDocument();
    auto obj = QJsonObject();

    _isValid = false;
    foreach(auto key , KeyFiles){

        ByteArray buffer;
        get_get_function(KeyFiles.indexOf(key))(buffer);
        if(buffer.size() == 0)
            return "";
        std::string b64 = Base64Converter::byte_to_base64(&buffer[0], buffer.size());
        obj.insert(key, QString::fromStdString(b64));

    }

    doc.setObject(obj);
    _isValid = true;
    return doc.toJson();

}

//void KeysContainer::parseAdressKey(const QString& key){

//    QStringList m_data = key.split("\\");
//    QStringList __volume =  m_data[3].replace("\r", "").split("_");
//    setVolume((VolumeType)VolumeTypeString.indexOf(__volume[0]));
//    setVolumePath("");
//    if(__volume.size() > 1){
//#ifdef _WINDOWS
//        setVolumePath(__volume[1] + ":\\");
//#else
//        setVolumePath(__volume[1]);
//#endif
//    }
//    QString _nameBase64 =  m_data[4].replace("\r", "");
//    _name = stringFromBase64(_nameBase64);
//    int ind =  _name.indexOf("@");
//    _key_name = _name.left(ind);


////    QString volume =  device;
////    volume.replace("FAT12_", "");
////    QString nameBase64 =  m_data[4].replace("\r", "");
////    QString name = fromBase64(nameBase64);
////    int ind =  name.indexOf("@");
////    QString key_name = name.left(ind);

////    obj.insert("device", device);
////    obj.insert("nameBase64", nameBase64);
////    obj.insert("name", name);
////    obj.insert("key_name", key_name);
////    obj.insert("volume", volume);

////    return obj;
//}

bool KeysContainer::isValid()
{
    return _isValid;
}

//void KeysContainer::setWindowsSid(const QString &value)
//{
//    _sid = value;
//}

//void KeysContainer::writeFile(const std::string& filename, ByteArray& file_bytes){
//    std::ofstream file(filename, std::ios::out|std::ios::binary);
//    std::copy(file_bytes.cbegin(), file_bytes.cend(),
//        std::ostream_iterator<unsigned char>(file));
//}

//void KeysContainer::readFile(const std::string &filename, ByteArray &result)
//{

//    FILE * fp = fopen(filename.c_str(), "rb");

//    fseek(fp, 0, SEEK_END);
//    size_t flen= ftell(fp);
//    fseek(fp, 0, SEEK_SET);

//    std::vector<unsigned char> v (flen);

//    fread(&v[0], 1, flen, fp);

//    fclose(fp);

//    result = v;

//    qDebug() << __FUNCTION__ << result.size();

//    //std::string data( v.begin(), v.end() );
//}

QString KeysContainer::stringFromBase64(const QString &value)
{
    QString s = value.trimmed();
    QRegularExpression re("^[a-zA-Z0-9\\+/]*={0,3}$");
    bool isBase64 = (s.length() % 4 == 0) && re.match(s).hasMatch();
    if(!isBase64)
       return value;

    try {
        return QByteArray::fromBase64(value.toUtf8());
    }  catch (std::exception&) {
        return value;
    }
}

QString KeysContainer::fullName(){
    if(_notValidAfter.isEmpty())
        return "";
    QStringList s = _notValidAfter.split(" ");
    return _key_name + "@" + s[0].replace(".", "-") + "-" + _name;;
}

QBSqlQuery KeysContainer::getSqlQueryObject(QBSqlCommand command)
{

    if(!_isValid)
        return QBSqlQuery();

    if(_ref.isEmpty()){
       QString uuid = QUuid::createUuid().toString();
       _ref = uuid.mid(1, uuid.length() - 2);
    }

    auto bindQuery = QBSqlQuery(command, "Containers");
    QJsonObject obj = QJsonObject();
    obj.insert("name", "Ref");
    obj.insert("value", _ref);
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "FirstField");
    obj.insert("value", fullName()); //QString(name.toUtf8().toBase64())
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "SecondField");
    obj.insert("value", name());
    bindQuery.add_field(obj, bFieldType::qVariant);
    bindQuery.add_field_is_exists(obj);

    obj = QJsonObject();
    obj.insert("name", "subject");
    obj.insert("value", subject());
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "issuer");
    obj.insert("value", issuer());
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "notValidBefore");
    obj.insert("value", notValidBefore());
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "notValidAfter");
    obj.insert("value", notValidAfter());
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "parentUser");
    obj.insert("value", parentUser());
    bindQuery.add_field(obj, bFieldType::qVariant);

    obj = QJsonObject();
    obj.insert("name", "data");
    obj.insert("value", QString(toBase64()));
    bindQuery.add_field(obj, bFieldType::qByteArray);

    return  bindQuery;
}

QString KeysContainer::subject() const
{
    return _subject;
}

QString KeysContainer::issuer() const
{
    return _issuer;
}

//QString KeysContainer::container() const
//{
//    return _container;
//}

QString KeysContainer::notValidBefore() const
{
    return _notValidBefore;
}

QString KeysContainer::notValidAfter() const
{
    return _notValidAfter;
}

QString KeysContainer::parentUser() const
{
    return _parentUser;
}

QString KeysContainer::nameInStorgare() const
{
    return _nameInStorgare;
}
