#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnection.h"
#include <QMessageBox>
#include <QSqlError>
//#include "registry.h"
#include <QSqlQuery>
#include <QFileDialog>
#include "dialogselectinlist.h"
#include <QPixmap>
#include "dialogoptions.h"
#include <clientsettings.h>
#include <QSqlQueryModel>
#include <dialogcomputer.h>
#include <QSqlError>
#include <qjsontablemodel.h>
#include <QStringConverter>
#include <QInputDialog>
#include "dialogterminaloptions.h"
#include <QTextOption>
#include "dialogselectfromdatabase.h"
#include "serveresponse.h"
#include <QStorageInfo>
#include <sqlqueryinterface.h>
#include <sqlinterface.h>
#include <QScrollBar>
#include "tabledelegate.h"
#include "dialogcontainername.h"
#include "dialogcontainerinfo.h"
#include <QJsonArray>
#include "certificate.h"
#include "converter.h"
#include <QQueue>

#include <sstream>

#ifdef _WINDOWS
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
    #pragma warning(disable:4100)
#endif


const static QString Cyrillic = "йцукенгшщзхъфывапролджэячсмитьё"
        "ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁ";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << __FUNCTION__;

    //http://pushorigin.ru/cryptopro/cryptcp

    //список контейнеров
    //csptest.exe -keyset -enum_cont -verifycontext -fqcn
    //Копирование контейнера
    //csptest.exe -keycopy -src «Имя исходного контейнера» -pinsrc=пароль -dest «Имя конечного контейнера» -pindest=пароль
//    -src — имя контейнера. Имена контейнеров уникальны, поэтому можно не указывать путь к носителю, КриптоПро сама найдет путь к контейнеру.
//    -dest — имя скопированного контейнера, оно должно отличаться от исходного имени. Так же можно указать путь к контейнеру, например, если указать -dest "\\.\FAT12_H\2015ZAO_3", то контейнер будет скопирован на флэшку. Если не указать путь к носителю, а просто задать название контейнеру, то крипто про выведет графический диалог выбора носителя для копирования.
//    -pinsrc — пароль от исходного контейнера, если пинкода нет, то данный параметр можно не указывать.
//    -pindest — пароль на скопированный контейнер. Чтобы подавить графический диалог установки пароля при автоматическом копировании контейнеров, можно указать пустой пароль, выполнив -pindest=""
    //копирование рутокена в реестр
    //csptest.exe -keycopy -src 2015ZAO -pinsrc=12345678 -dest "\\.\REGISTRY\2015ZAO_3" -pindest=""

    //Установка сертификатов
    //csptest.exe -property -cinstall -cont "Имя контейнера"

//    Просмотр подробной информации о контейнере:
//    csptestf -keyset -container '\\.\HDIMAGE\test' -info


    //Просмотр личных сертификатов
    //certmgr -list -store uMy
    //Просмотр сертификатов в контейнере
    //certmgr -list -container '\\.\Aladdin R.D. JaCarta 00 00\8df47e71-18ae-49c1-8738-9b4b0944dcd4'
    //Просмотр промежуточных сертификатов
    //certmgr -list -store uca

//    Установка сертификатов в хранилище КриптоПро:
//    certmgr -inst -store uRoot -file <название-файла>.cer
//    Установка сертификатов в хранилище ПК:
//    certmgr -inst -store mRoot -file <название-файла>cer

//   Удалить сертификаты, установленные в хранилище КриптоПро:
//   certmgr -delete -store uRoot
//   Если установлено более одного сертификата, то будет предложено указать номер удаляемого сертификата.
//   Удалить все сертификаты, установленные в хранилище КриптоПро:
//   certmgr -delete -all -store uRoot

//    Установим пользовательский сертификат с привязкой к закрытому контейнеру \\.\HDIMAGE\bob
//    certmgr -inst -file cert-bob.cer -cont '\\.\HDIMAGE\bob'

//    Активируем хранилище HDIMAGE:
//    cpconfig -hardware reader -add HDIMAGE store

    //Экспортировать сертификат из локального хранилища в файл:
    //certmgr -export -dn 'CN=Ли Александр Сергеевич' -dest test.cer !!!

    //isFormLoaded = false;

    ui->setupUi(this);

    connect(this, &MainWindow::whenDataIsLoaded, this, &MainWindow::onWhenDataIsLoaded);
    connect(this, &MainWindow::endInitConnection, this, &MainWindow::onEndInitConnection);
    connect(this, &MainWindow::startGetCertUsersData, this, &MainWindow::onStartGetCertUsersData);

    ui->tableView->setItemDelegate(new TableDelegate);
    ui->tableView->setIconSize(QSize(16,16));

    createModels();

    formControl();

    initCsptest(); // init CriptoPro

    _sett = new Settings(this);

    createTerminal();

    createConnectionsObjects();

    setWsConnectedSignals();

    if(_sett->launch_mode() == mixed){
        if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
             connectToDatabase(_sett, _sett->pwd());
    }else
        connectToWsServer();

    setWindowTitle("Менеджер сертификатов");

}

void MainWindow::createModels(){

    createColumnAliases();

    modelSqlContainers = new QJsonTableModel(this);
    modelSqlContainers->setColumnAliases(m_colAliases);
    modelSqlContainers->setRowsIcon(QIcon(":/img/cont.png"));
    modelSqlContainers->setFormatColumn(2, "base64");

    modelSqlCertificates = new QJsonTableModel(this);
    modelSqlCertificates->setColumnAliases(m_colAliases);
    modelSqlCertificates->setRowsIcon(QIcon(":/img/rosette.ico"));

    modelWsUsers = new QJsonTableModel(this);
    modelWsUsers->setColumnAliases(m_colAliases);
    modelWsUsers->setRowsIcon(QIcon(":/img/user.png"));

    modelSqlUsers = new QJsonTableModel(this);
    modelSqlUsers->setColumnAliases(m_colAliases);
    modelSqlUsers->setRowsIcon(QIcon(":/img/user.png"));

    modelUserCertificates = new QJsonTableModel(this);
    modelUserCertificates->setRowsIcon(QIcon(":/img/rosette.ico"));
    modelUserCertificates->setColumnAliases(m_colAliases);

    modelUserContainers = new QJsonTableModel(this);
    modelUserContainers->setColumnAliases(m_colAliases);
    modelUserContainers->setRowsIcon(QIcon(":/img/cont.png"));
    proxyModelUserConteiners = new QProxyModel(this);
    proxyModelUserConteiners->setSourceModel(modelUserContainers);

}

void MainWindow::toolBarSetVisible(QWidget * bar, bool value){
    qDebug() << __FUNCTION__;
    bar->setVisible(value);
}

void MainWindow::createWsObject()
{
    qDebug() << __FUNCTION__;
    m_client = new bWebSocket(this, "conf_qt_cert_manager.json", currentUser->name());
//    m_client->options()[bConfFieldsWrapper::AppName] = "qt_cert_manager";
//    m_client->options()[bConfFieldsWrapper::User] = "admin";
//    QString hash = bWebSocket::generateHash("admin", "admin");
//    m_client->options()[bConfFieldsWrapper::Hash] = hash;
//    m_client->options().save();
}

void MainWindow::setWsConnectedSignals()
{
    qDebug() << __FUNCTION__;
    connect(m_client, &bWebSocket::connectionSuccess, this, &MainWindow::onConnectionSuccess);
    connect(m_client, &bWebSocket::closeConnection, this, &MainWindow::onCloseConnection);
    connect(m_client, &bWebSocket::connectedStatusChanged, this, &MainWindow::onConnectedStatusChanged);
    connect(m_client, &bWebSocket::clientJoinEx, this, &MainWindow::onClientJoinEx);
    connect(m_client, &bWebSocket::clientLeave, this, &MainWindow::onClientLeave);
    connect(m_client, &bWebSocket::displayError, this, &MainWindow::onDisplayError);
    connect(m_client, &bWebSocket::messageReceived, this, &MainWindow::onMessageReceived);
    connect(m_client, &bWebSocket::getActiveUsers, this, &MainWindow::onGetActiveUsers);
    connect(m_client, &bWebSocket::execQuery, this, &MainWindow::onWsExecQuery);
    connect(m_client, &bWebSocket::wsGetAvailableContainers, this, &MainWindow::onWsGetAvailableContainers);
    connect(m_client, &bWebSocket::wsCommandToClient, this, &MainWindow::onWsCommandToClient);
    connect(m_client, &bWebSocket::mplFormLoaded, this, &MainWindow::onWsMplClientFormLoaded);
}

void MainWindow::createTerminal()
{
    currentUser = new CertUser(this);
    QString curentHost = QSysInfo::machineHostName();
    currentUser->setDomain(curentHost);

    terminal = new CommandLine(this, false, _sett->charset());
    ui->txtTerminal->setWordWrapMode(QTextOption::NoWrap);
    QPalette pal = ui->txtTerminal->palette();
    pal.setBrush(QPalette::Base, Qt::black);
    ui->txtTerminal->setAutoFillBackground(true);
    ui->txtTerminal->setPalette(pal);
    ui->txtTerminal->setTextColor(QColor(0,255,0));
    terminal->setMethod(_sett->method());

    connect(terminal, &CommandLine::output, this, &MainWindow::onOutputCommandLine);
    connect(terminal, &CommandLine::endParse, this, &MainWindow::onParseCommand);
    connect(terminal, &CommandLine::error, this, &MainWindow::onCommandError);

    terminal->start();
    if(isUseCsptest)
        terminal->send(QString("cd \"%1\"\n").arg(_cprocsp_dir), CmdCommand::unknown);

#ifdef _WINDOWS
        std::string envUSER = "username";
        if(_sett->charset() != "CP866")
            terminal->setChcp();
       //terminal->send("echo %username%\n", CommandLine::cmdCommand::echoUserName);// ; exit\n

        QByteArray data(std::getenv(envUSER.c_str()));
        QString uname = QString::fromLocal8Bit(data);
        currentUser->setName(uname);

        //terminal->send(QString("WHOAMI /USER\n").arg(uname), cmdCommand::wmicGetSID);


#else
        std::string envUSER = "USER";
        QString cryptoProDir = "/opt/cprocsp/bin/amd64/";
        terminal->send("echo $USER\n", 1); //CommandLine::cmdCommand::echoUserName);// ; exit\n
#endif

}

void MainWindow::inVisibleToolBars()
{
    qDebug() << __FUNCTION__;
//    toolBar.append(ui->btnAdd);
//    toolBar.append(ui->btnEdit);
//    toolBar.append(ui->btnDelete);
//    toolBar.append(ui->btnImportFromDatabase);
//    toolBar.append(ui->btnInstallToUser);
//    toolBar.append(ui->btnToDatabase);
//    toolBarActiveUsers.append(ui->btnUserToDatabase);
//    toolBarActiveUsers.append(ui->btnCompToDatabase);
    toolBarSetVisible(ui->wToolBarAU, false);
    toolBarSetVisible(ui->wToolbarDatabase, false);
    toolBarSetVisible(ui->wToolBarCurrentUser, false);
    toolBarSetVisible(ui->wToolBarMain, false);
}

void MainWindow::setKeysToRegistry()
{
//    qDebug() << __FUNCTION__;
//    auto dlg = new DialogSelectDevice(this, {}, "База данных");
//    dlg->setModal(true);
//    dlg->exec();

//    if(dlg->result() == QDialog::Accepted){

//        if(dlg->currentSelection() == 0){

//            QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
//                                                         QDir::homePath(),
//                                                         QFileDialog::ShowDirsOnly
//                                                         | QFileDialog::DontResolveSymlinks);
//            if(dir != ""){
//                QFile file(dir + QDir::separator() + "name.key");
//                if(file.open(QIODevice::ReadOnly)){
//                    QString data = QString::fromLocal8Bit(file.readAll());
//                    //qDebug() << data;
//                    QRegularExpression ex("0/\u0016-");
//                    if(ex.match(data).hasMatch()){
//                        data.replace("0/\u0016-", "");
//                        auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));
//                        if(result == QMessageBox::Yes){
//                            QString error;
//                            bool result = srtmnr::Registry::importKeysFromLocalCatalog(currentUser, dir, data, error);
//                            if(result)
//                                QMessageBox::information(this, "Успех", "Контейнер успешно импортирован в реестр!");
//                            else{
//                                if(!error.isEmpty())
//                                    QMessageBox::critical(this, "Ошибка", "Ошибка импорта контейнера!");
//                            }
//                        }
//                    }else
//                       QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//                }else{
//                    QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//                }
//            }
//        }else if(dlg->currentSelection() == 1){

//        }else if(dlg->currentSelection() == 2){

//        }
//    }
}

bool MainWindow::isDbOpen()
{
    qDebug() << __FUNCTION__;
    QString status;

    bool result = db->isOpen();

    if(db->isOpen()){
        status = "SQL Server: " + _sett->server() + "  ";
    }

    if(m_client->isStarted()){
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
    }

    infoBar->setText(status);

    return result;
}

void MainWindow::formControl()
{
    qDebug() << __FUNCTION__;
    inVisibleToolBars();
    createTree();
    infoBar = new QLabel(this);
    ui->statusbar->addWidget(infoBar);
    infoBar->setText("Не подключен.");
}

void MainWindow::initCsptest()
{

    isUseCsptest = false;

#ifdef _WINDOWS
   _cprocsp_exe = "C:/Program Files (x86)/Crypto Pro/CSP/csptest.exe";
   _cprocsp_dir = "C:/Program Files (x86)/Crypto Pro/CSP/";
#else
    _cprocsp_exe = "/opt/cprocsp/bin/amd64/cprocsp";
    _cprocsp_dir = "/opt/cprocsp/bin/amd64/"
#endif

    QFile csptest(_cprocsp_exe);
    QString inf;
    if(!csptest.exists()){
        inf = "Ошибка: КриптоПро не найден в каталоге установки по умолчанию!";
        qCritical() << __FUNCTION__ << inf;

    }
    else{
        inf = "КриптоПро найден в каталоге установки по умолчанию.";
        //qDebug() << __FUNCTION__ << "Утилита csptest установлена на компьютере!";
        isUseCsptest = true;

    }

    ui->txtTerminal->setText(ui->txtTerminal->toPlainText() + inf + "\n");
    //https://redos.red-soft.ru/base/other-soft/szi/certs-cryptopro/

}

void MainWindow::csptestCurrentUserGetContainers(const QString &result)
{
    qDebug() << __FUNCTION__;

    if(result.isEmpty())
        return;

    if(!currentUser)
        return;

    QStringList keys = result.split("\n");
    currentUser->setContainers(keys);

    QString json = currentUser->modelContainersText();
    modelUserContainers->setJsonText(json);
    modelUserContainers->reset();
    resetInfoUserContainers();

    auto item = ui->treeWidget->currentItem();
    if(item)
        emit ui->treeWidget->itemClicked(item, 0);

    if(!currentRecipient.isEmpty()){
        sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n"), true);
        currentRecipient = "";
    }
}

QTreeWidgetItem *MainWindow::addTreeNode(const QString &text, const QVariant &key, const QString &imagePath)
{
    qDebug() << __FUNCTION__;
    auto * node = new QTreeWidgetItem();
    node-> setText (0, text);
    node->setData(0, Qt::UserRole, key);
    node->setIcon(0, QIcon(imagePath));
    return node;
}

QTreeWidgetItem *MainWindow::findTreeItem(const QString &key)
{
    auto tree = ui->treeWidget;
    auto root = tree->topLevelItem(0);
    return findTreeItem(key, root);
}

QTreeWidgetItem * MainWindow::findTreeItem(const QString& key, QTreeWidgetItem* parent)
{
    int i;

    if (!parent)
        return nullptr;

    if(parent->childCount()>0){
        for(i=0;i<parent->childCount();i++){
            if(parent->child(i)!=0)
            {
                //qDebug() << parent->child(i)->data(0, Qt::UserRole).toString();

                if (parent->child(i)->data(0, Qt::UserRole).toString() == key)
                    return parent->child(i);
                else{
                    auto ch = findTreeItem(key, parent->child(i));
                    if(ch)
                        return ch;
                }
            }

        }
    }

    return nullptr;
}

