#include "commandline.h"
#include <QDebug>
#include <QRegExp>
#include <QRegularExpression>
#include <QTextCodec>
#include <comanndlineparser.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

CommandLine::CommandLine(QObject *parent, bool usesysstem, const QString& enc)
    : QObject{parent}
{

    connect(&m_process, &QProcess::errorOccurred, this, &CommandLine::errorOccured);
    connect(&m_process, &QProcess::readyReadStandardError, this, &CommandLine::readyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &CommandLine::readyReadStandardOutput);
    connect(&m_process, &QProcess::started, this, &CommandLine::start);
    connect(&m_process, &QProcess::stateChanged, this, &CommandLine::stateChanged);
    connect(&m_process, &QProcess::readyRead, this, &CommandLine::readyRead);

    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &CommandLine::finished);

    m_listening = false;
    _currentEncoding = enc;
    _verefyEncoding = false;
    _method = 0;
    _useSystem = usesysstem;
    _program = program;
    codec = QTextCodec::codecForName(_currentEncoding.toUtf8());
//    codec = QTextCodec::codecForName("IBM 866");
//    codec = QTextCodec::codecForName("Windows-1251");

    QTextCodec::setCodecForLocale(codec);

}


void CommandLine::errorOccured(QProcess::ProcessError err) {
    if(!m_listening) return;

    qInfo() << Q_FUNC_INFO << err;
    emit error("Error", _command);
}

void CommandLine::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if(!m_listening) return;
    //qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    emit output("Complete", unknown);
}

void CommandLine::readyReadStandardError() {
    if(!m_listening) return;
    //qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardError();
    QString message = encodeData(data, _method);
    //QString message = "Standard Error: ";
    //message.append(data);
    //    emit output("Standard Error: " + message, _command);

    emit error(message, _command);

    //emit output(message, _command);
}

void CommandLine::readyReadStandardOutput() {
    if(!m_listening) return;
//    qInfo() << Q_FUNC_INFO;
//    QByteArray data = m_process.readAllStandardOutput();
//    //emit output(QTextCodec::codecForName("CP866")->toUnicode(data.trimmed()));
//    QString s = QString(data.trimmed()).toLocal8Bit();
//    emit output("Output:" + s);
}

void CommandLine::started() {
    qInfo() << Q_FUNC_INFO;
}

void CommandLine::stateChanged(QProcess::ProcessState newState) {

    qInfo() << Q_FUNC_INFO << newState;
    switch (newState) {
        case QProcess::NotRunning:
            emit output("Not running", unknown);
            emit complete();
            break;
        case QProcess::Starting:
            emit output("Starting ...", unknown);
            break;
        case QProcess::Running:{
            emit output("Running", unknown);
            emit cmdStarted();
        }
            break;
    }
}

void CommandLine::readyRead() {
    if(!m_listening) return;
    //qInfo() << Q_FUNC_INFO;
    //QTextDecoder *decoder = codec->makeDecoder(QTextCodec::IgnoreHeader);
    QByteArray data = m_process.readAll();//.trimmed();
    QStringList pres = {"______________________QTextCodec::codecForName(" + _currentEncoding + ")->toUnicode(data)______________________________\n"
                       , "______________________codec->toUnicode(data)______________________________\n"
                       , "______________________QString::fromUtf8(data)______________________________\n"
                       , "______________________QString::fromLocal8Bit(data)______________________________\n"};

    if(_verefyEncoding){

        for (int i = 0; i < 4; ++i) {
            if(i != _method){
               emit output(pres[i], unknown);
               emit output(encodeData(data, i), unknown);
            }
            else{
                emit output(pres[i], _command);
               emit output(encodeData(data, i), _command);
            }
        }

    }else
        emit output(encodeData(data, _method), _command);

//    if(_verefyEncoding){
//        emit output("______________________codec->toUnicode(data)______________________________\n", unknown);
//        emit output(codec->toUnicode(data), unknown);
//        emit output("______________________QString::fromUtf8(data)______________________________\n", unknown);
//        emit output(QString::fromUtf8(data), unknown);
//        emit output("______________________QTextCodec::codecForName(current CP)->toUnicode(data)______________________________\n", unknown);
//    }
//    emit output(QTextCodec::codecForName(_currentEncoding.toStdString().c_str())->toUnicode(data), _command);
//    if(_verefyEncoding){
//        emit output("______________________QString::fromLocal8Bit(data)______________________________\n", unknown);
//        emit output(QString::fromLocal8Bit(data), unknown);
//    }
//    emit output(codec->makeDecoder()->toUnicode(data, strlen(data)));
//    emit output(codec->toUnicode(data));
    //emit output(QTextCodec::codecForName("cp1251")->toUnicode(data));
    //emit output(QTextCodec::codecForName("CP866")->toUnicode(data));
    //qDebug().noquote()<<QString::fromUtf8(data);
    //qInfo() << data;
   // emit output(data);

}

