#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

#include "include/clientsettings.h"
#include "include/qmlwebsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_mnuExit_triggered();
    void on_mnuOptions_triggered();
    void on_mnuInstallService_triggered();
    void on_mnuStart_triggered();
    void on_mnuStop_triggered();
    void on_mnuStatusService_triggered();
    void on_mnuConnect_triggered();
    void on_mnuDisconnect_triggered();

    //webSocket
    void connectedStatusChanged(bool status);
    void displayError(const QString& what, const QString& err);



private:
    Ui::MainWindow *ui;
    ClientSettings settings;
    QLabel* lblStatusService;
    QLabel* lblStatusSocket;
    bWebSocket* client;
    bool isServiceStarted;

    QTreeWidget* treeServerObjects;
    QTableWidget * listChildServerObjects;
    QTreeWidget * treeChannelsObjects;

    //form
    void fillTree(bool started);
    QString serverView();
    void visibilityСontrol(bool visible, bool isSessions);
};

#endif // MAINWINDOW_H
