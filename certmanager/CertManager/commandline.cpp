#include "commandline.h"
#include <QDebug>
#include <QRegExp>
#include <QRegularExpression>
#include <QTextCodec>

CommandLine::CommandLine(QObject *parent)
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
    _currentEncoding = "CP866";
    _verefyEncoding = false;
    _method = 0;


    codec = QTextCodec::codecForName(_currentEncoding.toUtf8());
//    codec = QTextCodec::codecForName("IBM 866");
//    codec = QTextCodec::codecForName("Windows-1251");

    QTextCodec::setCodecForLocale(codec);

}


void CommandLine::errorOccured(QProcess::ProcessError error) {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO << error;
    emit output("Error", _command);
}

void CommandLine::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if(!m_listening) return;
    //qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    emit output("Complete", _command);
}

void CommandLine::readyReadStandardError() {
    if(!m_listening) return;
    //qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardError();
    QString message = "Standard Error: ";
    message.append(data);
    emit output(message, _command);
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
            emit output("Not running", _command);
            break;
        case QProcess::Starting:
            emit output("Starting ...", _command);
            break;
        case QProcess::Running:
            emit output("Running", _command);
            break;
    }
}

void CommandLine::readyRead() {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
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

void CommandLine::start() {

    //qInfo() << Q_FUNC_INFO;
    if(m_listening)
        return;
//    if (params.isEmpty())
//        return;
    m_listening = true;
    //m_process.start(program, params);
    m_process.start(program);
}

void CommandLine::stop() {
    qInfo() << Q_FUNC_INFO;
    m_listening = false;
    m_process.close();
}

void CommandLine::send(const QString &commandText, cmdCommand command)
{
    if(m_listening){
        m_process.write(commandText.toUtf8());
        _lastCommand = commandText;
        _command = command;
    }
}

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

QString CommandLine::parseCommand(const QString &result, cmdCommand command)
{
    QStringList _result;
    if(command == echoUserName){
        QString str(result);
        str.replace("\r", "");
        _result = str.split("\n");
        int count = _result.size() - 1;
        QRegularExpression  re( "Microsoft");

        for (int i = count; i > 0; --i) {
            if(!_result[i].isEmpty()){
                QString tmp = _result[i];                
                if(re.match(tmp).hasMatch()){
                    continue;
                }else if(tmp.right(1)== ">"){
                    continue;
                }else if(tmp == ""){
                    continue;
                }else{
                    QString _res = tmp.replace("\r", "").trimmed();
                    emit endParse(_res, command);
                    m_process.setReadChannel(QProcess::StandardOutput);
                    break;
                }
            }
        }
    }else if(command == wmicGetSID){
        QString str(result);
        str.replace("\r", "");
        _result = str.split("\n");
        int count = _result.size() - 1;
        QRegularExpression  re( "wmic");
        for (int i = count; i > 0; --i) {
            if(!_result[i].isEmpty()){
                QString tmp = _result[i];
                if(re.match(tmp).hasMatch()){
                    continue;
                }else if(tmp.right(1)== ">"){
                    continue;
                }else if(tmp == ""){
                    continue;
                }else{
                    if(tmp.left(1) != "S")
                        continue;
                    QString _res = tmp.replace("\r", "").trimmed();
                    emit endParse(_res, command);
                    m_process.setReadChannel(QProcess::StandardOutput);
                    break;
                }
            }
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
//                    if(tmp.left(1) == "S")
//                        continue;
//                    QString _res = tmp.replace("\r", "").trimmed();
//                    //emit endParse(_res, command);
//                    //m_process.setReadChannel(QProcess::StandardOutput);
//                    currentEncoding = _res;
//                    break;
                    continue;
                }
            }
        }
    }
    return result;
}
