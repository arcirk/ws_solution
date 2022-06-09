#ifndef DIALOGMAINWINDOW_H
#define DIALOGMAINWINDOW_H

#include <QDialog>
#include <userprofile.h>
#include <QMap>
#include <QUuid>
#include <QLineEdit>
#include "profilemanager.h"
#include <QTableWidgetItem>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QCloseEvent>
#include <QProcess>
#include <QDir>

namespace Ui {
class DialogMainWindow;
}

class DialogMainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMainWindow(QWidget *parent = nullptr);
    ~DialogMainWindow();

    void accept() override;
    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnAdd_clicked();

    void onSelectProfile();
    void onSelectDefaultAddress();
    void onSelectDefaultCertificate();

    void on_btnEdit_clicked();

    void on_btnDelete_clicked();

    void onTrayTriggered();
    void onAppExit();
    void onWindowShow();


public slots:
    void onLineEditCursorPositionChanged ( int oldPos , int newPos );
private:
    Ui::DialogMainWindow *ui;
    ProfileManager * _profiles;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *quitAction;
    QAction *showAction;
    QProcess * mozillaApp;
    QDir appHome;
    QString dirName;
    const QString _bankClientFile = "sslgate.url";
    const QString _bakClientUsbKey = "BankUsbKey";

    void updateData();
    void updateFromData();
    void setRow(UserProfile * prof);
    void updateRow(UserProfile * prof, int row);

    void setProfile(UserProfile* prof);

    QLineEdit *getLineEdit(int row, int col);
    QWidget *getItemWidget(const QString& text, int row, int col, const char* member);

    void createTrayActions();
    void createTrayIcon();
    void createDynamicMenu();

    QString toBankClientFile();

};

#endif // DIALOGMAINWINDOW_H
