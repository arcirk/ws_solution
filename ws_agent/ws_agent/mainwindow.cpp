#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QAction>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextEdit>

#include <clientsettings.h>

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

    exitEvent = false;

    createActions();
    createTrayIcon();

    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::messageClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    trayIcon->show();

    m_client = new bWebSocket(this, "conf_qt_agent.json");
    m_client->options()[bConfFieldsWrapper::AppName] = "qt_agent";
    if(m_client->options()[bConfFieldsWrapper::User].toString().isEmpty())
        m_client->options()[bConfFieldsWrapper::User] = "admin";
    ui->txtServerHost->setText(m_client->options()[bConfFieldsWrapper::ServerHost].toString());
    ui->iServerPort->setValue(m_client->options()[bConfFieldsWrapper::ServerPort].toInt());
    ui->txtUserName->setText(m_client->getUserName());
    ui->chAutiConnect->setChecked(m_client->autoConnect());
    ui->chSaveAuth->setChecked(m_client->saveHash());
    ui->txtPassword->setEnabled(!m_client->saveHash());
    ui->btnViewPwd->setEnabled(ui->txtPassword->isEnabled());
    ui->pathToClient->setText(m_client_app.options()[bConfFieldsWrapper::ClientWorkingDirectory].toString());
    ui->lineDavDirectory->setText(m_client->options()[bConfFieldsWrapper::LocalWebDavDirectory].toString());
    ui->chUseLocalDirectory->setChecked(m_client->options()[bConfFieldsWrapper::UseLocalWebDavDirectory].toBool());

    connect(m_client, &bWebSocket::connectionSuccess, this, &MainWindow::onConnectionSuccess);
    connect(m_client, &bWebSocket::closeConnection, this, &MainWindow::onCloseConnection);
    connect(m_client, &bWebSocket::qmlError, this, &MainWindow::onQmlError);
    connect(m_client, &bWebSocket::connectedStatusChanged, this, &MainWindow::onConnectedStatusChanged);
    connect(m_client, &bWebSocket::clientJoin, this, &MainWindow::onClientJoin);
    connect(m_client, &bWebSocket::clientLeave, this, &MainWindow::onClientLeave);
    connect(m_client, &bWebSocket::displayError, this, &MainWindow::onDisplayError);
    connect(m_client, &bWebSocket::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_client, &bWebSocket::hiddenChanged, this, &MainWindow::onHiddenChanged);

    this->setWindowFlags(Qt::Dialog);

    QString exeName = "ws_client_qml";

#ifdef _WINDOWS
    exeName = "ws_client_qml.exe";
#endif

    QString clientFileName = QDir::fromNativeSeparators(m_client_app.options()[bConfFieldsWrapper::ClientWorkingDirectory].toString() + QDir::separator() + exeName);

    if(QFileInfo::exists(clientFileName)){
        m_client_app.setAppPath(clientFileName);
    }
    else{
        qDebug() << "ws_client_qml не найден";
    }

    m_client->setIsAgent(true);
    if(m_client->options()[bConfFieldsWrapper::AutoConnect].toBool()){
        m_client->open(m_client->getUserName(), "");
    }
}