QJsonObject MainWindow::parseDeviceString(const QString& key){
    QJsonObject obj = QJsonObject();
    QStringList m_data = key.split("\\");

    QString device =  m_data[3].replace("\r", "");
    QString volume =  device;
    volume.replace("FAT12_", "");
    QString nameBase64 =  m_data[4].replace("\r", "");
    QString name = fromBase64(nameBase64);
    int ind =  name.indexOf("@");
    QString key_name = name.left(ind);

    obj.insert("device", device);
    obj.insert("nameBase64", nameBase64);
    obj.insert("name", name);
    obj.insert("key_name", key_name);
    obj.insert("volume", volume);

    return obj;
}

void MainWindow::saveAsCurrentUserContainer()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    auto model = getLocalMountedVolumes();

    auto dlgSel = new DialogSelectInList(model, "Выбор устройства", this);
    dlgSel->setModal(true);
    dlgSel->exec();
    if(dlgSel->result() == QDialog::Accepted){
        QStringList dlgResult = dlgSel->dialogResult();
        QString volume = dlgResult[0];
        QString path = dlgResult[1];

        QString name = table->model()->index(index.row(), 2).data().toString();
        int ind = name.indexOf("@");
        QString key_name = name.left(ind + 1) + ".000";

        if(path != "Реестр"){
            QDir dir(path + key_name);
            if(dir.exists()){
                QMessageBox::critical(this, "Ошибка", QString("Каталог с именем %1 уже существует на устройстве!").arg(key_name));
                return;
            }
        }

        auto result =  QMessageBox::question(this, "Копирование контейнера", QString("Копировать контейнер: \n%1 \n на устройство?").arg(volume));

        if(result != QMessageBox::Yes){
            return;
        }

        auto dlg = new DialogContainerName(name,this);
        dlg->setModal(true);
        dlg->exec();

        if(dlg->result() != QDialog::Accepted)
            return;

        QString newName = dlg->keyName() + dlg->name();
        QString nameBase64 = volume + QByteArray(newName.toUtf8()).toBase64();

        if(path == "Реестр"){
            QStringList lst = currentUser->getRigstryData();

            if(lst.lastIndexOf(newName) != -1 || lst.lastIndexOf(nameBase64) != -1){
                QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже существует в реестре!").arg(newName));
                return;
            }
        }



        QModelIndex _index = table->model()->index(index.row(), 1);
        QString device = _index.model()->data(_index, Qt::UserRole + 1).toString().replace("\r", "");

        QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
        terminal->send(cmd, csptestContainerCopy);
    }
}

void MainWindow::saveAsCurrentUserCertificate()
{
    terminal->send("certmgr.msc", unknown);
//    auto table = ui->tableView;
//    auto index = table->currentIndex();
//    if(!index.isValid()){
//        QMessageBox::critical(this, "Ошибка", "Не выбран сертификат!");
//        return;
//    }

//    QString fileName = QFileDialog::getSaveFileName(this, tr("Выбрать файл"),
//                                                 QDir::homePath(),
//                                                 "Файл сертификата (*.cer)");
//    if(fileName != ""){
//        QFile file(fileName);
//        //QFileInfo fileInfo(file.fileName());
//        //if(file.open(QIODevice::ReadOnly)){

//        //certmgr -export -dn 'CN=Ли Александр Сергеевич' -dest test.cer

//        int col = modelCurrentUserCerts->getColumnIndex("serial");
//        QString serial = modelCurrentUserCerts->index(index.row(), col).data().toString();

//        auto certItr = currentUser->certificates().find(serial);
//        if(certItr != currentUser->certificates().end()){
//            QString CN = certItr.value()->getParam("Subject", "CN");
//            if(!CN.isEmpty()){
//                //QString cmd = QString("certmgr -export -dn \"CN=%1\" -dest \"%2\"\n").arg(CN, fileName);
//                QString cmd = QString("certmgr -export -dest \"%1\"\n").arg(fileName);
//                terminal->send(cmd, unknown);
//            }
//        }
    //    }
}

void MainWindow::saveAsDatabaseContainer()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    auto model = getLocalMountedVolumes();
    auto dlgSel = DialogSelectInList(model, "Выбор устройства", this);
    dlgSel.setModal(true);
    dlgSel.exec();

    if(dlgSel.result() == QDialog::Accepted){
        QStringList dlgResult = dlgSel.dialogResult();
        QString volume = dlgResult[0];
        QString path = dlgResult[1];

        int colName = modelSqlContainers->getColumnIndex("FirstField");
        auto name = modelSqlContainers->index(index.row(), colName).data().toString();
        //QString name = table->model()->index(index.row(), 2).data().toString();
        int ind = name.indexOf("@");
        QString key_name = name.left(ind + 1) + ".000";

        if(path != "Реестр"){
            QDir dir(path + key_name);
            if(dir.exists()){
                QMessageBox::critical(this, "Ошибка", QString("Каталог с именем %1 уже существует в хранилище!").arg(key_name));
                return;
            }
        }

        auto result =  QMessageBox::question(this, "Копирование контейнера", QString("Копировать контейнер в хранилище: %1?").arg(volume));

        if(result != QMessageBox::Yes){
            return;
        }

//        auto dlg = DialogContainerName(name,this);
//        dlg.setModal(true);
//        dlg.exec();

//        if(dlg.result() != QDialog::Accepted)
//            return;

        QString newName = name; //dlg.name();
        QString fillName = volume + newName;

        if(path == "Реестр"){
            QStringList lst = currentUser->getRigstryData();

            if(lst.lastIndexOf(newName) != -1){
                QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже существует в реестре!").arg(newName));
                return;
            }
        }

        auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "Containers");

        int col = modelSqlContainers->getColumnIndex("Ref");
        QString ref = modelSqlContainers->index(index.row(), col).data(Qt::UserRole + col).toString();

        bindQuery.addField("Ref", "Ref");
        bindQuery.addField("FirstField", "FirstField");
        bindQuery.addField("data", "data");
        bindQuery.addWhere("Ref", ref);

        QString query = bindQuery.to_json();

        QJsonObject cmd = QJsonObject();
        cmd.insert("command", "copy_container_from_data");
        cmd.insert("toDevice", fillName);

        if(_sett->launch_mode() == mixed){
            if(!isDbOpen())
                return;
            QString resultQuery;
            QString _error;
            db->exec_qt(query, resultQuery, _error);
            auto doc = QJsonDocument();
            doc.setObject(cmd);
            onGetDataFromDatabase(resultQuery, doc.toJson());
        }else{
            if(m_client->isStarted()){
                auto obj = QJsonObject();
                obj.insert("query", query);
                obj.insert("header", true);
                obj.insert("id_command", "get_data");
                obj.insert("run_on_return", cmd);
                auto doc = QJsonDocument();
                doc.setObject(obj);
                QString param = doc.toJson();
                m_client->sendCommand("exec_query_qt", "", param);
            }
        }

    }
}

void MainWindow::saveAsDatabaseCertificate()
{

}

void MainWindow::addCertificate()
{
    qDebug() << __FUNCTION__;


    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    if(node == SqlCertificates || node == currentUserCertificates){

        QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                     QDir::homePath(),
                                                     "Файл сертификата (*.cer)");
        if(fileName != ""){
            QFile file(fileName);
            if(!file.open(QIODevice::ReadOnly))
                return;
            bool removeTmp = false;
            if(isCyrillic(fileName)){
                //не могу победить кодировку
                //
                QString uuid = QUuid::createUuid().toString();
                uuid = uuid.mid(1, uuid.length() - 2);
                QString newPath = QCoreApplication::applicationDirPath() + QDir::separator() + uuid + ".tmp";
                file.copy(newPath);
                fileName = newPath;
                removeTmp = true;
                if(!QFile::exists(fileName)){
                    file.close();
                    return;
                }
            }

            file.close();

            auto cert = Certificate(this);
            cert.fromFile(fileName, removeTmp);
            if(cert.isValid()){
                if(node == SqlCertificates){
                    auto bindQuery = cert.getSqlQueryObject(QBSqlCommand::QSqlInsert);
                    QString result = bindQuery.to_json();
                    if(_sett->launch_mode() == mixed){
                        auto bindQuery1 = QBSqlQuery();
                        bindQuery1.fromJson(result);
                        QSqlQuery sql = bindQuery1.query(db->getDatabase());
                        sql.exec();
                        if(sql.lastError().type() != QSqlError::NoError){
                            qDebug() << __FUNCTION__ << sql.lastError().text();
                        }else{
                            QMessageBox::information(this, "Копирование на сервер", "Сертификат успешно скопирован на сервер!");
                            getDataContainersList();
                        }
                    }else{
                        if(m_client->isStarted()){
                            auto doc = QJsonDocument();
                            auto objMain = QJsonObject();
                            objMain.insert("query", result);
                            objMain.insert("id_command", insertCertificateToData);
                            doc.setObject(objMain);
                            QString param = doc.toJson();
                            m_client->sendCommand("exec_query_qt", "", param);
                        }
                    }

                }else if(node == currentUserCertificates){

                }
            }

        }

    }

}

void MainWindow::delCertificate()
{

    qDebug() << __FUNCTION__;

    auto table = ui->tableView;
    auto index = table->currentIndex();

    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    QString ref = table->model()->index(index.row(), 1).data().toString();
    QString name = table->model()->index(index.row(), 2).data().toString();
    auto bOK =  QMessageBox::question(this, "Удаление сертификата", QString("Удалить сертификат %1 с сервера?").arg(name));
    if(bOK == QMessageBox::No){
        return;
    }

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlDelete, "Certificates");
    bindQuery.addWhere("Ref", ref);

    if(_sett->launch_mode() == mixed){
        QSqlQuery sql = bindQuery.query(db->getDatabase());
        sql.exec();
        if(sql.lastError().type() != QSqlError::NoError){
            qDebug() << __FUNCTION__ << sql.lastError().text();
        }else
        {
            QMessageBox::information(this, "Удаление", "Сертификат успешно удален!");
            getDataCertificatesList();
        }
    }else{
        if(m_client->isStarted()){
            QString result = bindQuery.to_json();
            auto doc = QJsonDocument();
            auto obj = QJsonObject();
            obj.insert("query", result);
            obj.insert("id_command", deleteCertificateFromData);
            doc.setObject(obj);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::addContainer()
{

    QStringList select = {
        "Добавить из каталога",
        "Добавить из устройства"
    };

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    if(node != SqlContainers){
        select.append("Добавить из базы данных");
    }

    auto dlg = new DialogSelectDevice(this, select, "Добавить контейнер");
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){

        //QString query;
        auto bindQuery = QBSqlQuery();

        if(dlg->currentSelection() == 0){

            QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
            if(dir != ""){
                QFile file(dir + QDir::separator() + "name.key");
                if(file.open(QIODevice::ReadOnly)){
                    QString data = QString::fromLocal8Bit(file.readAll());
                    int ind = data.indexOf("\026");
                    if(ind != -1){
                        QString name = data.right(data.length() - ind - 2);
                        auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Импортировать контейнер: \n%1?").arg(name));
                        if(result == QMessageBox::Yes){
                            auto cnt = KeysContainer();
                            cnt.fromFolder(dir);
                            if(cnt.isValid()){
                                if(isCyrillic(name)){
                                    auto dlg = DialogContainerName(name, this);
                                    dlg.setModal(true);
                                    dlg.exec();
                                    if(dlg.result() == QDialog::Accepted){
                                        name = dlg.name();
                                        cnt.setNewOriginalName(name);
                                    }else
                                        return;
                                }
                                cnt.fromContainerName(name);
                                if(isContainerExists(name)){
                                    QMessageBox::critical(this, "Ошибка",
                                                          QString("Контейнер с именем: \n'%1' уже загружен в базу!").arg(name));
                                    return;
                                }else{
                                    bindQuery = cnt.getSqlQueryObject(QBSqlCommand::QSqlInsert);
                                }
                            }else
                                QMessageBox::critical(this, "Ошибка", "Ошибка импорта!");
                        }
                    }else
                       QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }else{
                    QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
                }
            }
        }else if(dlg->currentSelection() == 1){

            auto model = new QStandardItemModel(this);
            model->setHorizontalHeaderLabels({"Устройство", "Наименование", "Контейнер"});
            int row = 0;
            foreach(auto cntPt, currentUser->containers()){
                auto cnt = currentUser->container(cntPt);
                if(cnt){
                    model->setRowCount(row + 1);
                    auto item = new QStandardItem(cnt->volume());
                    model->setItem(row, 0, item);
                    item = new QStandardItem(cnt->bindName());
                    model->setItem(row, 1, item);
                    item = new QStandardItem(cntPt);
                    model->setItem(row, 2, item);
                    row++;
                }
            }

            auto dlgSel = DialogSelectInList(model, "Выбор устройства", this);
            dlgSel.setModal(true);
            dlgSel.exec();

            if(dlgSel.result() == QDialog::Accepted){
                QStringList dlgResult = dlgSel.dialogResult();
                if(dlgResult.size() == 0)
                    return;
                QString contaner = dlgResult[2];
                QString volume = dlgResult[0];
                //QString path = dlgResult[1];
                QString name;
                auto cnt = currentUser->container(contaner);
                if(cnt){
                    if(cnt->keyName().isEmpty())
                        cnt->fromContainerName(contaner);
                    if(volume.indexOf(REGISTRY_) != -1){
                        cnt->fromRegistry(currentUser->sid(), cnt->originalName());
                        cnt->originalName();
                    }else{
                       QDir dir(cnt->volumePath() + cnt->keyName() + ".000");
                      // qDebug() << dir.path();
                       if(dir.exists()){
                           cnt->fromFolder(dir.path());
                           name = cnt->originalName();
                       }else{
                           QMessageBox::critical(this, "Ошибка", "Произошла ошибка чтения с каталога!");
                           return;
                       }
                    }

                    if(cnt->isValid()){
                         auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Импортировать контейнер: \n%1?").arg(cnt->bindName()));

                         if(result == QMessageBox::Yes){
                             name = cnt->originalName();
                            if(isCyrillic(cnt->originalName())){
                                auto dlg = DialogContainerName(name, this);
                                dlg.setModal(true);
                                dlg.exec();
                                if(dlg.result() == QDialog::Accepted){
                                    name = dlg.name();
                                    if(name != cnt->originalName())
                                         cnt->setNewOriginalName(name);
                                }else
                                    return;
                            }
                         }else
                            return;

                         if(isContainerExists(name)){
                             QMessageBox::critical(this, "Ошибка",
                                                   QString("Контейнер с именем: \n'%1' уже загружен в базу!").arg(name));
                             return;
                         }else{
                             bindQuery = cnt->getSqlQueryObject(QBSqlCommand::QSqlInsert);
                         }
                    }

                }
            }
        }else if(dlg->currentSelection() == 2){

        }

        if(bindQuery.isValid()){
            if(_sett->launch_mode() == mixed){
                QSqlQuery sql = bindQuery.query(db->getDatabase());
                sql.exec();
                if(sql.lastError().type() != QSqlError::NoError){
                    qDebug() << __FUNCTION__ << sql.lastError().text();
                }else{
                    QMessageBox::information(this, "Копирование на сервер", "Контейнер успешно скопирован на сервер!");
                    getDataContainersList();
                }
            }else{
                if(m_client->isStarted()){
                    auto doc = QJsonDocument();
                    auto objMain = QJsonObject();
                    objMain.insert("query", bindQuery.to_json());
                    objMain.insert("id_command", insertContainerToData);
                    doc.setObject(objMain);
                    QString param = doc.toJson();
                    m_client->sendCommand("exec_query_qt", "", param);
                }
            }
        }else
            QMessageBox::critical(this, "Ошибка", "Произошла ошибка импорта!");

    }
}

void MainWindow::delContainer()
{
    qDebug() << __FUNCTION__;

    auto table = ui->tableView;
    auto index = table->currentIndex();

    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    QString ref = table->model()->index(index.row(), 1).data().toString();
    QString name = table->model()->index(index.row(), 2).data().toString();
    auto bOK =  QMessageBox::question(this, "Удаление контейнера", QString("Удалить контейнер %1 с сервера?").arg(name));
    if(bOK == QMessageBox::No){
        return;
    }

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlDelete, "Containers");
    bindQuery.addWhere("Ref", ref);

    if(_sett->launch_mode() == mixed){
        QSqlQuery sql = bindQuery.query(db->getDatabase());
        sql.exec();
        if(sql.lastError().type() != QSqlError::NoError){
            qDebug() << __FUNCTION__ << sql.lastError().text();
        }else
        {
            QMessageBox::information(this, "Удаление", "Контейнер успешно удален!");
            getDataContainersList();
        }
    }else{
        if(m_client->isStarted()){
            QString result = bindQuery.to_json();
            auto doc = QJsonDocument();
            auto obj = QJsonObject();
            obj.insert("query", result);
            obj.insert("id_command", deleteContainerFromData);
            doc.setObject(obj);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::delCertUser()
{
    qDebug() << __FUNCTION__;

    auto table = ui->tableView;
    auto index = table->currentIndex();

    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    int row = index.row();

    int col = modelSqlUsers->getColumnIndex("FirstField");
    auto user = modelSqlUsers->index(row, col).data(Qt::UserRole + col).toString();
    col = modelSqlUsers->getColumnIndex("Ref");
    auto ref = modelSqlUsers->index(row, col).data(Qt::UserRole + col).toString();

    auto bOK =  QMessageBox::question(this, "Удаление пользователя", QString("Удалить ползователя %1 с сервера?").arg(user));
    if(bOK == QMessageBox::No){
        return;
    }

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlDelete, "CertUsers");
    bindQuery.addWhere("Ref", ref);

    if(_sett->launch_mode() == mixed){
        QSqlQuery sql = bindQuery.query(db->getDatabase());
        sql.exec();
        if(sql.lastError().type() != QSqlError::NoError){
            qDebug() << __FUNCTION__ << sql.lastError().text();
        }else
        {
            QMessageBox::information(this, "Удаление", "Пользователь успешно удален!");
            getDataUsersList();
        }
    }else{
        if(m_client->isStarted()){
            QString result = bindQuery.to_json();
            auto doc = QJsonDocument();
            auto obj = QJsonObject();
            obj.insert("query", result);
            obj.insert("id_command", deleteUserFromData);
            doc.setObject(obj);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::addCertUser()
{

}

QStandardItemModel *MainWindow::getLocalMountedVolumes()
{
    auto model = new QStandardItemModel(this);
    model->setColumnCount(2);
    model->setHorizontalHeaderLabels({"Volume", "Path", "Name"});

    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
         if (storage.isValid() && storage.isReady()) {
             model->setRowCount(model->rowCount() +1);
             if (!storage.isReadOnly()) {
                 QStringList v = storage.rootPath().split(":");
                 QString vol = QString("\\\\.\\FAT12_%1\\").arg(v[0]);

                 int row = model->rowCount()-1;
                 auto item = new QStandardItem(vol);
                 model->setItem(row, 0, item);
                 item = new QStandardItem(storage.rootPath());
                 model->setItem(row, 1, item);
                 item = new QStandardItem(storage.displayName());
                 model->setItem(row, 2, item);
             }
         }
     }
    model->setRowCount(model->rowCount() +1);
    int row = model->rowCount()-1;
    auto item = new QStandardItem("\\\\.\\REGISTRY\\");
    model->setItem(row, 0, item);
    item = new QStandardItem("Реестр");
    model->setItem(row, 2, item);
    return model;
}

void MainWindow::updateCertUsersOnlineStstus()
{
    int nameIndex = modelSqlUsers->getColumnIndex("FirstField");
    int nameHost = modelSqlUsers->getColumnIndex("host");

    for (int i = 0; i < modelSqlUsers->rowCount(); ++i) {

        QString name = modelSqlUsers->index(i, nameIndex).data(Qt::UserRole + nameIndex).toString();
        QString host = modelSqlUsers->index(i, nameHost).data(Qt::UserRole + nameHost).toString();
        auto index = modelSqlUsers->index(i, 1);

        if(isWsUserExists(name, host)){
            modelSqlUsers->setIcon(index, QIcon(":/img/online.png"));
        }else{
            modelSqlUsers->setIcon(index, QIcon(":/img/ofline.png"));
        }
    }

    modelSqlUsers->reset();
}

void MainWindow::treeSetCurrentContainers(const QString& filter)
{
        auto tableView = ui->tableView;
        tableView->setModel(nullptr);

//        currentList.filter = "{\"IsGroup\": 1,
//                               \"Parent\": \"" + uuid + "\"}"
        proxyModelUserConteiners->setFilter(QString("{\"volume\":\"%1\"}").arg(filter));
        tableView->setModel(proxyModelUserConteiners);

        int ind = modelUserContainers->getColumnIndex("name");
        if(ind != -1)
            tableView->setColumnHidden(ind, true);
        ind = modelUserContainers->getColumnIndex("nameInStorgare");
        if(ind != -1)
            tableView->setColumnHidden(ind, true);

        tableView->resizeColumnsToContents();

}

void MainWindow::treeSetFromCurrentUserCerts()
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(modelUserCertificates){
        table->setModel(modelUserCertificates);
        int col = modelUserCertificates->getColumnIndex("serial");
        table->setColumnHidden(col, true);
        table->resizeColumnsToContents();
    }
}


void MainWindow::treeSetFromSqlContainers()
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(modelSqlContainers){
        table->setModel(modelSqlContainers);
        int index = modelSqlContainers->getColumnIndex("Ref");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlContainers->getColumnIndex("cache");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlContainers->getColumnIndex("FirstField");
        if(index > 0)
            table->setColumnHidden(index, true);
        table->resizeColumnsToContents();
    }
}

void MainWindow::treeSetFromSqlUsers()
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(modelSqlUsers){
        table->setModel(modelSqlUsers);
        int index = modelSqlUsers->getColumnIndex("Ref");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlUsers->getColumnIndex("uuid");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlUsers->getColumnIndex("sid");
        if(index > 0)
            table->setColumnHidden(index, true);

        int nameIndex = modelSqlUsers->getColumnIndex("FirstField");
        int nameHost = modelSqlUsers->getColumnIndex("host");


        for (int i = 0; i < modelSqlUsers->rowCount(); ++i) {

            QString name = modelSqlUsers->index(i, nameIndex).data(Qt::UserRole + nameIndex).toString();
            QString host = modelSqlUsers->index(i, nameHost).data(Qt::UserRole + nameHost).toString();
            auto index = modelSqlUsers->index(i, 1);

            if(isWsUserExists(name, host)){
                modelSqlUsers->setIcon(index, QIcon(":/img/online.png"));
            }else{
                modelSqlUsers->setIcon(index, QIcon(":/img/ofline.png"));
            }
        }

        table->resizeColumnsToContents();
    }
}

void MainWindow::treeSetFromSqlCertificates()
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(modelSqlCertificates){
        table->setModel(modelSqlCertificates);
        int index = modelSqlCertificates->getColumnIndex("Ref");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlCertificates->getColumnIndex("FirstField");
        if(index > 0)
            table->setColumnHidden(index, true);
        index = modelSqlCertificates->getColumnIndex("serial");
        if(index > 0)
            table->setColumnHidden(index, true);
        table->resizeColumnsToContents();
    }
}

