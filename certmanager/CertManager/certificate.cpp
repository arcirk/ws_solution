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
    //_dnFindString.clear();

    QString sSubj = obj.value("Subject").toString();
    int ind = sSubj.indexOf("CN=");
    if(ind != -1)
        _subject = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed().replace("\"", "");

    QString t;
    QString g;
    QString sn;
    ind = sSubj.indexOf("T=");
    if(ind != -1){
        t = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();
        //_dnFindString.append("T=" + t);
    }

    ind = sSubj.indexOf("G=");
    if(ind != -1){
        g = getStringValue(ind + 2, sSubj).replace("\r", "").trimmed();
        //_dnFindString.append("G=" + g);
    }

    ind = sSubj.indexOf("SN=");
    if(ind != -1){
        sn = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed();
        //_dnFindString.append("SN=" + sn);
    }
    _parentUser = t + ", " + sn + " " + g;

    ind = sSubj.indexOf("ИНН=");
    if(ind != -1){
        sn = getStringValue(ind + 3, sSubj).replace("\r", "").trimmed();
        //_dnFindString.append("SN=" + sn);
    }

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

    _sha1 = sha1Hash();

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

QString Certificate::dataformat() const
{
    return _dataformat;
}

ByteArray Certificate::data()
{
    return _data;
}

QString Certificate::ref()
{
    return _ref;
}

