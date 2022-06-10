#include "certificate.h"
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QEventLoop>
#include "commandline.h"
#include <QJsonDocument>
#include <QJsonArray>
//#include <QTextCodec>

Certificate::Certificate(QObject *parent)
    : QObject{parent}
{
    _isValid = false;
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

void Certificate::fromData(ByteArray &cer)
{

    _isValid = false;
//    QTemporaryDir temp;
//    if(!temp.isValid())
//        return;


    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.mid(1, uuid.length() - 2);
    QTemporaryFile fo;
    fo.open();
    QString fileName = fo.fileName(); //temp.path() + QDir::separator() + uuid + ".tmp"; //QDir::toNativeSeparators(temp.path() + QDir::separator() + uuid + ".tmp");
    ByteArray data = cer;
    Base64Converter::writeFile(fileName.toStdString(), cer);

    if(!QFile::exists(fileName))
        return;

    auto cmd = new CommandLine(this, false, "CP866");

    cmd->setProgram("powershell");
    QEventLoop loop;
    QJsonObject res;

    //Синхронно получаем инфу о сертификате

    auto started = [cmd, &fo]() -> void
    {
        QString s = "certutil \"" + fo.fileName() + "\"";
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