MainWindow::~MainWindow()
{
//    if(wsProc->state() == QProcess::Running){
//        wsProc->close();
//    }
    delete m_client;
    delete ui;
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openQmlClient);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openConnect);
    trayIconMenu->addAction(closeConnect);
    trayIconMenu->addSeparator();
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
    openQmlClient->setEnabled(m_client->isStarted());
    openConnect->setEnabled(!m_client->isStarted());
    closeConnect->setEnabled(m_client->isStarted());
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
    qDebug() << exitEvent;

    if (trayIcon->isVisible() && !exitEvent) {
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

//    maximizeAction = new QAction(tr("Ma&ximize"), this);
//    connect(maximizeAction, &QAction::triggered, this, &QWidget::showMaximized);

    restoreAction = new QAction(tr("&Открыть настройки"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Выйти"), this);
    //connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::appExit);

    closeConnect = new QAction(tr("Отключится"), this);
    connect(closeConnect, &QAction::triggered, this, &MainWindow::onCloseConnect);

    openConnect = new QAction(tr("Подключиться"), this);
    connect(openConnect, &QAction::triggered, this, &MainWindow::onOpenConnect);

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
    m_client->options()[bConfFieldsWrapper::ServerPort] = ui->iServerPort->value();
    m_client->options().save();
}


void MainWindow::on_txtUserName_editingFinished()
{
    m_client->options()[bConfFieldsWrapper::User] = ui->txtUserName->text();
    m_client->options().save();
}


void MainWindow::on_txtPassword_editingFinished()
{
    //
}


void MainWindow::on_chSaveAuth_toggled(bool checked)
{
    m_client->setSaveHash(checked);
    m_client->options().save();
}


void MainWindow::on_chAutiConnect_toggled(bool checked)
{
    m_client->setAutoConnect(checked);
    m_client->options().save();
}


void MainWindow::on_btnConnect_clicked()
{
//    if(m_client->isStarted()){
//        return;
//    }
//
//    bool pwd_edit = ui->btnEditPwd->isChecked();
//    m_client->setPwdEdit(pwd_edit);
//
//    m_client->open(ui->txtUserName->text(), ui->txtPassword->text());

    onOpenConnect();
}


void MainWindow::on_btnDisconnect_clicked()
{

//    if(m_client->isStarted()){
//        m_client->close();
//    }
////    if(wsProc->state() == QProcess::Running){
////       wsProc->close();
////    }
    onCloseConnect();
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
    m_client->options()[bConfFieldsWrapper::ServerHost] = ui->txtServerHost->text();
    m_client->options().save();
}

void MainWindow::onConnectionSuccess()
{
    //qDebug() << "connectionSuccess";

    ui->txtPassword->setEnabled(false);
    ui->btnViewPwd->setEnabled(false);
    openConnect->setEnabled(false);
    closeConnect->setEnabled(true);
}

void MainWindow::onCloseConnection()
{
    openConnect->setEnabled(true);
    closeConnect->setEnabled(false);
}

void MainWindow::onQmlError(const QString &what, const QString &err)
{
    qDebug() << "onQmlError" << what << ": " << err;
    onDisplayError(what, err);
}

void MainWindow::onConnectedStatusChanged(bool status)
{
    //qDebug() << "onConnectedStatusChanged" << status;
    ui->btnConnect->setEnabled(!status);
    ui->btnDisconnect->setEnabled(status);
}

void MainWindow::onClientJoin(const QString &resp)
{
    qDebug() << "MainWindow::onClientJoin";
}

void MainWindow::onClientLeave(const QString &resp)
{
    qDebug() << "MainWindow::onClientLeave";
}

void MainWindow::openChatApp()
{
    if (m_client->isStarted()){
        QString exeName = "ws_client_qml";
#ifdef _WINDOWS
        exeName = "ws_client_qml.exe";
#endif
        QString pathToClient = QDir::fromNativeSeparators(m_client_app.options()[bConfFieldsWrapper::ClientWorkingDirectory].toString() + QDir::separator() + exeName);
        qDebug() << pathToClient;
        if(fileExists(pathToClient)){
            if(!m_client_app.isStarted()){
                QStringList m_arg;
                m_arg.append(m_client->getUserName());
                //qDebug() << m_client->getUserName();
                m_arg.append(m_client->getHash());
                //qDebug() << m_client->getHash();
                m_arg.append(m_client->getUuidSession());
                //qDebug() << m_client->getUuidSession();
                m_arg.append(m_client->options()[bConfFieldsWrapper::ServerHost].toString());
                //qDebug() << m_client->get_settings()->ServerHost;
                m_arg.append(QString::number(m_client->options()[bConfFieldsWrapper::ServerPort].toInt()));
                m_client_app.setParams(m_arg);
                m_client_app.setAppPath(pathToClient);
                m_client_app.start();
            }else
            {
                if(m_client->isStarted()){
                    m_client->agentClientShow();
                }
            }
        }else
            onDisplayError("Ошибка", "Файл клиента не найден!");
    }else
        onDisplayError("Ошибка", "Клиент не подключен!");

}


void MainWindow::on_pathToClient_editingFinished()
{
    m_client_app.options()[bConfFieldsWrapper::ClientWorkingDirectory] = ui->pathToClient->text();
    m_client_app.options().save();
}

void MainWindow::onDisplayError(const QString& what, const QString& err)
{
    QIcon msgIcon(":/img/images/381599_error_icon.svg");
    trayIcon->showMessage(what, err, msgIcon,
                          3 * 1000);
}

void MainWindow::output(QString data) {

}


void MainWindow::appExit() {

    if(m_client_app.isStarted())
        m_client_app.stop();

    QCoreApplication::quit();

}

void MainWindow::onCloseConnect() {
    if (m_client->isStarted()) {
        m_client->close();
    }
    if (m_client_app.isStarted())
        m_client_app.stop();

}

void MainWindow::onOpenConnect() {

    if(m_client->isStarted()){
        return;
    }

    bool pwd_edit = ui->btnEditPwd->isChecked();
    m_client->setPwdEdit(pwd_edit);

    m_client->open(ui->txtUserName->text(), ui->txtPassword->text());


}


void MainWindow::on_chUseLocalDirectory_toggled(bool checked)
{
    m_client->options()[bConfFieldsWrapper::UseLocalWebDavDirectory] = checked;
    m_client->options().save();;
}


void MainWindow::on_lineDavDirectory_editingFinished()
{
    m_client->options()[bConfFieldsWrapper::LocalWebDavDirectory] = ui->lineDavDirectory->text();
    m_client->options().save();
}


void MainWindow::on_btnSelClientFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->pathToClient->setText(dir);
    }
}