void MainWindow::treeSetOnlineWsUsers()
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(modelWsUsers){
        table->setModel(modelWsUsers);
//        int index = modelWsUsers->getColumnIndex("uuid");
//        if(index > 0)
//            table->setColumnHidden(index, true);
//        index = modelWsUsers->getColumnIndex("user_uuid");
//        if(index > 0)
//            table->setColumnHidden(index, true);

        table->resizeColumnsToContents();

    }
}

void MainWindow::updateContainerInfoOnData(const QString &info)
{
    auto index = ui->tableView->currentIndex();
    if(!index.isValid()){
        return;
    }

    int indNameInStorgare = modelUserContainers->getColumnIndex("nameInStorgare");
    int indName = modelUserContainers->getColumnIndex("name");
    int indSecondName = modelUserContainers->getColumnIndex("SecondField");
    QString name = ui->tableView->model()->index(index.row(), indName).data().toString();
    QString second_name = ui->tableView->model()->index(index.row(), indSecondName).data().toString();
    QString nameInStorgare = ui->tableView->model()->index(index.row(), indNameInStorgare).data().toString();

    KeysContainer * cnt = currentUser->container(nameInStorgare);
    if(cnt){
        QString notValidBefore = cnt->notValidBefore();
        QJsonObject obj = cnt->parseCsptestInfo(info);
        if(notValidBefore != cnt->notValidBefore()){
            modelUserContainers->setJsonText(currentUser->modelContainersText());
            modelUserContainers->reset();
        }
        auto dlg = new DialogContainerInfo(obj, second_name, this);
        dlg->setModal(true);
        dlg->exec();

        QString nameBase64 = name.toUtf8().toBase64();
        auto indexCnt = findInTable(modelSqlContainers, name, 2, false);

        if(indexCnt.isValid()){
            int col = modelSqlContainers->getColumnIndex("cache");
            QString cache = modelSqlContainers->index(indexCnt.row(), col).data(Qt::UserRole + col).toString();
            if(cache.isEmpty()){
                auto doc = QJsonDocument();
                doc.setObject(obj);
                updateInfoContainerOnDatabase(doc.toJson(), name, nameBase64, cnt);
                getDataContainersList();
            }
        }
    }else
        QMessageBox::critical(this, "Ошибка", "Ошибка чтения информаци о контейнере!");


}

QModelIndex MainWindow::findInTable(QAbstractItemModel * model, const QString &value, int column, bool findData)
{
    int rows =  model->rowCount();
    for (int i = 0; i < rows; ++i) {
        auto index = model->index(i, column);
        if(findData){
            if(value == index.data(Qt::UserRole + 1).toString())
                return index;
        }else{
            QString data = index.data(Qt::UserRole + column).toString();
            if(value == data)
                return index;
        }
    }

    return QModelIndex();
}

QString MainWindow::validUuid(const QString &uuid)
{
    QString _uuid = uuid;
    _uuid.replace("{", "");
    _uuid.replace("}", "");
    return _uuid;
}

QString MainWindow::fromBase64(const QString &value)
{
    QString s = value.trimmed();
    QRegularExpression re("^[a-zA-Z0-9\\+/]*={0,3}$");
    bool isBase64 = (s.length() % 4 == 0) && re.match(s).hasMatch();
    if(!isBase64)
       return value;

    QString result;
    try {
        return QByteArray::fromBase64(value.toUtf8());
    }  catch (std::exception &e) {
        return value;
    }
}

bool MainWindow::isCyrillic(const QString &source)
{
    for (int i = 0; i < source.length();  ++i) {

        if(Cyrillic.indexOf(source.mid(i, 1)) != -1)
            return true;
    }

    return false;
}



void MainWindow::onOutputCommandLine(const QString &data, int command)
{
    //qDebug() << __FUNCTION__;
    //qDebug() << __FUNCTION__ << qPrintable(data);
    ui->txtTerminal->setText(ui->txtTerminal->toPlainText() + data);
    ui->txtTerminal->verticalScrollBar()->setValue(ui->txtTerminal->verticalScrollBar()->maximum());

    qDebug() << __FUNCTION__ << "command: " << (CmdCommand)command;

    if(data.indexOf("Error:") > 0)
        return;

    terminal->parseCommand(data, command);
}

MainWindow::~MainWindow()
{
    terminal->stop();

    if(m_client)
        delete m_client;
    delete ui;
}

void MainWindow::createTree()
{
    qDebug() << __FUNCTION__;
    auto tree = ui->treeWidget;   
    //tree->setColumnCount(1);
    tree->setHeaderHidden(true);
//    tree->setColumnHidden(1, true);

//    auto * root = new QTreeWidgetItem();
//    auto * rootId = new QTreeWidgetItem();
//    root-> setText (0, "root");
//    rootId-> setText (0, "root");
//    root->setIcon(0, QIcon(":/img/system_config_root.ico"));


    auto root = addTreeNode("root", "root", ":/img/system_config_root.ico");
    tree->addTopLevelItem(root);
    auto curr_user = addTreeNode("Текущий пользователь", "currentUser", ":/img/userOptions.ico");
    root->addChild(curr_user);
    auto server = addTreeNode("База", "SqlServer", ":/img/sqlServer.png");
    root->addChild(server);
    auto ws = addTreeNode("Сервер взаимодействия", "WsServer", ":/img/socket_16_only.ico");
    root->addChild(ws);

////    QString usrName = currentUser->name();

//    auto * curr_user = new QTreeWidgetItem();
////    if(usrName.isEmpty())
//        curr_user->setText(0, "Текущий пользователь");
////    else
////       curr_user->setText(0, QString("Текущий пользователь (%1)").arg(usrName));
////    curr_user->setToolTip(0, QString("Текущий пользователь (%1)").arg(usrName));
//    curr_user->setIcon(0, QIcon(":/img/userOptions.ico"));
//    root->addChild(curr_user);

    //currentUser->setTreeItem(curr_user);

//    auto * allowed_keys = new QTreeWidgetItem();
//    allowed_keys->setText(0, "Доступные контейнеры");
//    //allowed_keys->setIcon(0, QIcon(":/img/registry.ico"));
//    curr_user->addChild(allowed_keys);

//    auto * currUserReg = new QTreeWidgetItem();
//    currUserReg->setText(0, "Реестр");
//    currUserReg->setIcon(0, QIcon(":/img/registry.ico"));
//    allowed_keys->addChild(currUserReg);

//    auto * currUserDevice = new QTreeWidgetItem();
//    currUserDevice->setText(0, "Устройства");
//    //currUserReg->setIcon(0, QIcon(":/img/registry.ico"));
//    allowed_keys->addChild(currUserDevice);

//    auto * server = new QTreeWidgetItem();
//    server->setText(0, "База");
//    server->setIcon(0, QIcon(":/img/sqlServer.png"));
//    root->addChild(server);

//    auto * containers = new QTreeWidgetItem();
//    containers->setText(0, "Контейнеры");
//    containers->setIcon(0, QIcon(":/img/key_password_lock_800.ico"));
//    server->addChild(containers);

//    auto * certs = new QTreeWidgetItem();
//    certs->setText(0, "Сертификаты");
//    certs->setIcon(0, QIcon(":/img/certificate.ico"));
//    server->addChild(certs);

//    auto * users = new QTreeWidgetItem();
//    users->setText(0, "Пользователи");
//    users->setIcon(0, QIcon(":/img/bootloader_users_person_people_6080.ico"));
//    server->addChild(users);

//    auto * comps = new QTreeWidgetItem();
//    comps->setText(0, "Компьютеры");
//    comps->setIcon(0, QIcon(":/img/computers.ico"));
//    root->addChild(comps);

//    auto * ws = new QTreeWidgetItem();
//    ws->setText(0, "Сервер взаимодействия");
//    ws->setIcon(0, QIcon(":/img/socket_16_only.ico"));
//    root->addChild(ws);

//    auto * activeUsers = new QTreeWidgetItem();
//    activeUsers->setText(0, "Активные пользователи");
//    activeUsers->setIcon(0, QIcon(":/img/activeUesers.png"));
//    ws->addChild(activeUsers);

    tree->expandAll();
}

Certificate *MainWindow::selectCertFromLocalHost()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                 QDir::homePath(),
                                                 "Файл сертификата (*.cer)");
    if(fileName != ""){
        QFile file(fileName);
        //QFileInfo fileInfo(file.fileName());

        if(isCyrillic(fileName)){

        }

    }

    return nullptr;
}

//void MainWindow::createRootList()
//{
//    qDebug() << __FUNCTION__;
//    auto table = ui->tableView;
//    table->setModel(nullptr);

//    if(!isDbOpen())
//        return;

//    auto model = new QStandardItemModel(this);

//    model->setColumnCount(1);
//    model->setRowCount(3);

//    QStringList cols = {"root"};
//    model->setHorizontalHeaderLabels(cols);

//    auto itemKeys = new QStandardItem("Контейнеры");
//    itemKeys->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//    itemKeys->setIcon(QIcon(":/img/key_password_lock_800.ico"));
//    model->setItem(0, 0, itemKeys);
//    auto itemCerts = new QStandardItem("Сертификаты");
//    itemCerts->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//    itemCerts->setIcon(QIcon(":/img/certificate.ico"));
//    model->setItem(1, 0, itemCerts);
//    auto itemUsers = new QStandardItem("Пользователи");
//    itemUsers->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//    itemUsers->setIcon(QIcon(":/img/bootloader_users_person_people_6080.ico"));
//    model->setItem(2, 0, itemUsers);

//    table->setModel(model);
//    table->resizeColumnsToContents();

//}

