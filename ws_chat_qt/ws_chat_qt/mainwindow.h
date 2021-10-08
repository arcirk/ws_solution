#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmlwebsocket.h"
#include "popup.h"
#include "appsettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_display_error(const QString& what, const QString& err);

private slots:
    void on_mnuSettings_triggered();

private:
    Ui::MainWindow *ui;
    QmlWebSocket * qClient;
    PopUp *popUp;
};
#endif // MAINWINDOW_H
