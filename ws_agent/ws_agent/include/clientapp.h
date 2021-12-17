//
// Created by arcady on 14.12.2021.
//

#ifndef WS_SOLUTION_CLIENTAPP_H
#define WS_SOLUTION_CLIENTAPP_H

#include <QObject>
#include <QProcess>

class ClientApp : public QObject
{
Q_OBJECT

public:
    explicit ClientApp(QObject *parent = nullptr);

    QString operatingSystem();
    void setParams(QStringList& param);
    void setAppPath(const QString& appPath);
    bool isStarted();

public slots:
    void start();
    void stop();

signals:
    void output(QString data);

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
    QString app_path;

//    //bool running;
//    QString m_address;
    bool m_listening;

    void openClientApp();
};

#endif //WS_SOLUTION_CLIENTAPP_H