void MainWindow::on_btnSelDavFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir != ""){
        ui->lineDavDirectory->setText(dir);
    }
}


void MainWindow::on_btnExit_clicked()
{
    auto result = QMessageBox::question(this, tr("Выход"), tr("Выйти из программы?"));

    if(result == QMessageBox::Yes){
        if(m_client_app.isStarted())
            m_client_app.stop();

        exitEvent = true;

        QCoreApplication::quit();
    }

}

void MainWindow::onMessageReceived(const QString &msg, const QString &uuid, const QString &recipient,
                                   const QString &recipientName) {

    QString message = QString::fromStdString(IClient::base64_decode(msg.toStdString()));

    QJsonObject _msg = QJsonDocument::fromJson(message.toUtf8()).object();

    QJsonArray rows = _msg.value("rows").toArray();

    if(rows.size() > 0){
        QJsonObject obj = rows[0].toObject();
        QString _base64 = obj.value("message").toString();
        QString _message = QString::fromStdString(IClient::base64_decode(_base64.toStdString()));
        QJsonObject objMsg = QJsonDocument::fromJson(_message.toUtf8()).object();
        QString r = QString::fromStdString(IClient::base64_decode(objMsg.value("message").toString().toStdString()));

        QTextEdit * edt = new QTextEdit(this);
        edt->setHtml(r);
        QIcon msgIcon(":/img/images/message.png");
        trayIcon->showMessage(recipientName, edt->toPlainText(), msgIcon,
                              3 * 1000);
//        qDebug()<< qPrintable(r);
//        QTextEdit * edt = new QTextEdit(this);
//        edt->setHtml(r);
//        qDebug()<< qPrintable(edt->toPlainText());
        //std::cout << r.toStdString() <<std::endl;
    }

}

void MainWindow::onHiddenChanged(bool value) {
    if(m_client_app.isStarted())
        m_client_app.setClientHidden(value);

    qDebug() << "MainWindow::onHiddenChanged: " << value;
}

