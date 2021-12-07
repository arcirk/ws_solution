#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "include/qmlwebsocket.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QAction;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

    void on_iServerPort_editingFinished();

    void on_txtUserName_editingFinished();

    void on_txtPassword_editingFinished();

    void on_chSaveAuth_toggled(bool checked);

    void on_chAutiConnect_toggled(bool checked);

    void on_btnConnect_clicked();

    void on_btnDisconnect_clicked();

    void on_btnHide_clicked();

    void on_btnViewPwd_toggled(bool checked);

    void on_btnEditPwd_toggled(bool checked);

    void on_txtServerHost_editingFinished();

    //ws_client
    void onConnectionSuccess();
    void onCloseConnection();
    void onQmlError(const QString& what, const QString& err);
    void onConnectedStatusChanged(bool satus);
    void onClientJoin(const QString& resp);
    void onClientLeave(const QString& resp);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    bWebSocket* m_client;

    void createTrayIcon();
    void createActions();
    void setIcon();
};
#endif // MAINWINDOW_H
