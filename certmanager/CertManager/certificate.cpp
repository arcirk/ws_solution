#include "certificate.h"
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QEventLoop>
#include "commandline.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QApplication>


Certificate::Certificate(QObject *parent)
    : QObject{parent}
{
    _isValid = false;
    QString uuid = QUuid::createUuid().toString();
    _ref = uuid.mid(1, uuid.length() - 2);
}

void Certificate::setSourceObject(const QJsonObject &obj)
{
    _isValid = false;

    sourceObject = obj;

    QString sSubj = obj.value("Subject").toString();
    int ind = sSubj.indexOf("CN=");
    if(ind != -1)
        _subject = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed().replace("\"", "");

    QString t;
    QString g;
    QString sn;
    ind = sSubj.indexOf("T=");
    if(ind != -1)
        t = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();

    ind = sSubj.indexOf("G=");
    if(ind != -1)
        g = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();

    ind = sSubj.indexOf("SN=");
    if(ind != -1)
        sn = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed();

    _parentUser = t + ", " + sn + " " + g;

    sSubj = obj.value("Issuer").toString();
    ind = sSubj.indexOf("CN=");
    if(ind != -1)
        _issuer = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed().replace("\"", "");

    sSubj = obj.value("Container").toString().replace("\r", "");
    _container = sSubj.trimmed();

    sSubj = obj.value("Not valid after").toString().replace("\r", "");
    _notValidAfter = sSubj.trimmed().replace("/", ".");

    sSubj = obj.value("Not valid before").toString().replace("\r", "");
    _notValidBefore = sSubj.trimmed().replace("/", ".");

    sSubj = obj.value("Serial").toString().replace("\r", "");
    _serial = sSubj.trimmed();

   _isValid = true;
}

bool Certificate::isValid()
{
    return _isValid;
}

QString Certificate::subject() const
{
    return _subject;
}

QString Certificate::issuer() const
{
    return _issuer;
}

QString Certificate::container() const
{
    return _container;
}

QString Certificate::notValidBefore() const
{
    return _notValidBefore;
}

QString Certificate::notValidAfter() const
{
    return _notValidAfter;
}

QString Certificate::parentUser() const
{
    return _parentUser;
}

QString Certificate::serial() const
{
    return _serial;
}

ByteArray Certificate::data()
{
    return _data;
}

QString Certificate::ref()
{
    return _ref;
}

void Certificate::setRef(const QString &uuid)
{
    _ref = uuid;
}

void Certificate::setData(const ByteArray &dt)
{
    _data = dt;
}

QString Certificate::getParam(const QString &key, const QString &name)
{
    if(sourceObject.isEmpty())
        return "";

    QString sSubj = sourceObject.value(key).toString();
    int ind = sSubj.indexOf(name + "=");
    if(ind != -1)
        return getStringValue(ind + name.length() + 1, sSubj).replace("\r", "").trimmed().replace("\"", "");

    return "";
}

void Certificate::fromFile(const QString& fileName, bool removeSource){

   _isValid = false;

   auto cmd = new CommandLine(this, false, "CP1251");
   cmd->setMethod(3);
   //cmd->setProgram("powershell");
   QEventLoop loop;
   QJsonObject res;

   //Синхронно получаем инфу о сертификате

   auto started = [cmd, &fileName]() -> void
   {
       QString s = "certutil \"" + fileName + "\"";
       cmd->send(s, CmdCommand::certutilGetCertificateInfo);
   };
   connect(cmd, &CommandLine::cmdStarted, started);

   QString str;
   auto output = [cmd, &str](const QString& data, int command) -> void
   {
       if(command == CmdCommand::certutilGetCertificateInfo){
           if(data.indexOf("Microsoft Corporation") == -1){
                str.append(data);
                if(data.indexOf("CertUtil: -dump") != -1)
                    cmd->parseCommand(str, command);
           }
       }
   };
   connect(cmd, &CommandLine::output, output);

   auto parse = [&loop, cmd, &res](const QVariant& result, int command) -> void
   {
       if(command == CmdCommand::certutilGetCertificateInfo){
           auto doc = QJsonDocument::fromJson(result.toString().toUtf8());
           auto arr = doc.array();

           for (auto itr = arr.begin(); itr != arr.end(); ++itr) {
               auto obj = itr->toObject();
               res = obj;
               break;
           }
           cmd->stop();
           loop.quit();
       }

   };
   connect(cmd, &CommandLine::endParse, parse);

   auto err = [&loop, cmd](const QString& data, int command) -> void
   {
       qDebug() << __FUNCTION__ << data << command;
       cmd->stop();
       loop.quit();
   };
   connect(cmd, &CommandLine::error, err);

   cmd->start();
   loop.exec();

   ByteArray data;
   Base64Converter::readFile(fileName.toStdString(), data);

   if(removeSource){
       QFile f(fileName);
       f.remove();
   }

   delete cmd;

   setSourceObject(res);
   setData(data);

   _isValid = true;

   return;
}

