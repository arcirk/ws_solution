//
// Created by arcady on 14.12.2021.
//

#ifndef WS_SOLUTION_CLIENTAPP_H
#define WS_SOLUTION_CLIENTAPP_H

#include <QThread>

class QProcess;

class ClientApp : public QThread
{
Q_OBJECT

public:
    void run();

public slots:
    void output();

signals:

private:
    QProcess *process;

};

#endif //WS_SOLUTION_CLIENTAPP_H
