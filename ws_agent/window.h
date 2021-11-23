//
// Created by arcady on 07.11.2021.
//

#ifndef WS_SOLUTION_WINDOW_H
#define WS_SOLUTION_WINDOW_H

#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE

namespace Ui {
class Window;
}
//! [0]
class Window : public QDialog
{
Q_OBJECT

public:
    Window(QWidget *parent = nullptr);

    void setVisible(bool visible) override;



protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage();
    void messageClicked();

    void on_pushButton_clicked();

private:

    Ui::Window *ui;

    void setIcon();

    void createActions();
    void createTrayIcon();


    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};
//! [0]

#endif // QT_NO_SYSTEMTRAYICON
#endif //WS_SOLUTION_WINDOW_H
