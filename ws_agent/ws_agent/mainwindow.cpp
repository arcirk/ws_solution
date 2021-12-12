#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>

#include <QProcess>

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    trayIcon->show();

    m_client = new bWebSocket();
    m_client->get_settings()->AppName = "qt_agent";
    if(m_client->get_settings()->RootUser.isEmpty())
        m_client->get_settings()->RootUser = "admin";
    ui->txtServerHost->setText(m_client->get_settings()->ServerHost);
    ui->iServerPort->setValue(m_client->get_settings()->ServerPort);
    ui->txtUserName->setText(m_client->getUserName());
    ui->chAutiConnect->setChecked(m_client->autoConnect());
    ui->chSaveAuth->setChecked(m_client->saveHash());
    ui->txtPassword->setEnabled(!m_client->saveHash());
    ui->btnViewPwd->setEnabled(ui->txtPassword->isEnabled());
    ui->pathToClient->setText(m_client->get_settings()->pathToClient);

    connect(m_client, &bWebSocket::connectionSuccess, this, &MainWindow::onConnectionSuccess);
    connect(m_client, &bWebSocket::closeConnection, this, &MainWindow::onCloseConnection);
    connect(m_client, &bWebSocket::qmlError, this, &MainWindow::onQmlError);
    connect(m_client, &bWebSocket::connectedStatusChanged, this, &MainWindow::onConnectedStatusChanged);
    connect(m_client, &bWebSocket::clientJoin, this, &MainWindow::onClientJoin);
    connect(m_client, &bWebSocket::clientLeave, this, &MainWindow::onClientLeave);

    this->setWindowFlags(Qt::Dialog);

    if(QFileInfo(m_client->get_settings()->pathToClient + "ws_gclient").exists()){
        qDebug() << "ws_gclient найден";
    }
    else{
        qDebug() << "ws_gclient не найден";
    }
}

MainWindow::~MainWindow()
{
    delete m_client;
    delete ui;
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openQmlClient);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    setIcon();

}

void MainWindow::setIcon()
{
    QIcon icon = QIcon(":/img/icons8-connected-people-48.png"); //iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip("Чат стартер");
}

void MainWindow::setVisible(bool visible)
{
    openQmlClient->setEnabled(true);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Чат"),
                                 tr("Программа продолжит работу в фоновом режиме "
                                    "Выберете <b>Выход</b> в контекстном меню системного трея"
                                    " для выхода."));
        hide();
        event->ignore();
    }
}

void MainWindow::createActions()
{
    openQmlClient = new QAction(tr("Открыть &чат"), this);
    connect(openQmlClient, &QAction::triggered, this, &MainWindow::openChatApp);

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Открыть настройки"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Выйти"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        //iconComboBox->setCurrentIndex((iconComboBox->currentIndex() + 1) % iconComboBox->count());
        break;
    case QSystemTrayIcon::MiddleClick:
        showMessage();
        break;
    default:
        ;
    }
}

void MainWindow::showMessage()
{
//    showIconCheckBox->setChecked(true);
//    int selectedIcon = typeComboBox->itemData(typeComboBox->currentIndex()).toInt();
//    QSystemTrayIcon::MessageIcon msgIcon = QSystemTrayIcon::MessageIcon(selectedIcon);
//
//    if (selectedIcon == -1) { // custom icon
//        QIcon icon(iconComboBox->itemIcon(iconComboBox->currentIndex()));
//        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), icon,
//                              durationSpinBox->value() * 1000);
//    } else {
//        trayIcon->showMessage(titleEdit->text(), bodyEdit->toPlainText(), msgIcon,
//                              durationSpinBox->value() * 1000);
    //    }
}

void MainWindow::messageClicked()
{
    QMessageBox::information(nullptr, tr("Чат"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}


void MainWindow::on_iServerPort_editingFinished()
{
    m_client->get_settings()->ServerPort = ui->iServerPort->value();
    m_client->get_settings()->save_settings();
}


void MainWindow::on_txtUserName_editingFinished()
{
    m_client->get_settings()->RootUser = ui->txtUserName->text();
    m_client->get_settings()->save_settings();
}


void MainWindow::on_txtPassword_editingFinished()
{
    //
}


void MainWindow::on_chSaveAuth_toggled(bool checked)
{
    m_client->setSaveHash(checked);
    m_client->get_settings()->save_settings();
}


void MainWindow::on_chAutiConnect_toggled(bool checked)
{
    m_client->setAutoConnect(checked);
    m_client->get_settings()->save_settings();
}


void MainWindow::on_btnConnect_clicked()
{
    if(m_client->isStarted()){
        return;
    }

    m_client->open(ui->txtUserName->text(), ui->txtPassword->text());

}


void MainWindow::on_btnDisconnect_clicked()
{

    if(m_client->isStarted()){
        m_client->close();
    }

}


void MainWindow::on_btnHide_clicked()
{
    if (trayIcon->isVisible()) {
        hide();
    }
}


void MainWindow::on_btnViewPwd_toggled(bool checked)
{
    if(checked){
        ui->btnViewPwd->setIcon(QIcon(QStringLiteral(":/img/images/view_hide_icon_124813.svg")));
        ui->txtPassword->setEchoMode(QLineEdit::Normal);
    }else{
        ui->btnViewPwd->setIcon(QIcon(QStringLiteral(":/img/images/view_show_icon_124811.svg")));
        ui->txtPassword->setEchoMode(QLineEdit::Password);
    }

}


void MainWindow::on_btnEditPwd_toggled(bool checked)
{
    ui->txtPassword->setEnabled(checked);
    ui->btnViewPwd->setEnabled(checked);
}


void MainWindow::on_txtServerHost_editingFinished()
{
    m_client->get_settings()->ServerHost = ui->txtServerHost->text();
    m_client->get_settings()->save_settings();
}

void MainWindow::onConnectionSuccess()
{
    qDebug() << "connectionSuccess";
}

void MainWindow::onCloseConnection()
{
    qDebug() << "onCloseConnection";
}

void MainWindow::onQmlError(const QString &what, const QString &err)
{
    qDebug() << "onQmlError" << what << ": " << err;
    onDisplayError(err);
}

void MainWindow::onConnectedStatusChanged(bool status)
{
    qDebug() << "onConnectedStatusChanged" << status;
    ui->btnConnect->setEnabled(!status);
    ui->btnDisconnect->setEnabled(status);
}

void MainWindow::onClientJoin(const QString &resp)
{
    qDebug() << "onClientJoin" << resp;
}

void MainWindow::onClientLeave(const QString &resp)
{
    qDebug() << "onClientLeave" << resp;
}

void MainWindow::openChatApp()
{
    qDebug() << m_client->get_settings()->pathToClient;
    if (m_client->isStarted()){

        QString pathToClient = m_client->get_settings()->pathToClient + "/ws_gclient";
        if(fileExists(pathToClient)){
            QProcess chat;
            chat.start(pathToClient);
            chat.waitForFinished(-1);
        }else
            onDisplayError("Файл клиента не найден!");
    }else
        onDisplayError("Клиент не подключен!");

}


void MainWindow::on_pathToClient_editingFinished()
{
    m_client->get_settings()->pathToClient = ui->pathToClient->text();
    m_client->get_settings()->save_settings();
}

void MainWindow::onDisplayError(const QString &err)
{
    QIcon msgIcon(":/img/images/381599_error_icon.svg");
    trayIcon->showMessage("Ошибка", err, msgIcon,
                          3 * 1000);
}