void CommandLine::onEndParse(const QVariant &result, int cmd)
{
    emit endParse(result.toString(), cmd);
}

void CommandLine::setProgram(const QString &name)
{
    _program = name;
}

QString CommandLine::encodeData(const QByteArray &data, int m)
{
    if(m==0){
        return QTextCodec::codecForName(_currentEncoding.toStdString().c_str())->toUnicode(data);
    }else if(m==1){
        return codec->toUnicode(data);
    }else if(m==2){
        return QString::fromUtf8(data);
    }else if(m==3){
        return QString::fromLocal8Bit(data);
    }else
        return QTextCodec::codecForName(_currentEncoding.toStdString().c_str())->toUnicode(data);
}

std::string CommandLine::executeSystem(const std::string &cmd)
{
    std::string file_name = "result.txt" ;
    std::string _cmd = cmd;
    _cmd.append(" > " + file_name);
    int r = std::system( _cmd.c_str());
    qDebug() << __FUNCTION__ << r;
    //std::_wsystem( cmd + " > " + file_name ).c_str() ) ; // redirect output to file

    // open file for input, return string containing characters in the file
    std::ifstream file(file_name) ;
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() } ;
}

QString CommandLine::getLine(const QString &source, int start)
{
    for (int i = start; i < source.length(); ++i) {
        QString s = source.mid(i, 1);
        if(s == "\n" || s == "\r" || i == source.length() - 1)
            return source.mid(start, i - start);
    }

    return "";
}

void CommandLine::setChcp()
{
    QRegularExpression re( "866");
    if(re.match(_currentEncoding).hasMatch()){
        send("chcp 866\n", unknown);
        return;
    }

    re = QRegularExpression("1251");
    if(re.match(_currentEncoding).hasMatch()){
        send("chcp 1251\n", unknown);
        return;
    }
}

void CommandLine::setCurrentEncoding(const QString &enc)
{

    if(enc.isEmpty())
        return;
    if(_currentEncoding != enc){
        _currentEncoding = enc;
        QRegularExpression re( "866");
        if(re.match(_currentEncoding).hasMatch()){
            send("chcp 866\n", unknown);
            return;
        }

        re = QRegularExpression("1251");
        if(re.match(_currentEncoding).hasMatch()){
            send("chcp 1251\n", unknown);
            return;
        }

        codec = QTextCodec::codecForName(_currentEncoding.toUtf8());
        QTextCodec::setCodecForLocale(codec);
    }
}

QString CommandLine::currentEncoding()
{
    return _currentEncoding;
}

void CommandLine::setVerefyEncodingg(bool val)
{
    _verefyEncoding = val;
}

bool CommandLine::verefyEncoding()
{
    return _verefyEncoding;
}

void CommandLine::setMethod(int val)
{
    _method = val;
}

int CommandLine::method()
{
    return _method;
}

bool CommandLine::useSystem()
{
    return _useSystem;
}

void CommandLine::setUseSystem(bool val)
{
    _useSystem = val;
}

void CommandLine::start() {

    //qInfo() << Q_FUNC_INFO;
    if(m_listening)
        return;

    m_listening = true;

    //QStringList chcp = {"chcp " + _currentEncoding};

    if(!useSystem())
        m_process.start(_program);


}

void CommandLine::stop() {
    //qInfo() << Q_FUNC_INFO;
    m_listening = false;
    if(m_process.state() == QProcess::Running)
        m_process.close();
}

void CommandLine::send(const QString &commandText, int command)
{
    _strPart = "";
    QString _commandText = commandText;
    if(QString(commandText).right(1) != "\n")
        _commandText = commandText + "\n";

    if(program == "powershell"){
        _commandText.replace("\"", "'");
        if(_commandText.left(QString("csptest").length()) == "csptest"){
            _commandText = ".\\" + _commandText;
        }
    }

    _lastCommand = _commandText;

    _command = command;
    if(!useSystem()){
        if(m_listening){
            //QTextCodec *codec = QTextCodec::codecForName("CP866");
            m_process.write(codec->fromUnicode(_commandText));
        }
    }else{
        std::string _result = executeSystem(_commandText.toStdString());
        emit output(QString::fromStdString(_result), command);
    }
}

