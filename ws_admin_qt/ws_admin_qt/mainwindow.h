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
    void on_display_error(const QString& what, const QString& err);
    void on_display_notify(const QString& msg);
    void fillTree();
    void on_fill_node(const QString& command, const QString& resp);
    void on_treeSrvObjects_itemSelectionChanged();
    void on_fill_group_tree(const QString& resp);

private:
    enum treeItemType{itTopItem=1001,itGroupItem,itImageItem};

    Ui::MainWindow *ui;

    QTreeWidget* treeServerObjects;
    QTableWidget * listChildServerObjects;
    QTreeWidget * treeChannelsObjects;
    OptionsDlg * optDlg;
    appSettings * settings;
    QDockWidget *docObjectTree;
    PopUp *popUp;       // Объявляем объект всплывающего сообщения
    IClient * client;

    void update_branch(const QString& branch_name, const QString& serverResp);
    QString serverView();
    void processServeResponse(const std::string& response);

    void resizeColumns();
    void initItemList();
    void initTreeObjectsList();

    QTreeWidgetItem * findTreeItem(const QString& source, int col = 0);

    void fillList(const QString& nodeName);
    void load_group_tree(QTreeWidgetItem* currentItem, QTableWidget* initialData);
    QTreeWidgetItem * new_tree_item(const QString& text, const QString& toolType = "", const QString& nextColVal = "");

    void fill_property_values(QJsonObject* jsonObject, QTreeWidgetItem* item);

    signals:
    void display_error(const QString& what, const QString& err);
    void display_notify(const QString& msg);
    void fill_tree_();
    void fill_node(const QString& command, const QString& resp);
    void fill_group_tree(const QString& resp);

};
#endif // MAINWINDOW_H