void MainWindow::resetTableJsonModel(const QJsonObject &obj, const QString &id_command){

    if(id_command == DataContainersList){
        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonConteiners = QByteArray::fromBase64(base64.toUtf8());

        modelSqlContainers->setJsonText(jsonConteiners);
        modelSqlContainers->reset();

        auto itemContainers = findTreeItem(SqlContainers);
        if(!itemContainers){
            auto rootDb = findTreeItem(SqlServer);
            if(rootDb){
               itemContainers = addTreeNode("Контейнеры", SqlContainers, ":/img/zamok.png");
               rootDb->addChild(itemContainers);
            }
        }

        auto itemCertificates = findTreeItem(SqlCertificates);
        if(!itemCertificates){
            getDataCertificatesList();
        }
    }else if(id_command == DataCertificatesList){

        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonCertificates = QByteArray::fromBase64(base64.toUtf8());

        modelSqlCertificates->setJsonText(jsonCertificates);
        modelSqlCertificates->reset();

        auto itemCertificates = findTreeItem(SqlCertificates);
        if(!itemCertificates){
            auto rootDb = findTreeItem(SqlServer);
            if(rootDb){
               itemCertificates = addTreeNode("Сертификаты", SqlCertificates, ":/img/cert.png");
               rootDb->addChild(itemCertificates);
            }
        }
        auto itemUsers = findTreeItem(SqlUsers);
        if(!itemUsers){
            getDataUsersList();
        }
    }else if(id_command == DataUsersList){

        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonUsers = QByteArray::fromBase64(base64.toUtf8());

        modelSqlUsers->setJsonText(jsonUsers);

        modelSqlUsers->reset();

        auto itemUsers = findTreeItem(SqlUsers);
        if(!itemUsers){
            auto rootDb = findTreeItem(SqlServer);
            if(rootDb){
               itemUsers = addTreeNode("Пользователи", SqlUsers, ":/img/certUsers.png");
               rootDb->addChild(itemUsers);
            }
        }
    }
}

void MainWindow::getDataContainersList()
{

    qDebug() << __FUNCTION__;
//    auto tableView = ui->tableView;
//    tableView->setModel(nullptr);

    if(_sett->launch_mode() == mixed){
        if(!isDbOpen())
            return;

        auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "Containers");

        QJsonObject objSel = QJsonObject();

        objSel.insert("name", "EmptyTitle");
        objSel.insert("value", "EmptyTitle");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "Ref");
        objSel.insert("value", "Ref"); //alias
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "FirstField");
        objSel.insert("value", "FirstField");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "SecondField");
        objSel.insert("value", "SecondField");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "subject");
        objSel.insert("value", "subject");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "issuer");
        objSel.insert("value", "issuer");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "notValidBefore");
        objSel.insert("value", "notValidBefore");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "notValidAfter");
        objSel.insert("value", "notValidAfter");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "parentUser");
        objSel.insert("value", "parentUser");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        objSel = QJsonObject();
        objSel.insert("name", "cache");
        objSel.insert("value", "cache");
        bindQuery.add_field(objSel, bFieldType::qVariant);

        QString query = bindQuery.to_json();
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);

        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, "DataContainersList");

    }else{

        if(!m_client->isStarted())
            return;
        QString query = "SELECT NULL AS EmptyTitle, [Ref] , [FirstField] , [cache] FROM [arcirk].[dbo].[Containers]";
        auto obj = QJsonObject();
        obj.insert("query", query);
        obj.insert("header", true);
        obj.insert("id_command", "DataContainersList");
        auto doc = QJsonDocument();
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("exec_query", "", param);

    }
}

void MainWindow::getDataCertificatesList()
{
    qDebug() << __FUNCTION__;


    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "Certificates");
    bindQuery.addField("EmptyTitle", "EmptyTitle");
    bindQuery.addField("Ref", "Ref");
    bindQuery.addField("FirstField", "FirstField");
    bindQuery.addField("subject", "subject");
    bindQuery.addField("issuer", "issuer");
    bindQuery.addField("notValidBefore", "notValidBefore");
    bindQuery.addField("notValidAfter", "notValidAfter");
    bindQuery.addField("parentUser", "parentUser");
    bindQuery.addField("serial", "serial");
    bindQuery.addField("privateKey", "privateKey");

    QString query = bindQuery.to_json();

    if(_sett->launch_mode() == mixed){
        if(!isDbOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);

        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, DataCertificatesList);
    }else{
        if(m_client->isStarted()){
            auto doc = QJsonDocument();
            auto objMain = QJsonObject();
            objMain.insert("query", query);
            objMain.insert("id_command", DataCertificatesList);
            doc.setObject(objMain);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::getDataUsersList()
{
    qDebug() << __FUNCTION__;

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "CertUsers");
    bindQuery.addField("Empty", "Empty");
    bindQuery.addField("EmptyisOnline", "EmptyisOnline");
    bindQuery.addField("FirstField", "FirstField");
    bindQuery.addField("SecondField", "SecondField");
    bindQuery.addField("Ref", "Ref");
    bindQuery.addField("uuid", "uuid");
    bindQuery.addField("sid", "sid");
    bindQuery.addField("host", "host");

    QString result = bindQuery.to_json();

    if(_sett->launch_mode() == mixed){
        if(!isDbOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(result, resultQuery, _error);
        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, DataUsersList);
    }else{
        if(!m_client->isStarted())
            return;
        auto obj = QJsonObject();
        obj.insert("query", result);
        obj.insert("header", true);
        obj.insert("id_command", DataUsersList);
        auto doc = QJsonDocument();
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("exec_query_qt", "", param);
    }
}

void MainWindow::resetCurrentUserCertModel()
{

    auto doc = QJsonDocument();
    auto objCols = QJsonArray();
    objCols.append("Empty");
    objCols.append("subject");
    objCols.append("issuer");
    objCols.append("notValidBefore");
    objCols.append("notValidAfter");
    objCols.append("parentUser");
    objCols.append("container");
    objCols.append("serial");


    auto objRows = QJsonArray();
    foreach(auto itr, currentUser->certificates()){
        auto row = QJsonObject();
        row.insert("Empty", QString());
        row.insert("subject", itr->subject());
        row.insert("issuer", itr->issuer());
        row.insert("notValidBefore", itr->notValidBefore());
        row.insert("notValidAfter", itr->notValidAfter());
        row.insert("parentUser", itr->parentUser());
        row.insert("container", itr->container());
        row.insert("serial", itr->serial());
        objRows.append(row);
    }

    auto objMain = QJsonObject();
    objMain.insert("columns", objCols);
    objMain.insert("rows", objRows);
    doc.setObject(objMain);

    modelUserCertificates->setJsonText(doc.toJson());
    modelUserCertificates->reset();
}

//void MainWindow::LoadUsersList()
//{
//    qDebug() << __FUNCTION__;
//    auto table = ui->tableView;
//    table->setModel(nullptr);
//    ui->btnAdd->setEnabled(true);

//    if(!isDbOpen())
//        return;

//    QString result;
//    QString query = "SELECT NULL AS isOnline\n"
//            ",[FirstField] AS Имя\n"
//            ",[Ref] AS Ссылка\n"
//            ",[uuid] AS ID\n"
//            ",[sid] AS SID\n"
//            ",[host] AS Host\n"
//            "FROM [dbo].[CertUsers]";

//    auto model = new QSqlQueryModel(this);
//    model->setQuery(query, db->getDatabase());
//    ui->tableView->setModel(model);


//    for (int i = 0; i < ui->tableView->model()->rowCount(); ++i) {
//        QModelIndex index = ui->tableView->model()->index(i, 0);
//        QModelIndex indexUuid = ui->tableView->model()->index(i, 3);
//        QUuid uuid = QUuid::fromString(indexUuid.data().toString().trimmed());
//         auto iter = m_actUsers.find(uuid);

//         QLabel *lbl_item = new QLabel();
//         if(iter != m_actUsers.end()){
//             if(iter.value()->online()){
//                 lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/online.png")));
//             }else{
//                 lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/ofline.png")));
//             }

//         }else{
//            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/ofline.png")));
//         }
//         lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//         ui->tableView->setIndexWidget(index, lbl_item);
//     }

//    ui->tableView->model()->setHeaderData(0, Qt::Orientation::Horizontal, "");
//    ui->tableView->resizeColumnsToContents();
//}

void MainWindow::loadCertList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);
    ui->btnAdd->setEnabled(true);

    if(!isDbOpen())
        return;

    QString result;
    QString query = "SELECT [_id]\n"
                    ",[FirstField]\n"
                    ",[SecondField]\n"
                    ",[Ref]\n"
                    ",[privateKey]\n"
                    "FROM [dbo].[Certificates]";


    auto model = new QSqlQueryModel(this);
    model->setQuery(query, db->getDatabase());
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();

}

void MainWindow::createUsersList()
{
    qDebug() << __FUNCTION__;
    auto table = ui->tableView;
    table->setModel(nullptr);
}

void MainWindow::enableToolbar(bool value)
{
    qDebug() << __FUNCTION__;
    for (auto btn : toolBar) {
        btn->setEnabled(value);
    }
}

void MainWindow::loadItemChilds(QTreeWidgetItem *item)
{
    qDebug() << __FUNCTION__;
    auto tableView = ui->tableView;
    tableView->setModel(nullptr);
    auto table = new QStandardItemModel(this);
    table->setColumnCount(1);
    table->setRowCount(item->childCount());    
    QStringList cols = {"", item->text(0)};
    table->setHorizontalHeaderLabels(cols);
    for (int i = 0; i < item->childCount(); i++) {
        auto child = item->child(i);
        auto itemTable = new QStandardItem();
        itemTable->setIcon(child->icon(0));
//        itemTable->setData(child->data(0, Qt::UserRole), Qt::UserRole);
//        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(i, 0, itemTable);
        itemTable = new QStandardItem(child->text(0));
        itemTable->setData(child->data(0, Qt::UserRole), Qt::UserRole);
        itemTable->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(i, 1, itemTable);
    }

    tableView->setModel(table);
    tableView->resizeColumnsToContents();
}

void MainWindow::loadItemSpecific(QTreeWidgetItem *item)
{
qDebug() << __FUNCTION__;
}

void MainWindow::getAvailableContainers(CertUser *usr)
{
    qDebug() << __FUNCTION__;

    if(usr == currentUser)
        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
}

//void MainWindow::loadOnlineUsers()
//{
//    qDebug() << __FUNCTION__;
//    auto table = ui->tableView;
//    table->setModel(nullptr);

//    if(!m_client->isStarted())
//        return;

//    auto model = new QStandardItemModel(this);
//    model->setColumnCount(4);
//    model->setRowCount(m_actUsers.size() - 1);
//    QStringList header = {"Имя", "Хост", "ID пользователя", "SID"};
//    model->setHorizontalHeaderLabels(header);
//    int row = 0;
//    for(auto itr : m_actUsers){
//        if(itr->online()){
//            auto name = new QStandardItem(itr->name());
//            model->setItem(row, 0, name);
//            auto host = new QStandardItem(itr->domain());
//            model->setItem(row, 1, host);
//            auto uuid = new QStandardItem(itr->uuid().toString());
//            model->setItem(row, 2, uuid);
//            auto sid = new QStandardItem(itr->sid());
//            model->setItem(row, 3, sid);
//        }
//    }
//    table->setModel(model);
//    table->resizeColumnsToContents();
//}

void MainWindow::disableToolBar()
{
    qDebug() << __FUNCTION__;
    ui->btnAdd->setEnabled(false);
    ui->btnEdit->setEnabled(false);
    ui->btnDelete->setEnabled(false);
    ui->btnImportFromDatabase->setEnabled(false);
    ui->btnToDatabase->setEnabled(false);
}

bool MainWindow::isContainerExists(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(_sett->launch_mode() == mixed){
        if(db->isOpen()){
            QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Containers] WHERE [FirstField] = '%1'").arg(name.toUtf8().toBase64()), db->getDatabase());
            while (query.next()) {
                return true;
            }
        }
    }else{
        int ind = modelSqlContainers->getColumnIndex("FirstField");
        if(ind != -1){
            auto index = findInTable(modelSqlContainers, name, ind, false);
            return index.isValid();
        }
    }

    return false;
}

bool MainWindow::isCertUserExists(const QString &name, const QString& host)
{
    qDebug() << __FUNCTION__;
    if(db->isOpen()){
        QString _host;
        if(!host.isEmpty()){
            _host = QString(" AND [host] = '%1'").arg(host);
        }
        QString str = QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[CertUsers] WHERE [FirstField] = '%1'%2").arg(name, _host);
        //qDebug() << str;
        QSqlQuery query(str, db->getDatabase());
        while (query.next()) {
            return true;
        }
    }

    return false;
}

