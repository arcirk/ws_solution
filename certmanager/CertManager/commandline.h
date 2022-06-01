#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QObject>
#include <QProcess>
#include <QTextCodec>

#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <wchar.h>
//#include <string>
//#include <process.h>
//#include <windows.h>

#ifdef _WINDOWS
static const QString program = "cmd";
#else
static const QString program = "/bin/bash";
#endif

enum cmdCommand{
    echoSystem = 0,
    echoUserName,
    wmicGetSID,
    echoGetEncoding,
    unknown
};

class CommandLine : public QObject
{
    Q_OBJECT
public:
    explicit CommandLine(QObject *parent = nullptr, bool usesysstem = false, const QString& enc = "CP866");

    void setParams(QStringList& param);

    bool listening();



    QString parseCommand(const QString& result, int command);

public slots:
    void start();
    void stop();

    void send(const QString& commandText, int);

    void clearBuffer();

    void setCurrentEncoding(const QString& enc);
    QString currentEncoding();

    void setVerefyEncodingg(bool val);
    bool verefyEncoding();

    void setMethod(int val);
    int method();

    bool useSystem();
    void setUseSystem(bool val);
    void setChcp();

signals:
    void output(const QString& data, int command);
    void endParse(const QString& result, int command);
private slots:
    void errorOccured(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void stateChanged(QProcess::ProcessState newState);
    void readyRead();

private:
    QProcess m_process;
    QStringList params;
    bool m_listening;
    QTextCodec *codec;
    QString _lastCommand;
    int _command;
    QString _currentEncoding;
    bool _verefyEncoding;
    int _method;
    bool _useSystem;

    QString encodeData(const QByteArray& data, int m);
    std::string executeSystem(const std::string& cmd);

};

#endif // COMMANDLINE_H
