#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QObject>
#include <QProcess>
#include <QTextCodec>

static const QString program = "cmd";

class CommandLine : public QObject
{
    Q_OBJECT
public:
    explicit CommandLine(QObject *parent = nullptr);

    void setParams(QStringList& param);

    bool listening();

    enum cmdCommand{
        echoSystem = 0,
        echoUserName,
        wmicGetSID,
        echoGetEncoding,
        unknown
    };


    QString parseCommand(const QString& result, CommandLine::cmdCommand command);

public slots:
    void start();
    void stop();

    void send(const QString& commandText, CommandLine::cmdCommand command);

    void clearBuffer();

    void setCurrentEncoding(const QString& enc);
    QString currentEncoding();

    void setVerefyEncodingg(bool val);
    bool verefyEncoding();

    void setMethod(int val);
    int method();

signals:
    void output(const QString& data, CommandLine::cmdCommand command);
    void endParse(const QString& result, CommandLine::cmdCommand command);
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
    cmdCommand _command;
    QString _currentEncoding;
    bool _verefyEncoding;
    int _method;

    QString encodeData(const QByteArray& data, int m);
};

#endif // COMMANDLINE_H