bool MainWindow::isWsUserExists(const QString &name, const QString &host)
{

    if(modelWsUsers){
        QString findKey = name + host;
        for (int i = 0; i < modelWsUsers->rowCount(); ++i) {
            if(modelWsUsers->rowKey(i) == findKey){
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::isHostExists(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(db->isOpen()){
        QSqlQuery query(QString("SELECT [_id] , [FirstField] AS name FROM [arcirk].[dbo].[Servers] WHERE [FirstField] = '%1'").arg(name), db->getDatabase());
        while (query.next()) {
            return true;
        }
    }

    return false;
}

void MainWindow::userToDatabase(const QString &name)
{
    qDebug() << __FUNCTION__;
    if(isCertUserExists(name))
    {
        QMessageBox::information(this, "Пользователь", "Пользователь уже есть на севрвере!");
    }
}

void MainWindow::loadCimputers()
{
    qDebug() << __FUNCTION__;
    ui->tableView->setModel(nullptr);
    auto queryModel = new QSqlQueryModel(this);
    queryModel->setQuery("SELECT [_id]\n"
                         ", NULL AS Сервер\n"
                         ",[FirstField] AS Наименование\n"
                         ",[SecondField] AS Представление\n"
                         ",[Ref] AS Идентификатор\n"
                         ",[ipadrr] AS Адрес\n"
                         ",[isServer]\n"
                         "FROM [dbo].[Servers]", db->getDatabase());

    ui->tableView->setModel(queryModel);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(4, true);
    ui->tableView->setColumnHidden(6, true);

    updateRowIcons();
    ui->tableView->model()->setHeaderData(1, Qt::Orientation::Horizontal, "");
    ui->tableView->resizeColumnsToContents();

    disableToolBar();
    ui->btnAdd->setEnabled(true);
    ui->btnEdit->setEnabled(true);
    ui->btnDelete->setEnabled(true);
}

void MainWindow::updateRowIcons(){
    qDebug() << __FUNCTION__;
   for (int i = 0; i < ui->tableView->model()->rowCount(); ++i) {
       QModelIndex index = ui->tableView->model()->index(i, 1);
       QLabel *lbl_item = new QLabel();
        if(ui->tableView->model()->index(i, 6).data().toInt() > 0){
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/server.ico")));// *ui->my_label->pixmap());
        }else
            lbl_item ->setPixmap(QPixmap::fromImage(QImage(":/img/desktop.ico")));
        lbl_item ->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableView->setIndexWidget(index, lbl_item);
    }
}

void MainWindow::connectToWsServer()
{
    qDebug() << __FUNCTION__;

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
    }
    if(!m_client)
        return;

    if(m_client->isStarted())
        return;

    if(_sett->launch_mode() == mixed){
        QSqlQuery result("select [host], [port] from dbo.WSConf;", db->getDatabase());
        if(result.lastError().type() == QSqlError::NoError){
            while (result.next()){
                QString _host = result.value(0).toString().trimmed();
                int _port = result.value(1).toInt();
                m_client->setHost(_host);
                m_client->setPort(_port);
                m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
                break;
            }            
        }
        //getDataContainersList();
        //getDataCertificatesList();
    }else{
        QString _host = m_client->options()[bConfFieldsWrapper::ServerHost].toString();
        int _port = m_client->options()[bConfFieldsWrapper::ServerPort].toInt();
        m_client->setHost(_host);
        m_client->setPort(_port);
        m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
    }


}

//bool MainWindow::insertSqlTableRow(const QString &table, QMap<QString, QVariant>& vals, const QString& ref)
//{

//    db.exec("USE arcirk;");

//    QString query = queryText(table, vals, sqlCommand::sqlInsert, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//bool MainWindow::updateSqlTableRow(const QString &table, QMap<QString, QVariant> vals, const QString& ref)
//{

//    QString query = queryText(table, vals, sqlCommand::sqlUpdate, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//bool MainWindow::deleteSqlTableRow(const QString &table, const QString& ref)
//{
//    QMap<QString, QVariant> vals;
//    QString query = queryText(table, vals, sqlCommand::sqlDelete, ref);

//    QSqlQuery q = db.exec(query);

//    bool result = q.lastError().type() == QSqlError::NoError;

//    if(!result){
//        qCritical() << __FUNCTION__ << qPrintable(query);
//        qCritical() << q.lastError().text();
//    }

//    return result;
//}

//QString MainWindow::queryText(const QString& table, QMap<QString, QVariant>& values,
//                                sqlCommand command, const QString& not_ref)
//{
//    QString query;


//    if(command == sqlCommand::sqlInsert){

//        QString into = "\n(";
//        QString values_ = "\n(";
//        query = "INSERT INTO ";
//        query.append("dbo." + table);
//        for (auto iter = values.begin(); iter != values.end(); iter++) {
//            into.append(iter.key());
//            if (iter != --values.end())
//                into.append(",\n");

//            if (iter.value().typeId() == QMetaType::QString){
//                QString value = iter.value().toString();
//                values_.append("'" + value + "'");
//                if (iter != --values.end())
//                    values_.append(",\n");
//            }else if (iter.value().typeId() == QMetaType::Int){
//                int res = iter.value().toInt();
//                QString value = QString::number(res);
//                values_.append("'" + value + "'");
//                if (iter != --values.end())
//                    values_.append(",\n");
//            }
//        }
//        into.append("\n)");
//        values_.append("\n)");
//        query.append(into);
//        query.append("\nVALUES\n");
//        query.append(values_);

//        if(!not_ref.isEmpty()){
//            QString query_ = QString("IF NOT EXISTS \n"
//                                     "    (   SELECT  [Ref]\n"
//                                     "        FROM    dbo.%1 \n"
//                                     "        WHERE   [Ref]='%2' \n"
//                                     "    )\n"
//                                     "BEGIN\n").arg(table, not_ref);
//            query_.append(query);
//            query_.append("\nEND");
//            query = query_;
//        }

//        query.append(";");

//    }else if(command == sqlCommand::sqlUpdate){
//        query = "UPDATE dbo." + table;
//        QString _set = "\n SET ";
//        QString _where = QString("\n WHERE Ref = '%1'").arg(not_ref);
//        for (auto iter = values.begin(); iter != values.end(); iter++) {
//            _set.append(iter.key());
//            if (iter.value().typeId() == QMetaType::QString){
//                QString value = iter.value().toString();
//                _set.append(" = '" + value + "'");
//                if (iter != --values.end())
//                    _set.append(",\n");
//            }else if (iter.value().typeId() == QMetaType::Int){
//                int res = iter.value().toInt();
//                QString value = QString::number(res);
//                _set.append(" = '" + value + "'");
//                if (iter != --values.end())
//                    _set.append(",\n");
//            }

//        }
//        _set.append("\n");
//        _where.append("\n");
//        query.append(_set);
//        query.append(_where + ";");
//    }else if(command == sqlCommand::sqlDelete){
//        query = QString("DELETE FROM [dbo].[Servers]\n"
//                "WHERE Ref = '%1'").arg(not_ref);
//    }

//    return query;

//}

void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{


    qDebug() << __FUNCTION__;
    QString key = item->data(0, Qt::UserRole).toString();

    inVisibleToolBars();

    if(item->childCount() > 0 && key != SqlUsers){
        loadItemChilds(item);
        disableToolBar();       
        toolBarSetVisible(ui->wToolBarMain, true);
    }else{   
        if(key == currentUserRegistry){
           toolBarSetVisible(ui->wToolBarCurrentUser, true);
           treeSetCurrentContainers("REGISTRY");
           ui->btnCurrentCopyToRegistry->setEnabled(true);
           ui->btnCurrentCopyToSql->setEnabled(true);
           ui->btnCurrentUserAdd->setEnabled(false);
        }else if(key == currentUserDivace){
           toolBarSetVisible(ui->wToolBarCurrentUser, true);
           treeSetCurrentContainers("!REGISTRY");
           ui->btnCurrentCopyToRegistry->setEnabled(true);
           ui->btnCurrentCopyToSql->setEnabled(true);
           ui->btnCurrentUserAdd->setEnabled(false);
        }else if(key == SqlContainers){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlContainers();
        }else if(key == WsActiveUsers){
            toolBarSetVisible(ui->wToolBarAU, true);
            treeSetOnlineWsUsers();
        }else if(key == SqlCertificates){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlCertificates();
        }else if(key == SqlUsers){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlUsers();
        }else if(key == currentUserCertificates){
            toolBarSetVisible(ui->wToolBarCurrentUser, true);
            treeSetFromCurrentUserCerts();
            ui->btnCurrentCopyToRegistry->setEnabled(false);
            ui->btnCurrentCopyToSql->setEnabled(false);
            ui->btnCurrentUserAdd->setEnabled(true);
            ui->btnAdd->setEnabled(true);
        }
    }
//        return;

//        QString itemText = item->text(0);
//        if(itemText == "Реестр"){
////            ui->btnAdd->setEnabled(true);

////            if(item->parent()->text(0).compare("Текущий пользователь")){
////                getAvailableContainers(currentUser);
////            }else{

////            }
//        }else if(itemText == "Контейнеры"){
//            toolBarSetVisible(ui->wToolBarAU, false);
//            toolBarSetVisible(ui->wToolBarMain, false);
//            toolBarSetVisible(ui->wToolbarContainers, true);
//            //getDataContainersList();
//        }else if(itemText == "Пользователи"){
//            toolBarSetVisible(ui->wToolBarAU, false);
//            toolBarSetVisible(ui->wToolBarMain, true);
//            toolBarSetVisible(ui->wToolbarContainers, false);
//            LoadUsersList();
//        }else if(itemText == "Компьютеры"){
//            toolBarSetVisible(ui->wToolBarAU, false);
//            toolBarSetVisible(ui->wToolBarMain, true);
//            toolBarSetVisible(ui->wToolbarContainers, false);
//            loadCimputers();
//        }else if(itemText == "Активные пользователи"){
//            toolBarSetVisible(ui->wToolBarAU, true);
//            toolBarSetVisible(ui->wToolBarMain, false);
//            toolBarSetVisible(ui->wToolbarContainers, false);
//            disableToolBar();
//            loadOnlineUsers();
//        }else if(itemText == "Сертификаты"){
//            loadCertList();
//            toolBarSetVisible(ui->wToolBarAU, false);
//            toolBarSetVisible(ui->wToolBarMain, true);
//            toolBarSetVisible(ui->wToolbarContainers, false);
//        }else{
//            toolBarSetVisible(ui->wToolBarAU, false);
//            toolBarSetVisible(ui->wToolBarMain, true);
//            toolBarSetVisible(ui->wToolbarContainers, false);
//            disableToolBar();
//            ui->tableView->setModel(nullptr);
//        }
//    }
//    QString itemText = item->text(0);
//    if(itemText == "root"){
//        createRootList();
//        enableToolbar(false);
//    }else if(itemText == "Контейнеры"){
//        createContainersList();
//        enableToolbar(true);
//    }else if(itemText == "Сертификаты"){
//        createCertList();
//        enableToolbar(true);
//    }else if(itemText == "Пользователи"){
//        createUsersList();
//        enableToolbar(true);
//    }else
//        enableToolbar(true);

}

void MainWindow::connectToDatabase(Settings *sett, const QString &pwd)
{

    qDebug() << __FUNCTION__;

    QString host = sett->server();
    QString database = "arcirk";
    QString userName = sett->user();
    QString password = pwd;

    db->setSqlUser(userName);
    db->setSqlPwd(password);
    db->setHost(host);
    db->setDatabaseName(database);
    db->connect();


    if(sett->launch_mode() == mixed)
        connectToWsServer();

    sett->save();

    if (!isDbOpen()){
        QMessageBox::critical(this, "Ошибка", QString("Ошибка подключения к базе данных: %2").arg(db->lastError()));
    }else{
        getDataContainersList();
    }

    emit whenDataIsLoaded();
}

void MainWindow::on_mnuConnect_triggered()
{
    qDebug() << __FUNCTION__;
    QMap<QString, QVariant> clientSett;
    clientSett.insert("ServerHost", m_client->options()[bConfFieldsWrapper::ServerHost].toString());
    clientSett.insert("ServerPort", m_client->options()[bConfFieldsWrapper::ServerPort].toInt());
    clientSett.insert("ServerUser", m_client->options()[bConfFieldsWrapper::User].toString());
    clientSett.insert("Password", "***");
    clientSett.insert("pwdEdit", false);

    auto dlg = new DialogConnection(_sett, clientSett, this);

    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){        
        if(db->isOpen())
            db->close();
        if(m_client->isStarted())
            m_client->close();

        if(_sett->launch_mode() == mixed){
            connectToDatabase(_sett, dlg->pwd());
        }else{
            QString pass;
            QString pwd = clientSett["Password"].toString();
            QString usr = clientSett["ServerUser"].toString();
            if(clientSett["pwdEdit"].toBool()){
                QString  hash = bWebSocket::generateHash(usr, pwd);
                m_client->options()[bConfFieldsWrapper::Hash] = hash;
            }
            m_client->options()[bConfFieldsWrapper::User] = usr;
            m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
            m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
            m_client->options().save();
        }

        _sett->save();
        connectToWsServer();

        isDbOpen();
    }

}



void MainWindow::on_btnAdd_clicked()
{
//    qDebug() << __FUNCTION__;


//    auto tree = ui->treeWidget;
//    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

//    if(node == "SqlCertificates" || node == "currentUserCertificates"){

//        QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
//                                                     QDir::homePath(),
//                                                     "Файл сертификата (*.cer)");
//        if(fileName != ""){
//            QFile file(fileName);
//            if(!file.open(QIODevice::ReadOnly))
//                return;
//            bool removeTmp = false;
//            if(isCyrillic(fileName)){
//                //не могу победить кодировку
//                //
//                QString uuid = QUuid::createUuid().toString();
//                uuid = uuid.mid(1, uuid.length() - 2);
//                QString newPath = QCoreApplication::applicationDirPath() + QDir::separator() + uuid + ".tmp";
//                file.copy(newPath);
//                fileName = newPath;
//                removeTmp = true;
//                if(!QFile::exists(fileName)){
//                    file.close();
//                    return;
//                }
//            }

//            file.close();

////            ByteArray data;
////            Base64Converter::readFile(fileName.toStdString(), data);
////            if(data.size() == 0)
////                return;
//            auto cert = Certificate(this);
//            //cert.fromData(data);
//            cert.fromFile(fileName, removeTmp);
//            if(cert.isValid()){
//                if(node == "SqlCertificates"){
//                    auto bindQuery = cert.getSqlQueryObject(QBSqlCommand::QSqlInsert);
//                    QString result = bindQuery.to_json();
//                    if(_sett->launch_mode() == mixed){
//                        auto bindQuery1 = QBSqlQuery();
//                        bindQuery1.fromJson(result);
//                        QSqlQuery sql = bindQuery1.query(db->getDatabase());
//                        sql.exec();
//                        if(sql.lastError().type() != QSqlError::NoError){
//                            qDebug() << __FUNCTION__ << sql.lastError().text();
//                        }else{
//                            QMessageBox::information(this, "Копирование на сервер", "Сертификат успешно скопирован на сервер!");
//                            getDataContainersList();
//                        }
//                    }else{
//                        if(m_client->isStarted()){
//                            auto doc = QJsonDocument();
//                            auto objMain = QJsonObject();
//                            objMain.insert("query", result);
//                            objMain.insert("id_command", "insertCertificateToData");
//                            doc.setObject(objMain);
//                            QString param = doc.toJson();
//                            m_client->sendCommand("exec_query_qt", "", param);
//                        }
//                    }

//                }else if(node == "currentUserCertificates"){

//                }
//            }

//        }

//    }

//    return;

//    auto reg = Registry();
//    QString admin = reg.currentWindowsName();
//    qDebug() << admin;

//    auto item = ui->treeWidget->currentItem();
//    if(item){
//        if(item->text(0) == "Контейнеры"){

//            QStringList keys = reg.containersNodes();
//             foreach(const QString& key, keys){
//                qDebug() << key;
//            }
//        }
//    }else
//        return;

//    auto dlg = new DialogSelectDevice(this);
//    dlg->setModal(true);
//    dlg->exec();

//    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог"),
//                                                 QDir::homePath(),
//                                                 QFileDialog::ShowDirsOnly
//                                                 | QFileDialog::DontResolveSymlinks);
//    if(dir != ""){
//        QFile file(dir + QDir::separator() + "name.key");
//        if(file.open(QIODevice::ReadOnly)){
//            QString data = QString::fromLocal8Bit(file.readAll());
//            qDebug() << data;
//            if(data.compare("0/\u0016-")){
//                data.replace("0/\u0016-", "");
//                auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));

//                if(result == QMessageBox::Yes){

//                }
//            }else
//               QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//        }else{
//            QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//        }
//    }

    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    if(currentNode == "Реестр"){
        setKeysToRegistry();
    }else if(currentNode == "Пользователи"){
//        auto reg = srtmnr::Registry();
//        QStringList users = reg.localUsers();
//        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
//        dlg->setModal(true);
//        dlg->exec();
//        if(dlg->result() == QDialog::Accepted){
//            QString userName = dlg->dialogResult()[0];
//        }
    }else if(currentNode == "Компьютеры"){
        auto dlg = new DialogComputer(this);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            auto result = dlg->computer();
            if(result["Ref"].toString().isEmpty()){
                QString ref =  QUuid::createUuid().toString();
                ref = ref.mid(1, ref.length() - 2);
                result["Ref"] = ref;
                bool r = db->insertSqlTableRow("Servers", result);
                //auto model = (QSqlQueryModel*)ui->tableView->model();
                //model->setQuery(model->query().lastQuery());
                //UpdateRowIcons();
                //ui->tableView->resizeColumnsToContents();
                 loadCimputers();
                if(!r)
                    qCritical() << __FUNCTION__ << "Ошибка добавления новой строки в таблицу 'Servers'!";
            }
        }
    }else if(currentNode == "Сертификаты"){

        QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                     QDir::homePath(),
                                                     "Файл сертификата (*.cer)");
        if(fileName != ""){
            QFile file(fileName);
            QFileInfo fileInfo(file.fileName());
            if(file.open(QIODevice::ReadOnly)){
                QByteArray data = file.readAll();
                QUuid _uuid = QUuid::createUuid();
                QString uuid = _uuid.toString().mid(1, 36);
                QSqlQuery sql = QSqlQuery(db->getDatabase());
                sql.prepare("INSERT INTO [dbo].[Certificates] ([FirstField] ,[SecondField] ,[Ref] ,[data]) "
                              "VALUES (?,?,?,?)");
                sql.addBindValue(fileInfo.baseName());
                sql.addBindValue(file.fileName());
                sql.addBindValue(uuid);
                sql.addBindValue(data);
                auto r = sql.exec();
                if(!r){
                    qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Certificates'!";
                    qCritical() << __FUNCTION__ << db->lastError();
                }
                else
                    loadCertList();
            }

//            if(file.open(QIODevice::ReadOnly)){
//                QString data = QString::fromLocal8Bit(file.readAll());
//                qDebug() << data;
//                if(data.compare("0/\u0016-")){
//                    data.replace("0/\u0016-", "");
//                    auto result =  QMessageBox::question(this, "Импорт контейнера", QString("Найден контейнер: \n%1 \nИмпортировать?").arg(data));

//                    if(result == QMessageBox::Yes){

//                    }
//                }else
//                   QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//            }else{
//                QMessageBox::critical(this, "Ошибка", "В выбранном каталоге контейнер закрытого ключа не найден!");
//            }
        }
    }

}