QJsonObject Certificate::getObject()
{
    auto obj = QJsonObject();
    obj.insert("FirstField", QString("%1 %2-%3").arg(subject(), notValidBefore(), notValidAfter()));
    obj.insert("Ref", ref());
    obj.insert("subject", subject());
    obj.insert("issuer", issuer());
    obj.insert("privateKey", container());
    obj.insert("notValidBefore", notValidBefore());
    obj.insert("notValidAfter", notValidAfter());
    obj.insert("parentUser", parentUser());
    obj.insert("serial", serial());
    if(_data.size() > 0){
        std::string base64 = Base64Converter::byte_to_base64(&_data[0], _data.size());
        obj.insert("data", QString::fromStdString(base64));
    }
    return obj;
}

QBSqlQuery Certificate::getSqlQueryObject(QBSqlCommand command)
{
    auto bindQuery = QBSqlQuery(command, "[Certificates]");
    QJsonObject obj = QJsonObject();
    obj.insert("name", "Ref");
    obj.insert("value", ref());
    bindQuery.add_field(obj, bFieldType::qVariant);
    obj = QJsonObject();
    obj.insert("name", "FirstField");
    obj.insert("value", QString("%1 %2-%3").arg(subject(), notValidBefore(), notValidAfter()));
    bindQuery.add_field(obj, bFieldType::qVariant);
    obj = QJsonObject();
    obj.insert("name", "subject");
    obj.insert("value", subject());
    bindQuery.add_field(obj, bFieldType::qVariant);
    obj = QJsonObject();
    obj.insert("name", "issuer");
    obj.insert("value", issuer());
    bindQuery.add_field(obj, bFieldType::qVariant);
    obj = QJsonObject();
    obj.insert("name", "privateKey");
    obj.insert("value", container());
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
    obj.insert("name", "serial");
    obj.insert("value", serial());
    bindQuery.add_field(obj, bFieldType::qVariant);
    if(_data.size() > 0){
        std::string base64 = Base64Converter::byte_to_base64(&_data[0], _data.size());
        QByteArray dt = base64.data();
        obj = QJsonObject();
        obj.insert("name", "data");
        obj.insert("value", QString(dt.toBase64()));
        bindQuery.add_field(obj, bFieldType::qByteArray);
    }

    return  bindQuery;

}


void Certificate::fromData(ByteArray &cer)
{

    _isValid = false;
//    QTemporaryDir temp;
//    if(!temp.isValid())
//        return;


    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);
//    QTemporaryFile fo;
//    fo.open();fo.fileName()
    QString fileName = QCoreApplication::applicationDirPath() + QDir::separator() + uuid + ".tmp"; //QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".tmp");
    ByteArray data = cer;
    Base64Converter::writeFile(fileName.toStdString(), cer);


    if(!QFile::exists(fileName))
        return;

    auto cmd = new CommandLine(this, false, "CP1251");
    cmd->setMethod(3);
    cmd->setProgram("powershell");
    QEventLoop loop;
    QJsonObject res;

    //Синхронно получаем инфу о сертификате

    auto started = [cmd, &fileName]() -> void
    {
        QString s = "certutil \"" + fileName + "\"";
        cmd->send(s, CmdCommand::certutilGetCertificateInfo);
    };
    connect(cmd, &CommandLine::cmdStarted, started);

    auto output = [cmd](const QString& data, int command) -> void
    {
        if(command == CmdCommand::certutilGetCertificateInfo){
            cmd->parseCommand(data, command);
        }
    };
    connect(cmd, &CommandLine::output, output);

    auto parse = [&loop, cmd, &res](const QVariant& result, int command) -> void
    {
        if(command == CmdCommand::certutilGetCertificateInfo){
            auto doc = QJsonDocument::fromJson(result.toString().toUtf8());
            auto arr = doc.array();

            for (auto itr = arr.begin(); itr != arr.end(); ++itr) {
                auto obj = itr->toObject();
                res = obj;
                break;
            }
            cmd->stop();
            loop.quit();
        }

    };
    connect(cmd, &CommandLine::endParse, parse);

    auto err = [&loop, cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data << command;
        cmd->stop();
        loop.quit();
    };
    connect(cmd, &CommandLine::error, err);

    cmd->start();
    loop.exec();

    QFile f(fileName);
    f.remove();

    delete cmd;

    setSourceObject(res);
    setData(data);

    _isValid = true;

    return;
}

QString Certificate::getStringValue(int start, const QString &source)
{
    for (int i = start; i < source.length(); ++i) {
        QString s = source.mid(i, 1);
        if(s == "," || s == "\n" || s == "\r" || i == source.length() - 1)
            return source.mid(start, i - start);
    }

    return "";
}
