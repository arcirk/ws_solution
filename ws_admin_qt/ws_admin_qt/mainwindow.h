#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "stdfx.h"
#include <QMainWindow>
#include <QtWidgets>
#include <QTreeView>
#include <QListView>
#include "optionsdlg.h"
#include "AppSettings.h"

//#include "iws_client.h"
#include "../../ws_client/include/iws_client.h"

#include "popup.h"

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

    void start();

private slots:
    void on_mnuOptions_triggered();

    void on_mnuStartSession_triggered();

    void on_action_4_triggered();

    void on_mnuDisconnect_triggered();

private:

    enum treeItemType{itTopItem=1001,itGroupItem,itImageItem};

    Ui::MainWindow *ui;
    QTreeWidget* treeServerObjects;
    QListWidget * listServerObjects;

    OptionsDlg * optDlg;

    appSettings * settings;

    QDockWidget *docObjectTree;

    void fillTree();
    void update_branch(const QString& branch_name, const QString& serverResp);
    QString serverView();
    void processServeResponse(const std::string& response);

    IClient * client;

    QTreeWidgetItem * findTreeItem(const QString& source, int col = 0);

    PopUp *popUp;       // Объявляем объект всплывающего сообщения

};
#endif // MAINWINDOW_H