QString Certificate::sha1()
{
    return _sha1;
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

bool Certificate::install(const QString& cnt)
{
    if(_data.size() == 0)
        return false;

    QTemporaryDir tmpDir;
    auto cmd = new CommandLine(this, false);
    QEventLoop loop;
    bool res = false;
    QString certmgrParentFolder = "C:\\Program Files (x86)\\Crypto Pro\\CSP";

#ifndef Q_OS_WINDOWS
    certmgrParentFolder == "ToDo";
#endif

    QDir dir(certmgrParentFolder);
    if(!dir.exists()){
        qCritical() << __FUNCTION__ << "Не установлен КриптоПро!";
        return false;
    }

    QString fileName = QDir::toNativeSeparators(tmpDir.path() + QDir::separator() + QUuid::createUuid().toString() + ".cer");
    Base64Converter::writeFile(QTextCodec::codecForName("CP1251")->fromUnicode(fileName).toStdString(), _data);

    QFile file(fileName);
    if(!file.exists())
        return false;

    auto started = [cmd, &fileName, &certmgrParentFolder, &cnt]() -> void
    {
        if(cnt.isEmpty())
            cmd->send(QString("cd \"%1\" & certmgr -inst -file \"%2\" & exit").arg(certmgrParentFolder, fileName), certmgrInstallCert);
        else
            cmd->send(QString("cd \"%1\" & certmgr -inst -file \"%2\" -cont \"%3\" & exit").arg(certmgrParentFolder, fileName, cnt), certmgrInstallCert);
    };
    loop.connect(cmd, &CommandLine::cmdStarted, started);

    auto output = [cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << qPrintable(data);
        if(command == CmdCommand::certmgrInstallCert){
            if(data.indexOf("Microsoft Corporation") == -1){
               cmd->parseCommand(data, command);
            }
        }
    };
    loop.connect(cmd, &CommandLine::output, output);

    auto parse = [&loop, cmd, &res](const QVariant& result, int command) -> void
    {
        if(command == CmdCommand::certmgrInstallCert){
            res = true;
            cmd->stop();
            loop.quit();
        }

    };
    loop.connect(cmd, &CommandLine::endParse, parse);

    auto err = [&loop, cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data << command;
        cmd->stop();
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::error, err);

    auto state = [&loop]() -> void
    {
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::complete, state);

    cmd->start();
    loop.exec();
    delete cmd;

    if(file.exists())
        file.remove();

    return res;
}

bool Certificate::remove()
{

    if(_sha1.isEmpty()){
        qCritical() << __FUNCTION__ << "Хеш сертификата не указан!";
        return false;
    }

    auto cmd = new CommandLine(this, false);
    QEventLoop loop;
    bool res = false;
    QString certmgrParentFolder = "C:\\Program Files (x86)\\Crypto Pro\\CSP";

    QDir dir(certmgrParentFolder);
    if(!dir.exists()){
        qCritical() << __FUNCTION__ << "Не установлен КриптоПро!";
        return false;
    }

    QString __sha = sha1();
    auto started = [cmd, &certmgrParentFolder, &__sha]() -> void
    {
        QString _qbyte = QString("certmgr -delete -thumbprint \"%1\"\n").arg(__sha);

        cmd->send(QString("cd \"%1\" & %2 & exit").arg(certmgrParentFolder, _qbyte), certmgrDeletelCert);
    };
    loop.connect(cmd, &CommandLine::cmdStarted, started);

    auto output = [cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << qPrintable(data);
        if(command == CmdCommand::certmgrDeletelCert){
            if(data.indexOf("Microsoft Corporation") == -1){
               cmd->parseCommand(data, command);
            }
        }
    };
    loop.connect(cmd, &CommandLine::output, output);

    auto parse = [&loop, cmd, &res](const QVariant& result, int command) -> void
    {
        if(command == CmdCommand::certmgrDeletelCert){
            res = true;
            cmd->stop();
            loop.quit();
        }

    };
    loop.connect(cmd, &CommandLine::endParse, parse);

    auto err = [&loop, cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data << command;
        cmd->stop();
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::error, err);

    auto state = [&loop]() -> void
    {
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::complete, state);

    cmd->start();
    loop.exec();
    delete cmd;

    return res;
}

bool Certificate::save(const QString &fileName)
{
    if(_data.size() == 0)
        return false;

    Base64Converter::writeFile(QTextCodec::codecForName("CP1251")->fromUnicode(fileName).toStdString(), _data);
    QFile file(fileName);

    return file.exists();
}

bool Certificate::fromSha1(const QString &sha)
{

    QTemporaryDir tmpDir;
    //tring tmpDir = std::getenv("TEMP");
    auto cmd = new CommandLine(this, false);//);"ISO 8859-1"
    QEventLoop loop;
    QJsonObject res;

    QString fileName = tmpDir.path() + QDir::separator() + QUuid::createUuid().toString() + ".cer";
    //qDebug() << fileName;
    auto started = [cmd, &fileName, &sha]() -> void
    {       
        QString qbyte = QString("cd \"%1\"").arg("C:\\Program Files (x86)\\Crypto Pro\\CSP");
        qbyte.append(QString(" & cryptcp -copycert -thumbprint \"%1\" -u -df \"%2\" & exit\n").arg(sha, fileName));
        cmd->send(qbyte, certmgrInstallCert);
    };
    loop.connect(cmd, &CommandLine::cmdStarted, started);

    auto output = [cmd, &loop](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data;
        if(command == CmdCommand::certmgrInstallCert){
            if(data.indexOf("Microsoft Corporation") == -1){
                if(data.indexOf("ReturnCode:") != -1){
                    cmd->stop();
                    loop.quit();
                }else if(data.indexOf("ErrorCode:") != -1){
                    cmd->stop();
                    loop.quit();
                }
            }
        }
    };
    loop.connect(cmd, &CommandLine::output, output);

    auto err = [&loop, cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data << command;
        cmd->stop();
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::error, err);

    auto state = [&loop]() -> void
    {
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::complete, state);

    cmd->start();
    loop.exec();
    delete cmd;

    if(!QFile(fileName).exists())
        return false;

    fromFile(fileName, true);

    return _isValid;
}

void Certificate::fromFile(const QString& fileName, bool removeSource){

   _isValid = false;

   QFileInfo inf(fileName);
   QString suffix = inf.completeSuffix();

   auto cmd = new CommandLine(this, false); //, "CP1251");
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
   loop.connect(cmd, &CommandLine::cmdStarted, started);

   QString str;
   auto output = [cmd, &str](const QString& data, int command) -> void
   {
       if(command == CmdCommand::certutilGetCertificateInfo && cmd->listening()){
           if(data.indexOf("Microsoft Corporation") == -1){
                str.append(data);
                if(data.indexOf("CertUtil: -dump") != -1){
                    cmd->parseCommand(str, command);
                }
           }
       }
   };
   loop.connect(cmd, &CommandLine::output, output);

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
   loop.connect(cmd, &CommandLine::endParse, parse);

   auto err = [&loop, cmd](const QString& data, int command) -> void
   {
       qDebug() << __FUNCTION__ << data << command;
       cmd->stop();
       loop.quit();
   };
   loop.connect(cmd, &CommandLine::error, err);

   cmd->start();
   loop.exec();

   ByteArray data;
   Base64Converter::readFile(QTextCodec::codecForName("CP1251")->fromUnicode(fileName).toStdString(), data);

   if(removeSource){
       QFile f(fileName);
       f.remove();
   }

   delete cmd;

   setSourceObject(res);
   setData(data);
   _dataformat = suffix;
   _isValid = true;

   return;
}

void Certificate::fromModelObject(const QJsonObject &obj)
{
    _subject = obj.value("subject").toString();
    _issuer = obj.value("issuer").toString();
    _notValidBefore = obj.value("notValidBefore").toString();
    _notValidAfter = obj.value("notValidAfter").toString();
    _parentUser = obj.value("parentUser").toString();
    _container = obj.value("container").toString();
    _serial = obj.value("serial").toString();
    sourceObject = getObject();
    sourceObject.insert("SHA1 Hash", obj.value("sha1").toString());
}

void Certificate::fromObject(const QJsonObject &obj)
{
    _ref = obj.value("Ref").toString();
    _subject = obj.value("subject").toString();
    _ref = obj.value("Ref").toString();
    _issuer = obj.value("issuer").toString();
    _container = obj.value("privateKey").toString();
    _notValidBefore = obj.value("notValidBefore").toString();
    _notValidAfter = obj.value("notValidAfter").toString();
    _parentUser = obj.value("parentUser").toString();
    _serial = obj.value("serial").toString();
    _sha1 = obj.value("sha1").toString();
    std::string base64 = obj.value("data").toString().toStdString();
    _data = Base64Converter::base64_to_byte(base64);

}

void Certificate::loadFromTemporaryFile()
{



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
    obj.insert("sha1", _sha1);
    if(_data.size() > 0){
        std::string base64 = Base64Converter::byte_to_base64(&_data[0], _data.size());
        obj.insert("data", QString::fromStdString(base64));
    }
    return obj;
}

QBSqlQuery Certificate::getSqlQueryObject(QBSqlCommand command)
{
    auto bindQuery = QBSqlQuery(command, "Certificates");
    bindQuery.addField("Ref", ref());
    bindQuery.addField("FirstField", QString("%1 %2-%3").arg(subject(), notValidBefore(), notValidAfter()));
    bindQuery.addField("subject", subject());
    bindQuery.addField("issuer", issuer());
    bindQuery.addField("privateKey", container());
    bindQuery.addField("notValidBefore", notValidBefore());
    bindQuery.addField("notValidAfter", notValidAfter());
    bindQuery.addField("parentUser", parentUser());
    bindQuery.addField("serial", serial());
    bindQuery.addField("sha1", _sha1);

    if(_data.size() > 0){
        std::string base64 = Base64Converter::byte_to_base64(&_data[0], _data.size());
        bindQuery.addField("data", QString::fromStdString(base64), bFieldType::qByteArray);
    }

    bindQuery.addField("dataformat", dataformat());

    return  bindQuery;

}

QString Certificate::dnFindString()
{
    if(sourceObject.isEmpty())
        return "";

    QString sSubj = sourceObject.value("Subject").toString();
    QString str = sSubj.replace("\r", "").trimmed();
    int ind = str.indexOf("OID.");
    if(ind != -1){
        QString result = getStringValue(ind, str);
        str = str.replace(result, "");
        ind = str.indexOf("OID.");
        result = result + "," + getStringValue(ind, str);
        return result;
    }else
        return str;
}

QString Certificate::sha1Hash()
{
    return sourceObject.value("SHA1 Hash").toString().replace("\r", "").trimmed();
}

QJsonObject Certificate::getSourceObject()
{
    return sourceObject;
}

QString Certificate::bindName()
{
    return  QString("%1 %2-%3").arg(subject(), notValidBefore(), notValidAfter());
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
    loop.connect(cmd, &CommandLine::cmdStarted, started);

    auto output = [cmd](const QString& data, int command) -> void
    {
        if(command == CmdCommand::certutilGetCertificateInfo){
            cmd->parseCommand(data, command);
        }
    };
    loop.connect(cmd, &CommandLine::output, output);

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
    loop.connect(cmd, &CommandLine::endParse, parse);

    auto err = [&loop, cmd](const QString& data, int command) -> void
    {
        qDebug() << __FUNCTION__ << data << command;
        cmd->stop();
        loop.quit();
    };
    loop.connect(cmd, &CommandLine::error, err);

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
