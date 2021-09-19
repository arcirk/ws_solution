#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QTreeView>
#include <QListView>
#include "optionsdlg.h"
#include "AppSettings.h"

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
    void on_mnuOptions_triggered();

    void on_mnuObjectTree_triggered();

    //void loadTree();
private:
    Ui::MainWindow *ui;
    QTreeWidget* treeServerObjects;
    QListWidget * listServerObjects;

    OptionsDlg * optDlg;

    appSettings * settings;

    QDockWidget *docObjectTree;

};
#endif // MAINWINDOW_H
