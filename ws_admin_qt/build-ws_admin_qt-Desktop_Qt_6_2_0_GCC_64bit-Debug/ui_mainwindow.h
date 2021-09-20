/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action;
    QAction *mnuOptions;
    QAction *mnuObjectTree;
    QAction *action_4;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QToolButton *toolButton;
    QTreeView *treeView;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *mnuView;
    QMenu *mnuService;
    QMenu *menu_4;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(821, 605);
        action = new QAction(MainWindow);
        action->setObjectName(QString::fromUtf8("action"));
        mnuOptions = new QAction(MainWindow);
        mnuOptions->setObjectName(QString::fromUtf8("mnuOptions"));
        mnuObjectTree = new QAction(MainWindow);
        mnuObjectTree->setObjectName(QString::fromUtf8("mnuObjectTree"));
        mnuObjectTree->setCheckable(true);
        mnuObjectTree->setChecked(true);
        action_4 = new QAction(MainWindow);
        action_4->setObjectName(QString::fromUtf8("action_4"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolButton = new QToolButton(centralwidget);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        verticalLayout->addWidget(toolButton);

        treeView = new QTreeView(centralwidget);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        verticalLayout->addWidget(treeView);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 821, 22));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        mnuView = new QMenu(menubar);
        mnuView->setObjectName(QString::fromUtf8("mnuView"));
        mnuService = new QMenu(menubar);
        mnuService->setObjectName(QString::fromUtf8("mnuService"));
        menu_4 = new QMenu(menubar);
        menu_4->setObjectName(QString::fromUtf8("menu_4"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(mnuView->menuAction());
        menubar->addAction(mnuService->menuAction());
        menubar->addAction(menu_4->menuAction());
        menu->addAction(action_4);
        mnuService->addAction(mnuOptions);
        menu_4->addAction(action);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\320\232\320\276\320\275\321\201\320\276\320\273\321\214 \320\260\320\264\320\274\320\270\320\275\320\270\321\201\321\202\321\200\320\270\321\200\320\276\320\262\320\260\320\275\320\270\321\217", nullptr));
        action->setText(QCoreApplication::translate("MainWindow", "\320\236 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\265 ...", nullptr));
        mnuOptions->setText(QCoreApplication::translate("MainWindow", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213", nullptr));
        mnuObjectTree->setText(QCoreApplication::translate("MainWindow", "\320\224\320\265\321\200\320\265\320\262\320\276 \320\276\320\261\321\212\320\265\320\272\321\202\320\276\320\262", nullptr));
        action_4->setText(QCoreApplication::translate("MainWindow", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", nullptr));
        toolButton->setText(QCoreApplication::translate("MainWindow", "...", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\320\244\320\260\320\271\320\273", nullptr));
        mnuView->setTitle(QCoreApplication::translate("MainWindow", "\320\222\320\270\320\264", nullptr));
        mnuService->setTitle(QCoreApplication::translate("MainWindow", "\320\241\320\265\321\200\320\262\320\265\321\200", nullptr));
        menu_4->setTitle(QCoreApplication::translate("MainWindow", "\320\241\320\277\321\200\320\260\320\262\320\272\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
