//
// Created by arcady on 14.12.2021.
//
#include "include/clientapp.h"
#include <QDebug>
#include <utility>
#include <QProcess>

ClientApp::ClientApp(QObject *parent) : QObject(parent)
{

    connect(&m_process, &QProcess::errorOccurred, this, &ClientApp::errorOccured);
    connect(&m_process, &QProcess::readyReadStandardError, this, &ClientApp::readyReadStandardError);
    connect(&m_process, &QProcess::readyReadStandardOutput, this, &ClientApp::readyReadStandardOutput);
    connect(&m_process, &QProcess::started, this, &ClientApp::start);
    connect(&m_process, &QProcess::stateChanged, this, &ClientApp::stateChanged);
    connect(&m_process, &QProcess::readyRead, this, &ClientApp::readyRead);

//    //
    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ClientApp::finished);

    m_listening = false;

}

void ClientApp::errorOccured(QProcess::ProcessError error) {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO << error;
    emit output("Error");
}

void ClientApp::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    emit output("Complete");
}

void ClientApp::readyReadStandardError() {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardError();
    QString message = "Standard Error: ";
    message.append(m_process.readAllStandardError());
    emit output(message);
}

void ClientApp::readyReadStandardOutput() {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAllStandardOutput();
    emit output(QString(data.trimmed()));
}

void ClientApp::started() {
    qInfo() << Q_FUNC_INFO;
}

void ClientApp::stateChanged(QProcess::ProcessState newState) {

    qInfo() << Q_FUNC_INFO;
    switch (newState) {
        case QProcess::NotRunning:
            emit output("Not running");
            break;
        case QProcess::Starting:
            emit output("Starting ...");
            break;
        case QProcess::Running:
            emit output("Running");
            openClientApp();
            break;
    }
}

void ClientApp::readyRead() {
    if(!m_listening) return;
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_process.readAll().trimmed();
    qInfo() << data;
    emit output(data);
}

void ClientApp::openClientApp() {

}

QString ClientApp::operatingSystem() {
    return QSysInfo::prettyProductName();
}

void ClientApp::start() {

    qInfo() << Q_FUNC_INFO;
    if (app_path.isEmpty())
        return;
    if (params.isEmpty())
        return;
    m_listening = true;
    m_process.start(app_path, params);
}

void ClientApp::stop() {
    qInfo() << Q_FUNC_INFO;
    m_listening = false;
    m_process.close();
}

void ClientApp::setParams(QStringList& param) {

    params = std::move(param);

}

void ClientApp::setAppPath(const QString &appPath) {
    app_path = appPath;
}

//void ClientApp::output(QString data) {
//
//}

bool ClientApp::isStarted() {
    return m_process.state() == QProcess::Running;
}