//void CommandLine::send(const QByteArray &commandText, int command)
//{
////    QString _commandText = commandText;
////    if(QString(commandText).right(1) != "\n")
////        _commandText = commandText + "\n";

//    _lastCommand = commandText;

//    _command = command;
//    if(!useSystem()){
//        if(m_listening){
//            m_process.write(commandText);
//        }
//    }else{
////        std::string _result = executeSystem(_commandText.toStdString());
////        emit output(QString::fromStdString(_result), command);
//    }
//}

void CommandLine::clearBuffer()
{
//    send("cls\n", unknown);
//    _command = echoSystem;
//    m_process.setReadChannel(QProcess::StandardError);
//    m_process.setReadChannel(QProcess::StandardOutput);
}

void CommandLine::setParams(QStringList& param) {

    params = std::move(param);

}

bool CommandLine::listening()
{
    return m_listening;
}

QString CommandLine::parseCommand(const QString &result, int command)
{
    if(result == "Not running")
        return "";

    QStringList _result;
    if(command == echoUserName){
        QRegularExpression  re( "echo %username%\n");
        if(re.match(result).hasMatch()){
            int length = QString("echo %username%\n").length();
            int l = result.lastIndexOf(re, length);
            int in = result.indexOf(re, length);
            int fwd = l > in ? l : in;
            int start = fwd + length;
            if(fwd >= 0){
                for(int i = start; i < result.length(); ++i){
                    QString s = result.mid(i, 1);
                    if(s == " " || s == "\n" || s == "\r"){
                        QString _res = result.mid(start, i - start);
                        emit endParse(_res, command);
                        break;
                    }
                }
            }
        }
    }else if(command == wmicGetSID){
        QString str(result);
        QRegularExpression  re( "S-1");
        int length = QString("S-1").length();
        int l = result.lastIndexOf(re, length);
        int in = result.indexOf(re, length);
        int fwd = l > in ? l : in;        
        if(fwd >= 0){
            for(int i = fwd; i < str.length(); ++i){
                QString s = str.mid(i, 1);
                if(s == " " || s == "\n" || s == "\r"){
                    QString _res = str.mid(fwd, i - fwd);
                    emit endParse(_res, command);
                    break;
                }
            }
        }
        QFile log("std.log");
        if(log.open(QIODevice::WriteOnly)){
            log.write(str.toStdString().c_str(), str.length());
            log.close();
        }
    }else if(command == echoGetEncoding){
        QString str(result);
        str.replace("\r", "");
        _result = str.split("\n");
        int count = _result.size() - 1;
        QRegularExpression  re( "chcp");
        QRegularExpression  re1( "Текущая кодовая страница:");
        for (int i = count; i > 0; --i) {
            if(!_result[i].isEmpty()){
                QString tmp = _result[i];
                if(re.match(tmp).hasMatch()){
                    continue;
                }else if(tmp.right(1)== ">"){
                    continue;
                }else if(tmp == ""){
                    continue;
                }else if(re1.match(tmp).hasMatch()){
                    QStringList _res = tmp.split(":");
                    if(_res.size() > 1){
                        QString __res = _res[1].trimmed();
                        _currentEncoding = "CP" + __res;
                        break;
                    }
                }else{
                    continue;
                }
            }
        }
    }else if(command == csptestGetConteiners){
        QString tmp(result);
        int l = tmp.indexOf("\\\\.\\");
        int e = tmp.lastIndexOf("OK.");

        //qDebug() << l << " " << e;
        if(l > 0 && e > 0 && l < e){
            tmp = tmp.mid(l, e - l);
            emit endParse(tmp, command);
        }

        if(tmp.indexOf("ErrorCode:") != -1)
            emit endParse("", command);

    }else if(command == csptestContainerFnfo){
        int ind = result.indexOf("KP_CERTIFICATE:");
        if(ind > 0){
            QString _str = result.right(result.length() - ind);
            int pKey = _str.indexOf("PrivKey:");
            int endpKey = _str.indexOf("\n", pKey);
            QString _info = _str.left(pKey);
            QString tmp = _str.mid(pKey, endpKey - pKey);
            _info.append("\n" + tmp);
            emit endParse(_info.replace("\r", ""), command);
        }
    }else if(command == csptestContainerCopy){
       int ind = result.indexOf("[ErrorCode: 0x00000000]");
       if(ind > 0){
            emit endParse("OK", command);
       }
    }else if(command == csptestContainerDelete){
        int ind = result.indexOf("[ErrorCode: 0x00000000]");
        if(ind > 0){
             emit endParse("OK", command);
        }
    }else if(command == csptestGetCertificates){

        int ind = result.indexOf("[ErrorCode: 0x00000000]");
        if(ind > 0){

            _strPart = _strPart + result;

            QString str = _strPart;
            QStringList results;
            int j = _strPart.length(); //0;
            int endIndex = _strPart.length();

            while ((j = str.lastIndexOf("Issuer", j)) != -1) {

                    if(j > 0){
                        results.append(str.mid(j, endIndex - j));
                    }
                    --j;
                    endIndex = j;
            }

            //QStringList lst;
            auto arr = QJsonArray();

            foreach(auto certText, results){
                QStringList l = certText.split("\n");
                QString p;
                auto obj = QJsonObject();

                foreach(auto line, l){
                    QStringList s = certText.split(":");
                    if(s.size() < 2){
                        continue;
                    }
                    int ind = line.indexOf(":");
                    if(ind != -1){
                        obj.insert(line.left(ind - 1).trimmed(), line.right(line.length() - 1 - ind).trimmed());
                        p.append(line + "\n");
                    }
                }

                arr.append(obj);
                //lst.append(p);
            }
            auto doc = QJsonDocument();
            doc.setArray(arr);
            emit endParse(QString(doc.toJson()), command);

            return _strPart;

        }else{
            _strPart = _strPart + result; //рвет сообщение почему то
            return "";
        }

    }else if(command == certutilGetCertificateInfo){

        //qDebug() << qPrintable(result);
        int ind = result.indexOf("CertUtil: -dump");
        if(ind > 0){
            QString line = getLine(result, ind);
            if(!line.isEmpty()){
                QStringList r = line.split(":");
                if(r.size() == 2){
                    QString s = r[1].trimmed();
                    if(s == "-dump — команда успешно выполнена."){
                        //QStringList lst;
                        auto arr = QJsonArray();
                        auto obj = QJsonObject();
                        ind = result.indexOf("Серийный номер:");
                        if(ind > 0){
                            QString sz = getLine(result, ind);
                            sz.replace("Серийный номер", "Serial");
                            auto s_r = sz.split(":");
                            obj.insert(s_r[0].trimmed(), s_r[1].trimmed());
                        }
                        ind = result.indexOf("Поставщик:");
                        if(ind > 0){
                            int ind_ = result.indexOf("Хэш имени (md5)", ind);
                            if(ind_ != -1){
                                QString sz_ = getLine(result, ind_);
                                int ind__ = result.lastIndexOf(sz_);
                                if(ind__ != -1){
                                    QString Issuer = result.mid(ind, ind__ - ind);
                                    Issuer.replace("\n", "");
                                    auto s_r = Issuer.split(":");
                                    obj.insert("Issuer", s_r[1].trimmed());
                                }
                            }
                        }
                        ind = result.indexOf("NotBefore:");
                        if(ind > 0){
                            auto s_r = getLine(result, ind).trimmed().split(":");
                            obj.insert("Not valid before", s_r[1].trimmed());
                        }
                        ind = result.indexOf("NotAfter:");
                        if(ind > 0){
                            auto s_r = getLine(result, ind).trimmed().split(":");
                            obj.insert("Not valid after", s_r[1].trimmed());
                        }
                        ind = result.indexOf("Субъект:");
                        if(ind > 0){
                            int ind_ = result.indexOf("Хэш имени (md5)", ind);
                            if(ind_ != -1){
                                QString sz_ = getLine(result, ind_);
                                int ind__ = result.lastIndexOf(sz_);
                                if(ind__ != -1){
                                    QString subject = result.mid(ind, ind__ - ind);
                                    subject.replace("\n", "");
                                    auto s_r = subject.split(":");
                                    obj.insert("Subject", s_r[1].trimmed());
                                }
                            }
                        }
                        ind = result.indexOf("Хеш сертификата(sha1):");
                        if(ind > 0){
                            auto s_r = getLine(result, ind).trimmed().split(":");
                            obj.insert("SHA1 Hash", s_r[1].trimmed());
                        }
                        auto doc = QJsonDocument();
                        arr.append(obj);
                        doc.setArray(arr);
                        emit endParse(QString(doc.toJson()), command);
                    }
                }else{
                    emit error(line, certutilGetCertificateInfo);
                    if(listening())
                        return "";
                }
            }
        }
    }else if(command == certmgrInstallCert){
        int ind = result.indexOf("ErrorCode: 0x00000000");
        if(ind != -1)
            emit endParse("sucsess", command);
    }else{
        emit endParse("", command);
    }
    return result;
}