void MainWindow::on_btnToDatabase_clicked()
{
//    if(!isDbOpen())
//        return;

//    auto table = ui->tableView;
//    int row = table->currentIndex().row();

//    if(row < 0){
//        return;
//    }

//    auto treeItem = ui->treeWidget->currentItem();
//    if(!treeItem){
//        return;
//    }

//    QString currentNode = treeItem->text(0);
//    QString currentKeyName = table->model()->index(row, 1).data().toString();

//    if(currentNode == "Реестр"){
//        if(treeItem->parent()->text(0).compare("Текущий пользователь")){
//            if(!isContainerExists(currentKeyName)){
//                auto result = QMessageBox::question(this, "Экспорт контейнера", "Экспортировать контейнер на сервер?");
//                if(result == QMessageBox::Yes){
//                    KeysContainer cnt = KeysContainer(currentUser->sid(), currentKeyName, db, this);
//                    bool result = cnt.toDataBase();
//                    if(!result)
//                        QMessageBox::critical(this, "Ошибка", "Ошибка экспорта контейнера!");
//                    else
//                        QMessageBox::information(this, "Экспорт", "Контейнер успешно экспортирован на сервер!");
//                }
//            }else
//                QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже есть на сервере!").arg(currentKeyName));
//        }else{

//        }
//    }
//    qDebug() << __FUNCTION__;
}

void MainWindow::on_btnDelete_clicked()
{
    if(!isDbOpen())
        return;

    auto table = ui->tableView;

    if(!table->currentIndex().isValid())
        return;

    int row = table->currentIndex().row();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->data(0, Qt::UserRole).toString();

    if(currentNode == SqlContainers){
        QString ref = table->model()->index(row, 1).data().toString();
        auto result = QMessageBox::question(this, "Удаление контейнера", "Удалить контейнер?");
        if(result == QMessageBox::Yes){
//            bool result = srtmnr::Registry::deleteContainer(currentUser, currentKeyName);
//            if(!result)
//                QMessageBox::critical(this, "Ошибка", "Ошибка удаления контейнера!");
//            else{
//                QMessageBox::information(this, "Удаление контейнера закрытого ключа", "Контейнер успешно удален из реестра!");
//                table->model()->removeRow(row);
//                currentUser->setContainers(srtmnr::Registry::currentUserContainers(currentUser->sid()));
//            }
        }

    }


        //else{

//            if(currentNode == "Реестр"){

//            }else if(currentNode == "Пользователи"){
//        //        auto reg = Registry();
//        //        QStringList users = reg.localUsers();
//        //        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
//        //        dlg->setModal(true);
//        //        dlg->exec();
//        //        if(dlg->result() == QDialog::Accepted){
//        //            QString userName = dlg->dialogResult();
//        //        }
//            }
//        }
////    }else if(currentNode == "Компьютеры"){
////        //
////        auto result = QMessageBox::question(this, "Удаление строки", "Удалить выбранную строку?");
////        if(result == QMessageBox::Yes){
////            bool r = db->deleteSqlTableRow("Servers", ui->tableView->model()->index(row, 3).data().toString());
////            if(!r)
////                QMessageBox::critical(this, "Ошибка", "Ошибка удаления строки!");
////            else
////            {
//////                auto model = (QSqlQueryModel*)ui->tableView->model();
//////                model->setQuery(model->query().lastQuery());
//////                UpdateRowIcons();
////                loadCimputers();
////                ui->tableView->resizeColumnsToContents();
////            }
////        }

////    }
    qDebug() << __FUNCTION__;
}

void MainWindow::on_btnInstallToUser_clicked()
{
    if(!m_client->isStarted()){
        QMessageBox::critical(this, "Ошибка", "Сервер обмена не доступен!");
        return;
    }
    if(!db->isOpen()){
        QMessageBox::critical(this, "Ошибка", "Сервер баз данных не доступен!!");
        return;
    }
    auto model = new QSqlQueryModel(this);
    model->setQuery("SELECT [FirstField] AS Имя\n"
                    ",[Ref] AS Ссылка\n"
                    ",[host] AS Host\n"
                    ",[sid] AS SID\n"
                    ",[uuid] AS ID\n"
                    "FROM [dbo].[CertUsers]", db->getDatabase());

    auto dlg = new DialogSelectFromDataBase(model, "Выбор ползователя", this);
    dlg->setModal(true);
    dlg->exec();
    if(dlg->result() == QDialog::Accepted){

//        QJsonObject obj = QJsonObject();
//        obj.insert("uuid_agent", m_client->getUuidSession());
//        obj.insert("uuid_client", itr.key());
//        obj.insert("command", "clientShow");


//        QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
//        //qDebug() << param;

//        if(client->started()){
//            client->send_command("command_to_qt_client", "", param.toStdString());
//        }

    }
    qDebug() << __FUNCTION__;
}

void MainWindow::on_mnuOptions_triggered()
{
    auto optDlg = new DialogOptions(this);
    optDlg->setModal(true);
    optDlg->exec();

    if(optDlg->result() == QDialog::Accepted){
        //auto opt = ClientSettings("conf_qt_cert_manager.json");

    }

    qDebug() << __FUNCTION__;
}

void MainWindow::onConnectionSuccess()
{
    qDebug() << __FUNCTION__;

    if(m_client->isStarted())
    {
        QString status;
        if(db->isOpen()){
            status = "SQL Server: " + _sett->server() + "  ";
        }
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
        infoBar->setText(status);

        QJsonDocument doc = QJsonDocument();
        QJsonObject obj = QJsonObject();
        obj.insert("table", true);
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("get_active_users", "", param);

        if(_sett->launch_mode() != mixed){
            getDataContainersList();
        }

        emit endInitConnection();
    }

}

void MainWindow::onCloseConnection()
{
    QString status;
    if(db->isOpen()){
        status = "SQL Server: " + _sett->server();
    }
    infoBar->setText(status);
    qDebug() << __FUNCTION__;
}

void MainWindow::onConnectedStatusChanged(bool status)
{
    qDebug() << __FUNCTION__;
    if(currentUser)
        currentUser->setOnline(status);
}

void MainWindow::onClientJoinEx(const QString& resp, const QString& ip_address, const QString& host_name, const QString& app_name, const QString& user_name)
{
    qDebug() << __FUNCTION__; // << qPrintable(resp);
//  qDebug() << __FUNCTION__ << ip_address << " " << host_name << " " << app_name;
    //MainWindow::onClientJoinEx {"name": "admin", "uuid": "d1ca0bc6-b6cb-4b90-941c-85fb9faf70a9", "uuid_user": "e7429c10-8070-40da-ae2e-dea1cb9ae371", "active": true}
    //MainWindow::onClientJoinEx "192.168.10.14"   "VMBUHSVR"   "qt_cert_manager"

//    auto doc = QJsonDocument::fromJson(resp.toUtf8());
//    auto obj = doc.object();
//    QUuid uuid = QUuid::fromString(obj.value("uuid_user").toString());
    //QString name = obj.value("user_name").toString();

//    auto itr = m_actUsers.find(uuid);
//    if(itr != m_actUsers.end()){
//        itr.value()->setOnline(true);
//    }else{
//        if(!currentUser)
//            return;
//        CertUser * usr = nullptr;
//        if(!currentUser->thisIsTheUser(user_name, host_name)){
//            usr = new CertUser(this);
//            usr->setName(user_name);
//            usr->setDomain(host_name);
//        }
//        else {
//            usr = currentUser;
//            auto item = findTreeItem("currentUser");
//            if(item){
//                item->setText(0, QString("Текущий пользователь (%1)").arg(currentUser->name()) );
//                if(item->childCount() == 0){
//                    auto Root = addTreeNode("Доступные контейнеры", currentUserAvailableContainers, ":/img/key16.png");
//                    item->addChild(Root);
//                    auto certs = addTreeNode("Установленные сертификаты", currentUserCertificates, ":/img/cert.png");
//                    item->addChild(certs);
//                    auto reg = addTreeNode("Реестр", "currentUserRegistry", ":/img/registry16.png");
//                    Root->addChild(reg);
//                    auto dev = addTreeNode("Устройства", "currentUserDivace", ":/img/Card_Reader_16.ico");
//                    Root->addChild(dev);

//                    getAvailableContainers(usr);
//                }
//            }
//        }
//        usr->setOnline(true);
//        usr->setUuid(uuid);
//        //m_actUsers.insert(uuid, usr);
//    }

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto objResp = doc.object();
    QString uuid = objResp.value("uuid").toString();
    if(uuid == m_client->getUuidSession()){
        auto item = findTreeItem("currentUser");
        if(item){
            item->setText(0, QString("Текущий пользователь (%1)").arg(currentUser->name()) );
            if(item->childCount() == 0){
                auto Root = addTreeNode("Доступные контейнеры", currentUserAvailableContainers, ":/img/key16.png");
                item->addChild(Root);
                auto certs = addTreeNode("Установленные сертификаты", currentUserCertificates, ":/img/cert.png");
                item->addChild(certs);
                auto reg = addTreeNode("Реестр", "currentUserRegistry", ":/img/registry16.png");
                Root->addChild(reg);
                auto dev = addTreeNode("Устройства", "currentUserDivace", ":/img/Card_Reader_16.ico");
                Root->addChild(dev);

                getAvailableContainers(currentUser);
            }
        }
    }else{
        QString uuid_user = objResp.value("uuid_user").toString();
        QString name = objResp.value("name").toString();

        auto obj = QJsonObject();
        obj.insert("Empty", "");
        obj.insert("uuid", uuid);
        obj.insert("name", name);
        obj.insert("user_uuid", uuid_user);
        obj.insert("app_name", app_name);
        obj.insert("user_name", name);
        obj.insert("ip_address", ip_address);
        obj.insert("host_name", host_name);

        modelWsUsers->addRow(obj);
        QString findKey = name + host_name;
        modelWsUsers->setRowKey(modelWsUsers->rowCount() - 1, findKey);
        qDebug() << __FUNCTION__ << qPrintable(resp);

        updateCertUsersOnlineStstus();
    }
//    auto treeItem = ui->treeWidget->currentItem();
//    if(!treeItem){
//        return;
//    }

//    QString currentNode = treeItem->text(0);

//    if(currentNode == "Активные пользователи")
//    {
//        loadOnlineUsers();
//    }

}

void MainWindow::onClientLeave(const QString &resp)
{
    qDebug() << __FUNCTION__ << qPrintable(resp);

//    auto doc = QJsonDocument::fromJson(resp.toUtf8());
//    auto obj = doc.object();
//    QUuid uuid = QUuid::fromString(obj.value("uuid_user").toString());
//    QString name = obj.value("name").toString();

//    auto itr = m_actUsers.find(uuid);
//    if(itr != m_actUsers.end()){
//        itr.value()->setOnline(false);
//    }
//    auto treeItem = ui->treeWidget->currentItem();
//    if(!treeItem){
//        return;
//    }
//    QString currentNode = treeItem->text(0);

//    if(currentNode == "Активные пользователи")
//    {
//        loadOnlineUsers();
//    }

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();
    QString uuid = obj.value("uuid").toString();

    int ind = modelWsUsers->getColumnIndex("uuid");
    auto item = findInTable(modelWsUsers, uuid, ind, false);
    if(item.isValid()){
        modelWsUsers->removeRow(item.row());
    }

    updateCertUsersOnlineStstus();
}

void MainWindow::onMessageReceived(const QString &msg, const QString &uuid, const QString &recipient, const QString &recipientName)
{
    qDebug() << __FUNCTION__;
}

void MainWindow::onDisplayError(const QString &what, const QString &err)
{
    qCritical() << __FUNCTION__ << what << ": " << err ;
    if(err == "Отказано в доступе"){
        emit endInitConnection();
    }

}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    QModelIndex _index = ui->tableView->model()->index(row, 1);
    QString key = _index.data(Qt::UserRole).toString();
    if(!key.isEmpty()){
        auto item = findTreeItem(key);
        if(item){
            item->parent()->setExpanded(true);
            ui->treeWidget->setCurrentItem(item);
            emit ui->treeWidget->itemClicked(item, 0);
        }
    }
////    auto item = ui->tableView-item(row, column);
////    if(item){
//        QString itemText = index.data().toString();
//        QTreeWidgetItem * child = nullptr;

//        if(itemText == "Контейнеры"){
//            child = ui->treeWidget->topLevelItem(0)->child(0);
//        }else if(itemText == "Сертификаты"){
//            child = ui->treeWidget->topLevelItem(0)->child(1);
//        }else if(itemText == "Пользователи"){
//            child = ui->treeWidget->topLevelItem(0)->child(2);
//        }

//        if(child){
//            ui->treeWidget->setCurrentItem(child, 0);
//            emit ui->treeWidget->itemClicked(child, 0);
//        }

//    //}
}


void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
//    auto treeItem = ui->treeWidget->currentItem();
//    if(!treeItem){
//        return;
//    }

//    QString currentNode = treeItem->text(0);

//    if(currentNode == "Реестр"){
//        ui->btnAdd->setEnabled(false);
//        ui->btnEdit->setEnabled(false);
//        ui->btnDelete->setEnabled(true);
//        ui->btnImportFromDatabase->setEnabled(true);
//        ui->btnToDatabase->setEnabled(true);
//    }else if(currentNode == "Компьютеры"){
//        ui->btnAdd->setEnabled(true);
//        ui->btnEdit->setEnabled(true);
//        ui->btnDelete->setEnabled(true);
//    }else{
//        disableToolBar();
//    }

      qDebug() <<__FUNCTION__;
}


void MainWindow::on_btnEdit_clicked()
{
    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == "Реестр"){

    }else if(currentNode == "Пользователи"){
//        auto reg = Registry();
//        QStringList users = reg.localUsers();
//        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
//        dlg->setModal(true);
//        dlg->exec();
//        if(dlg->result() == QDialog::Accepted){
//            QString userName = dlg->dialogResult();
//        }
    }else if(currentNode == "Компьютеры"){
        auto dlg = new DialogComputer(this);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", ui->tableView->model()->index(row, 2).data().toString());
        _row.insert("SecondField", ui->tableView->model()->index(row, 3).data().toString());
        _row.insert("Ref", ui->tableView->model()->index(row, 4).data().toString());
        _row.insert("ipadrr", ui->tableView->model()->index(row, 5).data().toString());
        _row.insert("isServer", ui->tableView->model()->index(row, 6).data().toInt());
        dlg->setComputer(_row);
        dlg->setModal(true);
        dlg->exec();
        if(dlg->result() == QDialog::Accepted){
            auto result = dlg->computer();
            if(!result["Ref"].toString().isEmpty()){
                bool r = db->updateSqlTableRow("Servers", result, result["Ref"].toString());
//                auto model = (QSqlQueryModel*)ui->tableView->model();
//                model->setQuery(model->query().lastQuery());
//                UpdateRowIcons();
                 loadCimputers();
               // ui->tableView->resizeColumnsToContents();
                if(!r)
                    qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";
            }
        }
    }
}

void MainWindow::onGetActiveUsers(const QString& resp){

    auto onlineItem = findTreeItem("WsActiveUsers");
    if(!onlineItem){
        auto root = findTreeItem("WsServer");
        if(root){
            onlineItem = addTreeNode("Активные пользователи", "WsActiveUsers", ":/img/activeUesers16.png");
            root->addChild(onlineItem);
        }
    }
    qDebug() << __FUNCTION__;// << qPrintable(resp);
    modelWsUsers->setJsonText(resp);
    modelWsUsers->reset();
    int colHost = modelWsUsers->getColumnIndex("host_name");
    int colName = modelWsUsers->getColumnIndex("user_name");
    for (int i = 0; i < modelWsUsers->rowCount(); ++i) {
        auto user = modelWsUsers->index(i, colName).data(Qt::UserRole + colName).toString();
        auto host = modelWsUsers->index(i, colHost).data(Qt::UserRole + colHost).toString();
        modelWsUsers->setRowKey(i, user+host);
        if(isCertUserExists(user, host)){
            if(!currentUser->thisIsTheUser(user, host)){
                m_queue.append(qMakePair(user,host));
            }
        }
    }

    if(!currentUser)
        return;

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();

    auto rows = obj.value("rows").toArray();

    for (auto row = rows.begin(); row != rows.end(); ++row){
        auto item = row->toObject();
        QString name = item.value("user_name").toString();
        QUuid user_uuid = QUuid::fromString(item.value("user_uuid").toString());
        QString host = item.value("host_name").toString();
        CertUser * user;
        if(!currentUser->thisIsTheUser(name, host)){
            user = new CertUser(this);
            user->setName(name);
            user->setDomain(host);
        }
        else{
             user = currentUser;
             if(_sett->launch_mode() == mixed){
                 if(db->isOpen()){
                     QString str = QString("select [Ref], [sid], [uuid] from [arcirk].[dbo].[CertUsers] where [FirstField] = '%1' AND [host] = '%2'").arg(name, host);
                     QSqlQuery query = QSqlQuery(str, db->getDatabase());
                     QString ref;
                     QString sid;
                     QString uuid;
                     QMap<QString, QVariant> _row;

                     while (query.next()) {
                         ref = query.value(0).toString();
                         sid = query.value(1).toString();
                         uuid = query.value(2).toString();
                         if(sid.isEmpty())
                             _row.insert("sid", currentUser->sid());
                         if(uuid.isEmpty())
                             _row.insert("uuid", user_uuid.toString());
                         break;
                     }
                     if(!ref.isEmpty() && _row.size() > 0){
                         currentUser->setRef(ref);
                         //update sid
                         db->updateSqlTableRow("CertUsers", _row, ref);
                     }
                 }
             }
        }
        user->setUuid(user_uuid);
        user->setOnline(true);
    }

    if(m_queue.size() > 0){
        emit startGetCertUsersData();
    }

}

