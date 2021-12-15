//
// Created by arcady on 14.12.2021.
//
#include "include/clientapp.h"
#include "mainwindow.h"
#include <QProcess>
#include <QDebug>

void ClientApp::run()
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(output()));
    process.start("uname");
    exec();
}
