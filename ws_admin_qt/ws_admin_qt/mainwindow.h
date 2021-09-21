#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QTreeView>
#include <QListView>
#include "optionsdlg.h"
#include "AppSettings.h"

#include "iws_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ext_message(const std::string& msg);

private slots:
    void on_mnuOptions_triggered();

    void on_mnuStartSession_triggered();

    void on_action_4_triggered();

private:

    enum treeItemType{itTopItem=1001,itGroupItem,itImageItem};

    Ui::MainWindow *ui;
    QTreeWidget* treeServerObjects;
    QListWidget * listServerObjects;

    OptionsDlg * optDlg;

    appSettings * settings;

    QDockWidget *docObjectTree;

    void fillTree();
    QString serverView();
    void processServeResponse(const std::string& response);

    IClient * client;

    QTreeWidgetItem * findTreeItem(const QString& source);
};
#endif // MAINWINDOW_H