void MainWindow::onParseCommand(const QVariant &result, int command)
{
    if(command == CmdCommand::echoUserName){
        currentUser->setName(result.toString());
        currentUser->treeItem()->setText(0, QString("Текущий пользователь (%1)").arg(result.toString()));
        //terminal->send(QString("wmic useraccount where name='%1' get sid\n").arg(result), CommandLine::cmdCommand::wmicGetSID);
    }else if(command == CmdCommand::wmicGetSID){
        currentUser->setSid(result.toString());
        //get cert
        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);

    }else if(command == CmdCommand::csptestGetConteiners){
        QString res = result.toString();
        res.replace("\r", "");
        csptestCurrentUserGetContainers(result.toString());
#ifdef _WINDOWS
        if(currentUser->sid().isEmpty())
            terminal->send("WHOAMI /USER\n", CmdCommand::wmicGetSID);
#endif
    }else if(command == CmdCommand::csptestContainerFnfo){
        updateContainerInfoOnData(result.toString());
    }else if(command == CmdCommand::csptestContainerCopy){
        QMessageBox::information(this, "Копироване контейнера", "Контейнер успешно скопирован!");
        getAvailableContainers(currentUser);
    }else if(command == CmdCommand::csptestContainerDelete){
        QMessageBox::information(this, "Удаление контейнера", "Контейнер успешно удален!");
        getAvailableContainers(currentUser);
    }else if(command == CmdCommand::csptestGetCertificates){

        auto doc = QJsonDocument::fromJson(result.toString().toUtf8());
        auto arr = doc.array();

        currentUser->certificates().clear();

        for (auto itr = arr.begin(); itr != arr.end(); ++itr) {
            auto obj = itr->toObject();
            auto cert = new Certificate(this);
            cert->setSourceObject(obj);
            currentUser->certificates().insert(cert->serial(), cert);
        }

        resetCurrentUserCertModel();

    }
}

void MainWindow::onCommandError(const QString &result, int command)
{
    //qCritical() << __FUNCTION__ << "error: " <<  result;
    QMessageBox::critical(this, "Ошибка", result);
    ui->txtTerminal->setText(ui->txtTerminal->toPlainText() +  "\nerror:"  + result);
}

void MainWindow::onWsGetAvailableContainers(const QString &recipient)
{
    qDebug() << __FUNCTION__;
    if(currentUser->containers().size() > 0){
        sendToRecipient(recipient, "available_containers", currentUser->containers().join("\n"), true);
    }else{
        currentRecipient = recipient;
        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
    }
}

void MainWindow::onWsCommandToClient(const QString &recipient, const QString &command, const QString &message)
{
    qDebug() << __FUNCTION__;
    //if(command == AvailableContainers){
        qDebug() <<  __FUNCTION__ << qPrintable(message);
    //}
}

void MainWindow::onWsMplClientFormLoaded(const QString &resp)
{
    qDebug() << __FUNCTION__ << resp;
}

void MainWindow::onWsExecQuery(const QString &result)
{
    qDebug() << __FUNCTION__;// << result;

    auto doc = QJsonDocument::fromJson(result.toUtf8());
    auto obj = doc.object();
    if(doc.isEmpty())
        return;

    QString id_command = obj.value("id_command").toString();
    if(id_command == "DataContainersList" || id_command == "DataCertificatesList" ){
        resetTableJsonModel(obj, id_command);
    }else if(id_command == insertContainerToData){
        qDebug() << __FUNCTION__ << "Контейнер успешно импортирован в на сервер!";
        getDataContainersList();
    }else if(id_command == deleteContainerFromData){
        qDebug() << __FUNCTION__ << "Контейнер успешно удален с сервера!";
        getDataContainersList();
    }else if(id_command == DataUsersList){\
        resetTableJsonModel(obj, id_command);
    }else if(id_command == "get_data"){
        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString json = QByteArray::fromBase64(base64.toUtf8());
        QString run_on_return = obj.value("run_on_return").toString();
        if(!run_on_return.isEmpty()){
            onGetDataFromDatabase(json, run_on_return);
        }
    }else if(id_command == insertCertificateToData){
        qDebug() << __FUNCTION__ << "Сертификат успешно импортирован в на сервер!";
        getDataCertificatesList();
    }else if(id_command == deleteCertificateFromData){
        qDebug() << __FUNCTION__ << "Сертификат успешно удален с сервера!";
        getDataCertificatesList();
    }else if(id_command == deleteUserFromData){
        qDebug() << __FUNCTION__ << "Пользователь успешно удален с сервера!";
        getDataUsersList();
    }else if(id_command == insertUserToData){
        qDebug() << __FUNCTION__ << "Пользователь успешно добавлен с сервер!";
        getDataUsersList();
    }
}


void MainWindow::on_actionTest_triggered()
{
    //execute_command("test");

    if(terminal->listening()){
        //terminal->send("whoami /user & echo %username%\n");
        //wmic useraccount get name
        bool bOk;
        QString str = QInputDialog::getText( 0,
                                             "Ввод команды",
                                             "Команда:",
                                             QLineEdit::Normal,
                                             "",
                                             &bOk
        );
        if (bOk) {
            ui->txtTerminal->setText("");
            terminal->send(str + "\n", unknown);
            return;
        }
    }
}

#ifdef _WINDOWS
void MainWindow::execute_command(QString param)
{
//     SHELLEXECUTEINFO ShExecInfo = {0};
//     ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
//     ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
//     ShExecInfo.hwnd = NULL;
//     ShExecInfo.lpVerb = "runas";
//     ShExecInfo.lpFile = TEXT("powershell.exe");
//     ShExecInfo.lpParameters = TEXT("stuff..");
//     ShExecInfo.lpDirectory = NULL;
//     ShExecInfo.nShow = SW_SHOW;
//     ShExecInfo.hInstApp = NULL;
//     ShellExecuteEx(&ShExecInfo);
//     WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}
#endif

void MainWindow::createConnectionsObjects()
{
    db = new SqlInterface(this);

    createWsObject(/*usr, pwd */);

}


void MainWindow::on_btnTermOptions_clicked()
{

    auto dlg = new DialogTerminalOptions(terminal, _sett, this);
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){

    }

}


void MainWindow::on_btnTerminalClear_clicked()
{
    ui->txtTerminal->setText("");
}


void MainWindow::on_btnCompToDatabase_clicked()
{

    if(!isDbOpen())
        return;

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->text(0);

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == "Активные пользователи"){

        QString hostName = ui->tableView->model()->index(row, 5).data().toString();
        if(isHostExists(hostName)){
            QMessageBox::critical(this, "Ошибка", "Компьютер уже зарегистрирован в базе!");
            return;
        }else{
            auto result =  QMessageBox::question(this, "Регистрация комьютера в базе", "Зарегистрировать компьютер пользователя в базе?");
            if(result != QMessageBox::Yes){
                return;
            }
        }
        QUuid _uuid = QUuid::createUuid();
        QString uuid = _uuid.toString().mid(1, 36);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", hostName);
        _row.insert("SecondField", hostName);
        _row.insert("Ref",uuid);
        _row.insert("ipadrr", ui->tableView->model()->index(row, 4).data().toString());
        _row.insert("isServer", 0);
        bool r = db->insertSqlTableRow("Servers", _row);
        if(!r)
            qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";

    }
}


void MainWindow::on_btnUserToDatabase_clicked()
{
    if(_sett->launch_mode() == mixed){
        if(!isDbOpen())
            return;
    }else{
        if(!m_client->isStarted())
            return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->data(0, Qt::UserRole).toString();

    auto index = ui->tableView->currentIndex();

    if(!index.isValid())
        return;

    int row = index.row();

    if(currentNode == WsActiveUsers){
        int colHost = modelWsUsers->getColumnIndex("host_name");
        int colName = modelWsUsers->getColumnIndex("user_name");
        int colRef = modelWsUsers->getColumnIndex("Ref");
        auto user = modelWsUsers->index(row, colName).data(Qt::UserRole + colName).toString();
        auto host = modelWsUsers->index(row, colHost).data(Qt::UserRole + colHost).toString();
        auto ref = modelWsUsers->index(row, colHost).data(Qt::UserRole + colRef).toString();

        if(isCertUserExists(host, user)){
            QMessageBox::critical(this, "Ошибка", "Пользователь уже зарегистрирован в базе!");
            return;
        }else{
            auto result =  QMessageBox::question(this, "Регистрация пользователя в базе", "Зарегистрировать пользователя в базе для использования сертификатов?");
            if(result != QMessageBox::Yes){
                return;
            }
        }
        QUuid _uuid = QUuid::createUuid();
        QString uuid = _uuid.toString().mid(1, 36);
        QMap<QString, QVariant> _row;
        _row.insert("FirstField", user);
        _row.insert("SecondField", user);
        _row.insert("Ref", ref);
        _row.insert("uuid", uuid);
        _row.insert("host", host);
        if(_sett->launch_mode() == mixed){
            bool r = db->insertSqlTableRow("CertUsers", _row);
            if(!r){
                qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";
                QString err = db->lastError();
                if(!err.isEmpty())
                    QMessageBox::critical(this, "Ошибка", "Ошибка регистрации в базе!");
            }
            else
                QMessageBox::information(this, "Успех", "Пользователь успешно зарегистрирован!");
        }else{
            auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlInsert, "CertUsers");
            bindQuery.addField("FirstField", user);
            bindQuery.addField("SecondField", user);
            bindQuery.addField("Ref", uuid);
            bindQuery.addField("host", host);

            if(_sett->launch_mode() == mixed){
                QSqlQuery sql = bindQuery.query(db->getDatabase());
                sql.exec();
                if(sql.lastError().type() != QSqlError::NoError){
                    qDebug() << __FUNCTION__ << sql.lastError().text();
                }else{
                    QMessageBox::information(this, "Копирование на сервер", "Контейнер успешно скопирован на сервер!");
                    getDataUsersList();
                }
            }else{
                if(m_client->isStarted()){
                    QString result = bindQuery.to_json();
                    auto doc = QJsonDocument();
                    auto objMain = QJsonObject();
                    objMain.insert("query", result);
                    objMain.insert("id_command", insertUserToData);
                    doc.setObject(objMain);
                    QString param = doc.toJson();
                    m_client->sendCommand("exec_query_qt", "", param);
                }
            }
        }
    }
}


void MainWindow::on_btnDatabaseAdd_clicked()
{

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->data(0, Qt::UserRole).toString();

    if(currentNode == SqlContainers){
        addContainer();
    }else if(currentNode == SqlCertificates){
        addCertificate();
    }else if(currentNode == SqlUsers){
        addCertUser();
    }
}


void MainWindow::on_btnSendCommand_clicked()
{

    if(terminal->listening()){
        bool bOk;
        QString str = QInputDialog::getText( 0,
                                             "Ввод команды",
                                             "Команда:",
                                             QLineEdit::Normal,
                                             "",
                                             &bOk
        );
        if (bOk) {
            ui->txtTerminal->setText("");
            terminal->send(str + "\n", unknown);
            return;
        }
    }
}


void MainWindow::on_btnCurrentUserSaveAs_clicked()
{

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    if(node == currentUserRegistry || node == currentUserDivace){
        saveAsCurrentUserContainer();
    }else if(node == currentUserCertificates){
        saveAsCurrentUserCertificate();
    }

}


void MainWindow::on_btnCurrentCopyToRegistry_clicked()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    QString name = table->model()->index(index.row(), 2).data().toString();

    auto result =  QMessageBox::question(this, "Копирование контейнера", QString("Копировать контейнер: \n%1 \n в реестр?").arg(name));

    if(result != QMessageBox::Yes){
        return;
    }

    auto dlg = new DialogContainerName(name,this);
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() != QDialog::Accepted)
        return;

    QString newName = dlg->keyName() + dlg->name();

    QString nameBase64 = QString("\\\\.\\REGISTRY\\%1").arg(QByteArray(newName.toUtf8()).toBase64());

    QStringList lst = currentUser->getRigstryData();
    QString tmp =  QString("\\\\.\\REGISTRY\\%1").arg(newName);
    if(lst.indexOf(tmp) != -1 || lst.indexOf(nameBase64) != -1){
        QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем %1 уже существует в реестре!").arg(newName));
        return;
    }

#ifdef _WINDOWS
    QModelIndex _index = table->model()->index(index.row(), 1);
    QString device = _index.model()->data(_index, Qt::UserRole + 1).toString().replace("\r", "");
    if(isCyrillic(device)){
        if(currentUser->sid().isEmpty()){
            QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
            terminal->send(cmd, csptestContainerCopy);
            return;
        }
        auto m_device = parseDeviceString(device);
        QString volume = m_device.value("volume").toString();
        if(volume.length() == 1){
            volume = volume + ":\\";
        }
        QDir dir(volume + m_device.value("key_name").toString() + ".000");
        if(dir.exists()){
            auto keyCon = KeysContainer();
            keyCon.fromContainerName(device);
//            keyCon.parseAdressKey(device);
//            keyCon.setWindowsSid(currentUser->sid());
            keyCon.fromFolder(dir.path());
            if(keyCon.isValid()){
                //keyCon.setPath(currentUser->sid(), QByteArray(newName.toUtf8()).toBase64());
                bool result = keyCon.syncRegystry(currentUser->sid());
                if(result){
                    QMessageBox::information(this, "Копирование контейнера", "Контейнер успешно скопирован!");
                    getAvailableContainers(currentUser);
                }else{
                    QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
                    terminal->send(cmd, csptestContainerCopy);
                }
            }
        }else{
            QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
            terminal->send(cmd, csptestContainerCopy);
        }
    }else{
        QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
        terminal->send(cmd, csptestContainerCopy);
    }
#else
    QString cmd = QString("csptest -keycopy -contsrc \"%1\" -contdest \"%2\" -pindest=\"\"").arg(device, nameBase64);
    terminal->send(cmd, csptestContainerCopy);
#endif
}

void MainWindow::on_btnCurrentCopyToSql_clicked()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    QString volume = table->model()->index(index.row(), 1).data().toString();
    QString name = table->model()->index(index.row(), 2).data().toString();

    //ToDo: Сделать проверку на кирилицу

    QDir folder;

            if(QString(volume).left(6) == "FAT12_"){
                QString tom = QString(volume).right(volume.length() - 6);

                QStorageInfo storage(tom + ":" + QDir::separator());
                if(!storage.isReady())
                {
                    QMessageBox::critical(this, "Ошибка", QString("Не возможно прочитать данные с устройства '%1'").arg(volume));
                    return;
                }

                ////Для теста
                //terminal->send(keySetInfo.arg(container), unknown);

                QStringList lst = name.split("@");
                folder = QDir(storage.rootPath() + QDir::separator() +  lst[0] + ".000");

                if(!folder.exists()){
                    return;
                }
            }
            KeysContainer cnt = KeysContainer(this);
            cnt.setName(name);
            if(QString(volume).left(6) == "FAT12_"){
                cnt.fromFolder(folder.path());
            }else if(volume == "REGISTRY"){
                if(currentUser->sid().isEmpty()){
                    qCritical() <<__FUNCTION__ << "Требуется SID!";
                    return;
                }
                //cnt.setPath(currentUser->sid(), cnt.nameBase64());
                cnt.fromRegistry(currentUser->sid(), cnt.bindName());
            }

            if(!cnt.isValid())
            {
                qCritical() <<__FUNCTION__ << "Ошибка: Ошибка загрузки данных контейнера с устройства!";
                return;
            }else{
                if(isContainerExists(name)){
                    QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем '%1' уже есть на сервере!").arg(name));
                    return;
                }
                auto bOK =  QMessageBox::question(this, "Экспорт контейнера", QString("Экспортировать на сервер контейнер %1?").arg(name));
                if(bOK == QMessageBox::No){
                    return;
                }

                QString uuid = QUuid::createUuid().toString();
                uuid = uuid.mid(1, uuid.length() - 2);
                QByteArray data = cnt.toBase64();

                auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlInsert, "Containers");
                QJsonObject obj = QJsonObject();
                obj.insert("name", "Ref");
                obj.insert("value", uuid);
                bindQuery.add_field(obj, bFieldType::qVariant);

                obj = QJsonObject();
                obj.insert("name", "FirstField");
                obj.insert("value", QString(name.toUtf8().toBase64()));
                bindQuery.add_field(obj, bFieldType::qVariant);
                bindQuery.add_field_is_exists(obj);

                obj = QJsonObject();
                obj.insert("name", "SecondField");
                obj.insert("value", name);
                bindQuery.add_field(obj, bFieldType::qVariant);
                bindQuery.add_field_is_exists(obj);

                obj = QJsonObject();
                obj.insert("name", "data");
                obj.insert("value", QString(data.toBase64()));
                bindQuery.add_field(obj, bFieldType::qByteArray);


                //qDebug() << qPrintable(result);

                if(_sett->launch_mode() == mixed){
                    QSqlQuery sql = bindQuery.query(db->getDatabase());
                    sql.exec();
                    if(sql.lastError().type() != QSqlError::NoError){
                        qDebug() << __FUNCTION__ << sql.lastError().text();
                    }else{
                        QMessageBox::information(this, "Копирование на сервер", "Контейнер успешно скопирован на сервер!");
                        getDataContainersList();
                    }
                }else{
                    if(m_client->isStarted()){
                        QString result = bindQuery.to_json();
                        auto doc = QJsonDocument();
                        auto objMain = QJsonObject();
                        objMain.insert("query", result);
                        objMain.insert("id_command", "insertContainerToData");
                        doc.setObject(objMain);
                        QString param = doc.toJson();
                        m_client->sendCommand("exec_query_qt", "", param);
                    }
                }

            }
//                obj.insert("Ref", uuid);
//                obj.insert("FirstField", name);
//                obj.insert("data", QString(data.toBase64()));

//                doc.setObject(obj);
//                QString jsonObject = doc.toJson();

//                QJsonDocument docRef = QJsonDocument();
//                QJsonObject objRef = QJsonObject();
//                objRef.insert("FirstField", name);
//                docRef.setObject(objRef);
//                QString jsonObjectRef = docRef.toJson();

//                if(_sett->launch_mode() == mixed){
//                    db->insert("Containers", jsonObject, jsonObjectRef);
//                }

//                qDebug() << SqlInterface::getParametersFromString("INSERT INTO [dbo].[Containers] ([Ref], [FirstField], [data]) "
//                                                                  "VALUES (?, ?, ?)");



            //}
        //}

    //}

}


void MainWindow::on_btnDatabaseDelete_clicked()
{
    qDebug() << __FUNCTION__;

    auto table = ui->tableView;
    auto index = table->currentIndex();

    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    if(node == SqlContainers){
        delContainer();
    }if(node == SqlCertificates){
        delCertificate();
    }if(node == SqlUsers){
        delCertUser();
    }
}


void MainWindow::on_btnConInfo_clicked()
{
    qDebug() << __FUNCTION__;

    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    int col = modelUserContainers->getColumnIndex("nameInStorgare");

    QModelIndex _index = table->model()->index(index.row(), col);

    QString device = _index.model()->data(_index, Qt::UserRole + col).toString().replace("\r", "");

    QString cmd = QString("csptest -keyset -container \"%1\" -info").arg(device);

    terminal->send(cmd, csptestContainerFnfo);

}

void MainWindow::createColumnAliases()
{
    m_colAliases.insert("uuid", "ID");
    m_colAliases.insert("name", "Имя");
    m_colAliases.insert("uuid_user", "ID пользователя");
    m_colAliases.insert("user_uuid", "ID пользователя");
    m_colAliases.insert("app_name", "Приложение");
    m_colAliases.insert("user_name", "Имя пользователя");
    m_colAliases.insert("ip_address", "IP адрес");
    m_colAliases.insert("host_name", "Host");
    m_colAliases.insert("Ref", "Ссылка");
    m_colAliases.insert("FirstField", "Имя");
    m_colAliases.insert("SecondField", "Представление");
    m_colAliases.insert("privateKey", "Контейнер");
    m_colAliases.insert("_id", "Иднекс");
    m_colAliases.insert("sid", "SID");
    m_colAliases.insert("cache", "cache");
    m_colAliases.insert("subject", "Кому выдан");
    m_colAliases.insert("issuer", "Кем выдан");
    m_colAliases.insert("container", "Контейнер");
    m_colAliases.insert("notValidBefore", "Начало действия");
    m_colAliases.insert("parentUser", "Владелец");
    m_colAliases.insert("notValidAfter", "Окончание дейтствия");
    m_colAliases.insert("serial", "Серийный номер");
    m_colAliases.insert("volume", "Хранилище");
}


void MainWindow::on_btnCurrentDelete_clicked()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    QString deleteKey = node == currentUserRegistry ? "из реестра" : "с устройства";

    //if(node == "currentUserRegistry"){
        QString name = table->model()->index(index.row(), 2).data().toString();
        auto result =  QMessageBox::question(this, "Удаление контейнера", QString("Удалить контейнер: \n%1 \n%2?").arg(name, deleteKey));

        if(result != QMessageBox::Yes){
            return;
        }

        QModelIndex _index = table->model()->index(index.row(), 1);
        QString device = _index.model()->data(_index, Qt::UserRole + 1).toString().replace("\r", "");

#ifdef _WINDOWS
        if(isCyrillic(device)){
            bool isDisk = false;
            auto m_device = parseDeviceString(device);
            QString volume = m_device.value("volume").toString();
            if(volume.length() == 1){
                volume = volume + ":\\";
                isDisk = true;
            }
            if(isDisk){
                QDir dir(volume + m_device.value("key_name").toString() + ".000");
                if(dir.exists()){
                    dir.removeRecursively();
                    QMessageBox::information(this, "Удаление контейнера", "Контейнер успешно удален!");
                    return;
                }
            }else{
                if(currentUser->sid().isEmpty()){
                    QString cmd = QString("csptest -keyset -deletekeyset -container \"%1\"\n").arg(device);
                    terminal->send(cmd, CmdCommand::csptestContainerDelete);
                    return;
                }
                auto keyCon = KeysContainer();
                //keyCon.setPath(m_device.value("fullName").toString(), currentUser->sid());
                bool result = keyCon.removeContainerFromRegistry(currentUser->sid(), m_device.value("name").toString());
                if(result){
                    QMessageBox::information(this, "Удаление контейнера", "Контейнер успешно удален!");
                    getAvailableContainers(currentUser);
                }
            }

        }else{
            QString cmd = QString("csptest -keyset -deletekeyset -container \"%1\"\n").arg(device);
            terminal->send(cmd, CmdCommand::csptestContainerDelete);
        }
#else
        QString cmd = QString("csptest -keyset -deletekeyset -container \"%1\"\n").arg(device);
        terminal->send(cmd, cmdCommand::csptestContainerDelete);
#endif

        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
}


void MainWindow::on_btnDatabaseSaveAs_clicked()
{
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    if(node == SqlContainers){
        saveAsDatabaseContainer();
    }else if(node == SqlCertificates){
        saveAsDatabaseCertificate();
    }
}

void MainWindow::updateInfoContainerOnDatabase(const QString &info, const QString &name, const QString& nameBase64, KeysContainer * cnt)
{

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlUpdate, "Containers");
    QStringList lst = {name, nameBase64};
    bindQuery.addWhere("FirstField", lst.join(","), QBSqlTypeOfComparison::QOn_List);
    bindQuery.addField("cache", info);
    bindQuery.addField("subject", cnt->subject());
    bindQuery.addField("issuer", cnt->issuer());
    bindQuery.addField("notValidAfter", cnt->notValidAfter());
    bindQuery.addField("notValidBefore", cnt->notValidBefore());
    bindQuery.addField("parentUser", cnt->parentUser());

    QString query = bindQuery.to_json();

    QJsonObject cmd = QJsonObject();
    cmd.insert("command", "update_info_container");

    if(_sett->launch_mode() == mixed){
        if(!isDbOpen())
            return;
        QString query = bindQuery.to_json();
        QString _error;
        db->exec_qt(query, _error);
    }else{
        if(m_client->isStarted()){
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("id_command", "update_info_container");
            obj.insert("run_on_return", cmd);
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::onGetDataFromDatabase(const QString &table, const QString param)
{
    auto _table = QJsonDocument::fromJson(table.toUtf8()).object();
    auto _param = QJsonDocument::fromJson(param.toUtf8()).object();
    QString command = _param.value("command").toString();
    if(command == "copy_container_from_data"){
        QString device = _param.value("toDevice").toString();
        if(device.isEmpty()){
            return;
        }else
        {
            if(currentUser->sid().isEmpty())
                return;
            auto rows = _table.value("rows").toArray();
            auto row = rows[0].toObject();
            if(!row.isEmpty()){
                QString dataBase64 = row.value("data").toString();
                QByteArray data = QByteArray::fromBase64(dataBase64.toUtf8());
                auto cnt = KeysContainer();
                cnt.fromJson(data);
                if(!cnt.isValid()){
                    QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");
                    return;
                }
                cnt.fromContainerName(device);
                bool result = cnt.sync(currentUser->sid());
                if(result){
                    QMessageBox::information(this, "Копирование контейнера", "Контейнер успешно скопирован!");
                    terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
                }else
                    QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");
            }
        }
    }
}

void MainWindow::on_btnDatabaseInfo_clicked()
{
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    if(node == currentUserRegistry || node == currentUserDivace || node == SqlContainers){

            int col = modelSqlContainers->getColumnIndex("cache");
            int colName = modelSqlContainers->getColumnIndex("FirstField");
            auto cache = modelSqlContainers->index(index.row(), col).data().toString();
            auto name = modelSqlContainers->index(index.row(), colName).data().toString();
            if(cache.isEmpty()){
                QStringList containers = currentUser->containers();
                foreach(auto cnt, containers){
                    if(cnt.indexOf(name.toUtf8().toBase64()) != 0){
                        QString cmd = QString("csptest -keyset -container \"%1\" -info").arg(cnt);
                        terminal->send(cmd, csptestContainerFnfo);
                        return;
                    }
                }

                QMessageBox::information(this, "Предупреждение", "Информация о контейнере не загружена!"
                                         "Обновите данные запустив просмотр иноформации на компьютере с установленным контейнером в КриптоПро.");

            }else{
                int colName = modelSqlContainers->getColumnIndex("SecondField");
                auto second_name = modelSqlContainers->index(index.row(), colName).data().toString();
                auto doc = QJsonDocument::fromJson(cache.toUtf8());
                auto obj = doc.object();
                auto dlg = new DialogContainerInfo(obj, second_name, this);
                dlg->setModal(true);
                dlg->exec();

            }

    }else if(node == SqlCertificates){

    }else if(node == SqlUsers){

         if(tree->currentItem()->childCount() == 0){
             int nameIndex = modelSqlUsers->getColumnIndex("FirstField");
             int nameHost = modelSqlUsers->getColumnIndex("host");
             int uuidIndex = modelWsUsers->getColumnIndex("uuid");
             //qDebug() << uuidIndex;
             QString name = modelSqlUsers->index(index.row(), nameIndex).data(Qt::UserRole + nameIndex).toString();
             QString host = modelSqlUsers->index(index.row(), nameHost).data(Qt::UserRole + nameHost).toString();

             int row = -1;
             QString findKey = name + host;
             for (int i = 0; i < modelWsUsers->rowCount(); ++i) {
                 if(modelWsUsers->rowKey(i) == findKey){
                     row = i;
                 }
             }
             if(row != -1){
                QString uuid = modelWsUsers->index(row, uuidIndex).data(Qt::UserRole + uuidIndex).toString();
                //qDebug() << uuid;
                sendToRecipient(uuid, "get_available_containers", "get_available_containers", false);
             }
         }

    }


}


void MainWindow::on_btnCurrentUserAdd_clicked()
{
    auto table = ui->tableView;

    if(!table->currentIndex().isValid())
        return;

    int row = table->currentIndex().row();

    if(row < 0){
        return;
    }

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->data(0, Qt::UserRole).toString();

    if(currentNode == currentUserCertificates){
        addCertificate();
    }else{
        if(currentNode == currentUserDivace || currentNode == currentUserRegistry){
            addContainer();
        }
    }
}


void MainWindow::on_mnuCertUsers_triggered()
{
    //C:\Windows\SysWOW64\certmgr.msc
    terminal->send("certmgr.msc", unknown);
}


void MainWindow::on_mnuCryptoPro_triggered()
{
    //C:\Windows\System32\rundll32.exe shell32.dll,Control_RunDLL "C:\Program Files\Crypto Pro\CSP\cpconfig.cpl"
    terminal->send("rundll32.exe shell32.dll,Control_RunDLL \"C:\\Program Files\\Crypto Pro\\CSP\\cpconfig.cpl\"", unknown);
}

void MainWindow::onEndInitConnection()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::onStartGetCertUsersData()
{
    if(m_queue.size() == 0)
        return;

    auto item = m_queue.dequeue();
    int row = modelWsUsers->row(item.first +item.second);
    if(row != -1){
        int ind = modelWsUsers->getColumnIndex("uuid");
        auto uuid = modelWsUsers->index(row, 1).data(Qt::UserRole + ind).toString();
        sendToRecipient(uuid, "get_crypt_data", "get_crypt_data", false);
    }
}

void MainWindow::onWhenDataIsLoaded()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::sendToRecipient(const QString &recipient, const QString &command, const QString &message, bool to_agent)
{
    if(!m_client->isStarted())
        return;

    QString _message = QString("{\"command\": \"%1\", \"message\": \"%2\"}").arg(command, message);
    QJsonObject obj = QJsonObject();
    if(to_agent){
        obj.insert("uuid_agent", recipient);
        obj.insert("uuid_client", m_client->getUuidSession());
    }else{
        obj.insert("uuid_agent", m_client->getUuidSession());
        obj.insert("uuid_client", recipient);
    }
    obj.insert("command", command);
    obj.insert("message", _message);

    QString param = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    if(!to_agent)
        m_client->sendCommand("command_to_qt_client", "", param);
    else
        m_client->sendCommand("command_to_qt_agent", "", param);

}

void MainWindow::resetInfoUserContainers()
{
    //обновляем иформацию о контейнерах у пользователя
    bool isReset = false;
    if(currentUser->containers().size() > 0){
        for(int i = 0; i < modelSqlContainers->rowCount(); ++i){
            int indName = modelSqlContainers->getColumnIndex("FirstField");
            QString name = modelSqlContainers->index(i, indName).data(Qt::UserRole + indName).toString();
            if(!name.isEmpty()){
                int indName = modelUserContainers->getColumnIndex("name");
                auto index = findInTable(modelUserContainers, name, indName, false);
                if(index.isValid()){
                    int indNameInStorgare = modelUserContainers->getColumnIndex("nameInStorgare");
                    QString nameInStorgare = modelUserContainers->index(index.row(), indNameInStorgare).data(Qt::UserRole + indNameInStorgare).toString();
                    KeysContainer * cnt = currentUser->container(nameInStorgare);
                    if(cnt){
                       if(cnt->cache().isEmpty()){
                           int indCache = modelSqlContainers->getColumnIndex("cache");
                           QString cache = modelSqlContainers->index(i, indCache).data(Qt::UserRole + indCache).toString();
                           if(!cache.isEmpty()){
                                cnt->infoFromDataBaseJson(cache);
                                isReset = true;
                           }
                       }

                    }
                }
            }
        }

        if(isReset){
            QString json = currentUser->modelContainersText();
            modelUserContainers->setJsonText(json);
            modelUserContainers->reset();
        }
    }
}

