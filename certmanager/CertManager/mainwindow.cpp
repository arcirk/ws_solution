#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogconnection.h"
#include <QMessageBox>
#include <QSqlError>
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
#include <dialogabout.h>

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
    //certmgr -export -dn 'CN=Ли Александр Сергеевич' -dest test.cer !!! не работает ни хера, нет инфы как правильно использовать

    //
    //certutil -exportPFX -p "Пароль" my 790000fa279f2bd96421c6e2bc00010000fa27 export-cert-2.pfx
    //certutil -importPFX -p "Пароль" my pyatilistnik.pfx

    //рабочий вариант экспорта
    //
    //cryptcp -CSPcert -km -cont "\\.\FAT12_D\HDIMAGE\bob" -df C:\temp\test.cer -der
//    -provtype указать тип криптопровайдера (N) (по умолчанию 75);
//    -provname указать имя криптопровайдера (CSP);
//    -cont задать имя ключевого контейнера (по умолчанию выбор из списка);
//**   -ku* использовать контейнер пользователя (CURRENT_USER);
//**    -km использовать контейнер компьютера (LOCAL_MACHINE);
//    -ex* использовать ключ для обмена зашифрованными данными;
//    -sg использовать ключ для работы с подписями;
//    -dm копирование в хранилище компьютера (LOCAL_MACHINE);
//    -du* копирование в хранилище пользователя (CURRENT_USER);
//    имя название конечного хранилища (по умолчанию “My”);
//    -df в качестве хранилища используется сообщение или файл сертификата;
//    файл имя файла;
//    -der использовать формат DER вместо BASE64.

    //isFormLoaded = false;

    ui->setupUi(this);
    setWindowTitle("Менеджер сертификатов");

    connect(this, &MainWindow::whenDataIsLoaded, this, &MainWindow::onWhenDataIsLoaded);
    connect(this, &MainWindow::endInitConnection, this, &MainWindow::onEndInitConnection);
    connect(this, &MainWindow::startGetCertUsersData, this, &MainWindow::onStartGetCertUsersData);

    ui->tableView->setItemDelegate(new TableDelegate);
    ui->tableView->setIconSize(QSize(16,16));

    createModels();

    formControl();

    initCsptest(); // init CriptoPro

    _sett = new Settings(this);

    createConnectionsObjects();

    m_async_await.append(std::bind(&MainWindow::currentUserSid, this));
    m_async_await.append(std::bind(&MainWindow::currentUserSetTreeItems, this));
    m_async_await.append(std::bind(&MainWindow::currentUserGetConteiners, this));
    m_async_await.append(std::bind(&MainWindow::currentUserGetCertificates, this));
    m_async_await.append(std::bind(&MainWindow::connectToWsServer, this));
    m_async_await.append(std::bind(&MainWindow::connectToDatabase, this));
    m_async_await.append(std::bind(&MainWindow::getDataContainersList, this));
    m_async_await.append(std::bind(&MainWindow::getDataCertificatesList, this));
    m_async_await.append(std::bind(&MainWindow::getDataUsersList, this));
    m_async_await.append(std::bind(&MainWindow::queueInfoUserContainers, this));
    m_async_await.append(std::bind(&MainWindow::wsGetOnlineUsers, this));


    currentUser = new CertUser(this);
    QString curentHost = QSysInfo::machineHostName();
    currentUser->setDomain(curentHost);

    //запуск асинхронных вызовов
    createTerminal();

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

    modelCertUserCertificates = new QJsonTableModel(this);
    modelCertUserCertificates->setRowsIcon(QIcon(":/img/rosette.ico"));
    modelCertUserCertificates->setColumnAliases(m_colAliases);

    modelCertUserContainers = new QJsonTableModel(this);
    modelCertUserContainers->setColumnAliases(m_colAliases);
    modelCertUserContainers->setRowsIcon(QIcon(":/img/cont.png"));
    proxyModeCertlUserConteiners = new QProxyModel(this);
    proxyModeCertlUserConteiners->setSourceModel(modelCertUserContainers);
}

QMap<QString, QString> MainWindow::remoteItemParam(const QModelIndex &index, const QString &node)
{

    QMap<QString, QString> result;

    auto m_key = node.split("/");
    QString name = m_key[0];
    QString host = m_key[1];


    result.insert("host", host.replace("\r", ""));
    if(node.left(4) == "reg_"){
        result.insert("key", remoteUserRegistry);
        name.replace("reg_", "");
    }else if(node.left(4) == "vol_"){
        result.insert("key", remoteUserContainers);
        name.replace("vol_", "");
    }else if(node.left(5) == "cert_"){
        result.insert("key", remoteUserCertificates);
        name.replace("cert_", "");
    }

    result.insert("name", name.replace("\r", ""));

    if(index.isValid()){
        if(result["key"] == remoteUserRegistry || result["key"] == remoteUserContainers){
            int ind = modelCertUserContainers->getColumnIndex("nameInStorgare");
            QString object = proxyModeCertlUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
            object.replace("\r", "");
            result.insert("object", object);
        }else if(result["key"] == remoteUserCertificates){
            int ind = modelCertUserCertificates->getColumnIndex("sha1");
            QString object = modelCertUserCertificates->index(index.row(), ind).data().toString();
            object.replace("\r", "");
            result.insert("object", object);
        }
    }
    return result;
}

void MainWindow::toolBarSetVisible(QWidget * bar, bool value){
    //qDebug() << __FUNCTION__;
    bar->setVisible(value);
}

void MainWindow::createWsObject()
{
    qDebug() << __FUNCTION__;
    m_client = new bWebSocket(this, "conf_qt_cert_manager.json", currentUser->name());
    setWsConnectedSignals();
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
    infoBar->setText("Запуск терминала ...");

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
    connect(terminal, &CommandLine::cmdStarted, this, &MainWindow::onCommandLineStart);

    terminal->start();

}

void MainWindow::currentUserSid()
{
    infoBar->setText("Получение данных текущего пользователя ...");
#ifdef _WINDOWS
    if(currentUser->sid().isEmpty())
        terminal->send("WHOAMI /USER\n", CmdCommand::wmicGetSID);
#else
    onParseCommand("", CmdCommand::wmicGetSID);
#endif
}

void MainWindow::wsGetOnlineUsers()
{
    infoBar->setText("Получение данных о активных пользователях ...");

    if(!m_client->isStarted()){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        updateStatusBar();
        return;
    }
    QJsonDocument doc = QJsonDocument();
    QJsonObject obj = QJsonObject();
    obj.insert("table", true);
    doc.setObject(obj);
    QString param = doc.toJson();
    m_client->sendCommand("get_active_users", "", param);


}

void MainWindow::inVisibleToolBars()
{
    toolBarSetVisible(ui->wToolBarAU, false);
    toolBarSetVisible(ui->wToolbarDatabase, false);
    toolBarSetVisible(ui->wToolBarCurrentUser, false);
    toolBarSetVisible(ui->wToolBarMain, false);
}

void MainWindow::setKeysToRegistry()
{
}

bool MainWindow::updateStatusBar()
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
    QAction *pQCmd = ui->dockWidgetObjects->toggleViewAction();
    ui->mnuView->addAction(pQCmd);
    pQCmd = ui->dockWidgetTerminal->toggleViewAction();
    ui->mnuView->addAction(pQCmd);
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
    //qDebug() << __FUNCTION__;
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

    int ind = modelUserContainers->getColumnIndex("nameInStorgare");
    auto container = modelUserContainers->index(index.row(), ind).data(Qt::UserRole + ind).toString();
    container.replace("\r", "");


    auto model = getLocalMountedVolumes();

    auto dlgSel = new DialogSelectInList(model, "Выбор устройства", this);
    dlgSel->setModal(true);
    dlgSel->exec();
    if(dlgSel->result() == QDialog::Accepted){
        QStringList dlgResult = dlgSel->dialogResult();
        QString volume = dlgResult[0];
        addContainer(volume, container);
    }
}

void MainWindow::saveAsCurrentUserCertificate()
{
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран сертификат!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Выбрать файл"),
                                                 QDir::homePath(),
                                                 "Файл сертификата (*.cer)");
    if(fileName != ""){
        QFile file(fileName);
        int col = modelUserCertificates->getColumnIndex("serial");
        QString serial = modelUserCertificates->index(index.row(), col).data(Qt::UserRole + col).toString();
        if(!serial.isEmpty()){
            auto iter = currentUser->certificates().find(serial);
            if(iter != currentUser->certificates().end()){
                QString sha1 = iter.value()->sha1Hash();
                QString _qbyte = QString("cryptcp -copycert -thumbprint \"%1\" -u -df \"%2\"\n").arg(sha1, fileName);
                if(!sha1.isEmpty()){
                    terminal->send(_qbyte, certmgrExportlCert);
                }
            }
        }
    }
}

void MainWindow::getDatabaseData(const QString& table, const QString& ref, const QJsonObject& param)
{
    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, table);

    bindQuery.addField("Ref", "Ref");
    bindQuery.addField("FirstField", "FirstField");
    bindQuery.addField("data", "data");

    bindQuery.addWhere("Ref", ref);

    QString query = bindQuery.to_json();

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);
        auto doc = QJsonDocument();
        doc.setObject(param);
        onGetDataFromDatabase(resultQuery, doc.toJson());
    }else{
        if(m_client->isStarted()){
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("table", true);
            obj.insert("id_command", "get_data");
            obj.insert("run_on_return", QString(QJsonDocument(param).toJson()));
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString paramData = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", paramData);
        }
    }

}

void MainWindow::saveAsDatabaseCertificate()
{

}

void MainWindow::addCertificate(const QString& from, const QString& to, const QString& id)
{
    qDebug() << __FUNCTION__;


    auto cert = Certificate(this);

    QString fileName;
    bool removeTmp = false;
    QString sha1;

    if(from == STORGARE_LOCALHOST){
        if(id.isEmpty()){
            //selet dialog
        }else{
            sha1 = id;
        }
    }else if(from == STORGARE_DATABASE){

    }

    if(sha1.isEmpty()){
        fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                     QDir::homePath(),
                                                     "Файл сертификата (*.cer)");
        if(fileName.isEmpty()){
            return;
        }

        cert.fromFile(fileName, removeTmp);
    }else{

        cert.fromSha1(sha1);

    }

    if(cert.isValid()){
        if(to == STORGARE_DATABASE){
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
                    getDataCertificatesList();
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

        }else if(to == STORGARE_LOCALHOST){
             terminal->send(QString("certmgr -inst -file \"%1\"").arg(fileName), certmgrInstallCert);
        }
    }
}

void MainWindow::addCertificateFromBase64ToDatabase(const QString &data)
{
    QString s = data.trimmed();
    QRegularExpression re("^[a-zA-Z0-9\\+/]*={0,3}$");
    bool isBase64 = (s.length() % 4 == 0) && re.match(s).hasMatch();
    if(!isBase64)
        return;

    //ByteArray _data = Base64Converter::base64_to_byte(data.toStdString());
    //QString json = QByteArray::fromBase64(data.toUtf8());
    auto doc = QJsonDocument::fromJson(QByteArray::fromBase64(data.toUtf8()));
    auto cert = Certificate();
    cert.fromObject(doc.object());

    if(cert.data().size() > 0){

        auto bindQuery = cert.getSqlQueryObject(QBSqlCommand::QSqlInsert);
        QString result = bindQuery.to_json();
        if(_sett->launch_mode() == mixed){
            QSqlQuery sql = bindQuery.query(db->getDatabase());
            sql.exec();
            if(sql.lastError().type() != QSqlError::NoError){
                qDebug() << __FUNCTION__ << sql.lastError().text();
            }else{
                QMessageBox::information(this, "Копирование на сервер", "Сертификат успешно скопирован на сервер!");
                getDataCertificatesList();
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

void MainWindow::addContainer(const QString& from, const QString& to, const QString& byteArrayBase64, const QString& ref)
{

    auto tree = ui->treeWidget;

    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    QString selFrom = from;

    if(selFrom.isEmpty()){

        QStringList select = {
            "Добавить из каталога",
            "Добавить из устройства"
        };

        if(node != SqlContainers){
            select.append("Добавить из базы данных");
        }

        auto dlg = DialogSelectDevice(this, select, "Добавить контейнер");
        dlg.setModal(true);
        dlg.exec();

        if(dlg.result() != QDialog::Accepted){
            return;
        }

        int selection = dlg.currentSelection();
        if(selection == 0){
            selFrom = FromFolder;
        }else if(selection == 2){
            selFrom = FromDatabase;
        }else
            selFrom = FromVolume;
    }

    addContainerFromVolume(selFrom, to, byteArrayBase64);

}

void MainWindow::addContainerFromVolume(const QString& from, const QString& to, const QString& byteArrayBase64)
{

    QString container;
    bool fromByteArray = false;
    KeysContainer* cnt = new KeysContainer(this);
    QString dest = to;

    if(from == FromRegistry || from == FromVolume){

        auto model = from == FromRegistry ? currentUser->getRigstryData() : currentUser->getDivaceData();

        auto dlgSel = new DialogSelectInList(model, "Выбор устройства", this);
        dlgSel->setModal(true);
        dlgSel->exec();
        if(dlgSel->result() == QDialog::Accepted){
            QStringList dlgResult = dlgSel->dialogResult();
            container = dlgResult[0];
        }else{
            delete cnt;
            return;
        }

        cnt->fromContainerName(container);

    }else if(from == FromDatabase){

        if(byteArrayBase64.isEmpty()){
            QMessageBox::critical(this, "Ошибка", "Ошибка получения данных с сервера!");
            delete cnt;
            return;
        }
        QByteArray data = QByteArray::fromBase64(byteArrayBase64.toUtf8());
        cnt->fromJson(data);
        cnt->fromContainerName(from);

    }else if(from == FromFolder){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог источника"),
                                                     QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
        if(dir != ""){
            QFile file(dir + QDir::separator() + "name.key");
            if(file.open(QIODevice::ReadOnly)){
                QString data = QString::fromLocal8Bit(file.readAll());
                int ind = data.indexOf("\026");
                if(ind == -1){
                    QMessageBox::critical(this, "Ошибка", "В выбранном каталоге не найдены данные контейнера!");
                    delete cnt;
                    return;
                }
            }else{
                QMessageBox::critical(this, "Ошибка", "В выбранном каталоге не найдены данные контейнера!");
                delete cnt;
                return;
            }

            container = dir;
            cnt->fromContainerName(dir);
        }
    }else{
        if(!from.isEmpty())
            if(byteArrayBase64.isEmpty())
                cnt->fromContainerName(from);
            else{
                QByteArray data = QByteArray::fromBase64(byteArrayBase64.toUtf8());
                cnt->fromJson(data);
                cnt->fromContainerName(from);
                fromByteArray = true;
            }


        else{
            QMessageBox::critical(this, "Ошибка", "Дописать!");
            delete cnt;
            return;
        }

    }

    if(!fromByteArray)
        cnt->readData(currentUser->sid());

    if(!cnt->isValid()){
        QMessageBox::critical(this, "Ошибка", "Ошибка инициализации источника!");
        delete cnt;
        return;
    }

    QString containerName = cnt->originalName();

    auto dlg = DialogContainerName(containerName, this);
    dlg.setModal(true);
    dlg.exec();
    if(dlg.result() == QDialog::Accepted){
        containerName = dlg.name();
        if(containerName != cnt->originalName())
             cnt->setNewOriginalName(containerName);
    }else{
        QMessageBox::critical(this, "Ошибка", "Операция отменена!");
        delete cnt;
        return;
    }

    bool result = false;

    if(dest == ToFolder){
       if(!cnt->keyName().isEmpty()){
           QString dir = QFileDialog::getExistingDirectory(this, tr("Выбрать каталог для сохранения данных"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
           if(dir != ""){
               result = cnt->sync(KeysContainer::typeOfStorgare(dir), dir, currentUser->sid());
           }
       }
    }else if(dest == ToVolume){
        auto model = getLocalMountedVolumes();
        auto dlgSel = new DialogSelectInList(model, "Выбор устройства", this);
        dlgSel->setModal(true);
        dlgSel->exec();
        if(dlgSel->result() == QDialog::Accepted){
            QStringList dlgResult = dlgSel->dialogResult();
            QString destantion = dlgResult[0] + cnt->originalName();
            cnt->fromContainerName(destantion);
            result = cnt->sync(currentUser->sid());
            if(result)
                on_toolCurrentUserUpdate_clicked();
        }

    }else if(dest == ToRegistry){
        result = cnt->sync(KeysContainer::TypeOfStorgare::storgareTypeRegistry, "", currentUser->sid());
        if(result)
            on_toolCurrentUserUpdate_clicked();
    }else if(dest == ToDatabase){
        if(_sett->launch_mode() == mixed)
            result = cnt->sync(db);
        else
            result = cnt->sync(m_client);
    }else
        result = cnt->sync(KeysContainer::typeOfStorgare(dest), dest, currentUser->sid());

    if(!result){
        QMessageBox::critical(this, "Ошибка", "Ошибка записи данных контейнера!");
    }else{
        QMessageBox::information(this, "Успех", "Данные контейнера успешно записаны!");
        if(dest == ToFolder){

        }else if(dest == ToVolume || dest == ToRegistry){

        }else if(dest == ToDatabase){
            getDataContainersList();
        }else
            getDataContainersList();
    }
    delete cnt;


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

void MainWindow::resetCertData(CertUser *usr, const QString& node)
{
    if(node == currentUserContainers || node == currentUserRegistry || node == currentUserDivace){
        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
    }else if(node == currentUserCertificates){
        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
    }else if(node == remoteUser){

    }

}

void MainWindow::resetCertUsersTree()
{
    auto root = findTreeItem(SqlUsers);

    if(!root)
        return;

    for(int i = 0; i < root->childCount();  ++i){
        root->removeChild(root->child(i));
    }

    int iUser = modelSqlUsers->getColumnIndex("FirstField");
    int iHost = modelSqlUsers->getColumnIndex("host");
    int iUuid = modelSqlUsers->getColumnIndex("uuid");

    for(int i = 0; i < modelSqlUsers->rowCount(); ++i){
        QString name = modelSqlUsers->index(i, iUser).data(Qt::UserRole + iUser).toString();
        QString host = modelSqlUsers->index(i, iHost).data(Qt::UserRole + iHost).toString();
        QString uuid = modelSqlUsers->index(i, iUuid).data(Qt::UserRole + iUuid).toString();
        QPair<QString, QString> index = qMakePair(name, host);
        auto itr = m_users.find(index);
        CertUser * user = nullptr;
        if(itr == m_users.end()){
            user = new CertUser(this);
            user->setName(name);
            user->setDomain(host);
            user->setUuid(QUuid::fromString(uuid));
            m_users.insert(index, user);
            qDebug() << name << host << uuid << user->uuid().toString();
        }else
            user = itr.value();

        auto itemUser = addTreeNode(name + " (" + host + ")", name + host, ":/img/certUsers.png");
        root->addChild(itemUser);
    }

}

bool MainWindow::selectVolume(QString &volume, QString &container)
{
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

    if(dlgSel.result() != QDialog::Accepted){
        return false;
    }
        QStringList dlgResult = dlgSel.dialogResult();
        if(dlgResult.size() == 0)
            return false;
    container = dlgResult[2];
    volume = dlgResult[0];

    return true;
}

QStringList MainWindow::getObjectsFromdataBase()
{
    QList<int> vCols = {0};
    int col = modelSqlContainers->getColumnIndex("SecondField");
    vCols.append(col);
    col = modelSqlContainers->getColumnIndex("notValidAfter");
    vCols.append(col);
    col = modelSqlContainers->getColumnIndex("parentUser");
    vCols.append(col);
    auto dlg = DialogSelectInList(modelSqlContainers, "Выбор контейнера", vCols, this);
    dlg.setModal(true);
    dlg.exec();

    if(dlg.result() == QDialog::Accepted)
        return dlg.dialogResult();
    else
        return {};

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

void MainWindow::treeSetCurrentContainers(const QString& filter, QJsonTableModel * model, QProxyModel * proxy)
{
        auto tableView = ui->tableView;
        tableView->setModel(nullptr);

//        currentList.filter = "{\"IsGroup\": 1,
//                               \"Parent\": \"" + uuid + "\"}"
        proxy->setFilter(QString("{\"volume\":\"%1\"}").arg(filter));
        tableView->setModel(proxy);

        int ind = model->getColumnIndex("name");
        if(ind != -1)
            tableView->setColumnHidden(ind, true);
        ind = model->getColumnIndex("nameInStorgare");
        if(ind != -1)
            tableView->setColumnHidden(ind, true);

        tableView->resizeColumnsToContents();

}

void MainWindow::treeSetFromCurrentUserCerts(QJsonTableModel* model)
{
    auto table = ui->tableView;
    table->setModel(nullptr);
    if(model){
        table->setModel(model);
        int col = model->getColumnIndex("serial");
        //table->setColumnHidden(col, true);
        table->resizeColumnsToContents();
    }
}

void MainWindow::treeSetCertUserData(CertUser *usr)
{
    if(!usr)
        return;
    if(usr->uuid().isNull())
        return;

    auto root = findTreeItem(SqlUsers);
    if(!root)
        return;
    QString key = usr->name() + usr->domain();
    auto user = findTreeItem(key, root);
    qDebug() << usr->uuid().toString();
    if(!user){        
        user = addTreeNode(usr->name(), key, ":/img/certUsers.png");
        root->addChild(user);
    }

    QString pair = usr->name() + "/" + usr->domain();

    auto userData = findTreeItem("data_" + usr->uuid().toString(), user);
    if(!userData){
        userData = addTreeNode("Доступные контейнеры", "data_" + usr->uuid().toString(), ":/img/key16.png");
        user->addChild(userData);
    }
    auto userReg = findTreeItem("reg_" + pair, userData);
    if(!userReg){
        userReg = addTreeNode("Реестр", "reg_" + pair, ":/img/registry16.png");
        userData->addChild(userReg);
    }
    auto userVol = findTreeItem("vol_" + pair, userData);
    if(!userVol){
        userVol = addTreeNode("Устройства", "vol_" + pair, ":/img/Card_Reader_16.ico");
        userData->addChild(userVol);
    }
    auto userCrt = findTreeItem("cert_" + pair, user);
    if(!userCrt){
        userCrt = addTreeNode("Установленные сертификаты", "cert_" + pair, ":/img/cert.png");
        user->addChild(userCrt);
    }
}

void MainWindow::wsSetMplCertData(const QString& recipient, const QString& message)
{
    int iUuid = modelWsUsers->getColumnIndex("uuid");
    auto index = findInTable(modelWsUsers, recipient, iUuid, false);
    if(!index.isValid())
        return;

    QString result = QByteArray::fromBase64(message.toUtf8());
    auto doc = QJsonDocument::fromJson(result.toUtf8());
    if(doc.isEmpty())
        return;

    auto obj = doc.object();
    QStringList containers = obj.value("cnt").toString().split("\n");
    auto certs = obj.value("certs").toObject();

    int iName = modelWsUsers->getColumnIndex("user_name");
    int iHost = modelWsUsers->getColumnIndex("host_name");
    int iUuidUser = modelWsUsers->getColumnIndex("uuid_user");
    QString name = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iName).toString();
    QString host = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iHost).toString();
    QString uuidUser = modelWsUsers->index(index.row(), iUuidUser).data(Qt::UserRole + iUuidUser).toString();
    if(!name.isEmpty() && !host.isEmpty()){
        auto itr = m_users.find(qMakePair(name, host));
        CertUser * usr = nullptr;
        if(itr != m_users.end())
            usr = itr.value();
        else{
            usr = new CertUser(this);
            usr->setName(name);
            usr->setDomain(host);
            usr->setUuid(QUuid::fromString(uuidUser));
            m_users.insert(qMakePair(name, host), usr);
        }
        usr->setContainers(containers);
        usr->certsFromModel(certs);
        treeSetCertUserData(usr);
    }
    //Отправим следующему запрос
    onStartGetCertUsersData();
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
    }  catch (std::exception&) {
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

    //qDebug() << __FUNCTION__ << "command: " << (CmdCommand)command;

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
    tree->setHeaderHidden(true);

    auto root = addTreeNode("root", "root", ":/img/system_config_root.ico");
    tree->addTopLevelItem(root);
    auto curr_user = addTreeNode("Текущий пользователь", СurrentUser, ":/img/userOptions.ico");
    root->addChild(curr_user);
    auto server = addTreeNode("База", SqlServer, ":/img/sqlServer.png");
    root->addChild(server);
    auto ws = addTreeNode("Сервер взаимодействия", WsServer, ":/img/socket_16_only.ico");
    root->addChild(ws);

    tree->expandAll();
}

void MainWindow::resetTableJsonModel(const QJsonObject &obj, const QString &id_command){

    if(id_command == DataContainersList){
        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonConteiners = fromBase64(base64.toUtf8());

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
    }else if(id_command == DataCertificatesList){

        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonCertificates = fromBase64(base64.toUtf8());

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
    }else if(id_command == DataUsersList){

        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString jsonUsers = fromBase64(base64.toUtf8());

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

        //заполняем в дереве далее
        resetCertUsersTree();
    }
}

void MainWindow::getDataContainersList()
{

    qDebug() << __FUNCTION__;

    infoBar->setText("Получение данных с сервера ...");

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "Containers");

    bindQuery.addField("EmptyTitle", "EmptyTitle");
    bindQuery.addField("Ref", "Ref");
    bindQuery.addField("FirstField", "FirstField");
    bindQuery.addField("SecondField", "SecondField");
    bindQuery.addField("subject", "subject");
    bindQuery.addField("issuer", "issuer");
    bindQuery.addField("notValidBefore", "notValidBefore");
    bindQuery.addField("notValidAfter", "notValidAfter");
    bindQuery.addField("parentUser", "parentUser");
    bindQuery.addField("cache", "cache");

    QString query = bindQuery.to_json();

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;    
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);

        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, "DataContainersList");

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else{

        if(!m_client->isStarted())
            return;
        auto obj = QJsonObject();
        obj.insert("query", query);
        obj.insert("table", true);
        obj.insert("id_command", "DataContainersList");
        auto doc = QJsonDocument();
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("exec_query_qt", "", param);

    }
}

void MainWindow::getDataCertificatesList()
{
    qDebug() << __FUNCTION__;

    infoBar->setText("Получение данных с сервера ...");

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
        if(!db->isOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);

        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, DataCertificatesList);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else{
        if(m_client->isStarted()){
            if(!m_client->isStarted())
                return;
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("table", true);
            obj.insert("id_command", DataCertificatesList);
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString param = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", param);
        }
    }
}

void MainWindow::getDataUsersList()
{
    qDebug() << __FUNCTION__;

    infoBar->setText("Получение данных с сервера ...");

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
        if(!db->isOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(result, resultQuery, _error);
        auto objMain = QJsonObject();
        objMain.insert("table", QString(QByteArray(resultQuery.toUtf8()).toBase64()));
        resetTableJsonModel(objMain, DataUsersList);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else{
        if(!m_client->isStarted())
            return;
        auto obj = QJsonObject();
        obj.insert("query", result);
        obj.insert("table", true);
        obj.insert("id_command", DataUsersList);
        auto doc = QJsonDocument();
        doc.setObject(obj);
        QString param = doc.toJson();
        m_client->sendCommand("exec_query_qt", "", param);
    }
}

void MainWindow::currentUserSetTreeItems()
{
    auto item = findTreeItem(СurrentUser);
    if(item){
        item->setText(0, QString("Текущий пользователь (%1)").arg(currentUser->name()) );
        if(item->childCount() == 0){
            auto Root = addTreeNode("Доступные контейнеры", currentUserAvailableContainers, ":/img/key16.png");
            item->addChild(Root);
            auto certs = addTreeNode("Установленные сертификаты", currentUserCertificates, ":/img/cert.png");
            item->addChild(certs);
            auto reg = addTreeNode("Реестр", currentUserRegistry, ":/img/registry16.png");
            Root->addChild(reg);
            auto dev = addTreeNode("Устройства", currentUserDivace, ":/img/Card_Reader_16.ico");
            Root->addChild(dev);
        }
    }

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void MainWindow::currentUserGetConteiners()
{
    terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
}

void MainWindow::currentUserGetCertificates()
{
    terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
}

void MainWindow::resetUserCertModel(CertUser* usr, QJsonTableModel* model)
{

    model->setJsonText(usr->modelCertificatesText());
    model->reset();
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

void MainWindow::setDefaultItemChilds(QTreeWidgetItem *item)
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

void MainWindow::getAvailableContainers(CertUser *usr)
{
    qDebug() << __FUNCTION__;

    if(usr == currentUser)
        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
}

void MainWindow::disableToolBar()
{
    //qDebug() << __FUNCTION__;
    ui->btnAdd->setEnabled(false);
    ui->btnDelete->setEnabled(false);
}

bool MainWindow::isContainerExists(const QString &name, CertUser* usr, const QString& dest)
{
    qDebug() << __FUNCTION__;

    CertUser* user = usr;

    if(!user){
       user = currentUser;
    }
    if(dest.isEmpty() || dest == ToDatabase || dest == FromDatabase || dest == DATABASE_){
        int ind = modelSqlContainers->getColumnIndex("FirstField");
        if(ind != -1){
            auto index = findInTable(modelSqlContainers, name, ind, false);
            return index.isValid();
        }
    }else if(dest == ToRegistry || dest == FromRegistry || dest == REGISTRY_){
        if(user->getRigstryData().indexOf(name) != -1)
            return true;
    }else
        if(user->getDivaceData().indexOf(name) != -1)
            return true;

    return false;
}

bool MainWindow::isCertUserExists(const QString &name, const QString& host)
{
    qDebug() << __FUNCTION__;

    auto p = qMakePair(name, host);
    auto itr = m_users.find(p);

    return itr != m_users.end();

}

bool MainWindow::isWsUserExists(const QString &name, const QString &host)
{

    if(modelWsUsers){
        int index = modelWsUsers->row(qMakePair(name, host));
        return index != -1 ? true : false;
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

QString MainWindow::getSessionUuid(const QString &name, const QString &host) const
{

    int index = modelWsUsers->row(qMakePair(name, host));
    int iUuid = modelWsUsers->getColumnIndex("uuid");
    if(index != -1)
        return modelWsUsers->index(index, iUuid).data(Qt::UserRole + iUuid).toString();
    else
        return "";

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

    infoBar->setText("Попытка подключения к серверу ...");

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen()){
            if(m_async_await.size() > 0)
            {
                auto f = m_async_await.dequeue();
                f();
            }
            return;
        }
    }
    if(!m_client)
        return;

    if(m_client->isStarted())
        return;

    m_client->setAppName("qt_cert_manager");
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

    }else{
        QString _host = m_client->options()[bConfFieldsWrapper::ServerHost].toString();
        int _port = m_client->options()[bConfFieldsWrapper::ServerPort].toInt();
        m_client->setHost(_host);
        m_client->setPort(_port);
        m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
    }


}

void MainWindow::on_mnuExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{

    QString key = item->data(0, Qt::UserRole).toString();
    qDebug() << __FUNCTION__ << "tree key:" << key;

    inVisibleToolBars();

    if(item->childCount() > 0 && key != SqlUsers){
        setDefaultItemChilds(item);
        disableToolBar();       
        toolBarSetVisible(ui->wToolBarMain, true);
    }else{   
        if(key == currentUserRegistry){
           toolBarSetVisible(ui->wToolBarCurrentUser, true);
           treeSetCurrentContainers("REGISTRY", modelUserContainers, proxyModelUserConteiners);
           ui->btnCurrentCopyToRegistry->setEnabled(true);
           ui->btnCurrentCopyToSql->setEnabled(true);
           ui->btnCurrentUserAdd->setEnabled(true);
        }else if(key == currentUserDivace){
           toolBarSetVisible(ui->wToolBarCurrentUser, true);
           treeSetCurrentContainers("!REGISTRY", modelUserContainers, proxyModelUserConteiners);
           ui->btnCurrentCopyToRegistry->setEnabled(true);
           ui->btnCurrentCopyToSql->setEnabled(true);
           ui->btnCurrentUserAdd->setEnabled(true);
        }else if(key == SqlContainers){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlContainers();
            ui->btnDatabaseAdd->setEnabled(true);
            ui->btnDatabaseSaveAs->setEnabled(true);
        }else if(key == WsActiveUsers){
            toolBarSetVisible(ui->wToolBarAU, true);
            treeSetOnlineWsUsers();
        }else if(key == SqlCertificates){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlCertificates();
            ui->btnDatabaseAdd->setEnabled(true);
            ui->btnDatabaseSaveAs->setEnabled(true);
        }else if(key == SqlUsers){
            toolBarSetVisible(ui->wToolbarDatabase, true);
            treeSetFromSqlUsers();
            ui->btnDatabaseAdd->setEnabled(false);
            ui->btnDatabaseSaveAs->setEnabled(false);
        }else if(key == currentUserCertificates){
            toolBarSetVisible(ui->wToolBarCurrentUser, true);
            treeSetFromCurrentUserCerts(modelUserCertificates);
            ui->btnCurrentCopyToRegistry->setEnabled(false);
            ui->btnCurrentCopyToSql->setEnabled(true);
            ui->btnCurrentUserAdd->setEnabled(true);
            ui->btnAdd->setEnabled(true);
        }else{
            ui->tableView->setModel(nullptr);
            if(key.left(4) == "reg_"){
                QStringList m_key = key.split("_");
                QStringList m_userHost = m_key[1].split("/");
                QPair<QString, QString> index = qMakePair(m_userHost[0], m_userHost[1]);
                auto itr = m_users.find(index);
                if(itr != m_users.end()){
                    modelCertUserContainers->setJsonText(itr.value()->modelContainersText());
                    modelCertUserContainers->reset();
                    treeSetCurrentContainers("REGISTRY", modelCertUserContainers, proxyModeCertlUserConteiners);
                    resetInfoUserContainers(itr.value());
                }
                toolBarSetVisible(ui->wToolBarCurrentUser, true);
                ui->btnCurrentCopyToRegistry->setEnabled(true);
                ui->btnCurrentCopyToSql->setEnabled(true);
                ui->btnCurrentUserAdd->setEnabled(true);

            }else if(key.left(4) == "vol_"){
                QStringList m_key = key.split("_");
                QStringList m_userHost = m_key[1].split("/");
                QPair<QString, QString> index = qMakePair(m_userHost[0], m_userHost[1]);
                auto itr = m_users.find(index);
                if(itr != m_users.end()){
                    modelCertUserContainers->setJsonText(itr.value()->modelContainersText());
                    modelCertUserContainers->reset();
                    treeSetCurrentContainers("!REGISTRY", modelCertUserContainers, proxyModeCertlUserConteiners);
                    resetInfoUserContainers(itr.value());
                }
                toolBarSetVisible(ui->wToolBarCurrentUser, true);
                ui->btnCurrentCopyToRegistry->setEnabled(true);
                ui->btnCurrentCopyToSql->setEnabled(true);
                ui->btnCurrentUserAdd->setEnabled(true);
            }else if(key.left(5) == "cert_"){
                QStringList m_key = key.split("_");
                QStringList m_userHost = m_key[1].split("/");
                QPair<QString, QString> index = qMakePair(m_userHost[0], m_userHost[1]);
                auto itr = m_users.find(index);
                if(itr != m_users.end()){
                    resetUserCertModel(itr.value(), modelCertUserCertificates);
                    treeSetFromCurrentUserCerts(modelCertUserCertificates);
                }
                toolBarSetVisible(ui->wToolBarCurrentUser, true);
                ui->btnCurrentCopyToRegistry->setEnabled(true);
                ui->btnCurrentCopyToSql->setEnabled(true);
                ui->btnCurrentUserAdd->setEnabled(true);
                ui->btnAdd->setEnabled(true);
            }
        }
    }

}

void MainWindow::connectToDatabase()
{

    infoBar->setText("Попытка подключения к серверу ...");

    qDebug() << __FUNCTION__;

    if(_sett->launch_mode() != mixed)
    {
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    QString host = _sett->server();
    QString database = "arcirk";
    QString userName = _sett->user();
    QString password = _sett->pwd();

    db->setSqlUser(userName);
    db->setSqlPwd(password);
    db->setHost(host);
    db->setDatabaseName(database);
    db->connect();

    if (!db->isOpen()){
        QMessageBox::critical(this, "Ошибка", QString("Ошибка подключения к базе данных: %2").arg(db->lastError()));
    }

    connectToWsServer();

    _sett->save();

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
            connectToDatabase();
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

        updateStatusBar();
    }

}



void MainWindow::on_btnAdd_clicked()
{
}

void MainWindow::on_btnDelete_clicked()
{
    if(!db->isOpen())
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
    qDebug() << __FUNCTION__;

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

}

//void MainWindow::on_mnuOptions_triggered()
//{
//    auto optDlg = new DialogOptions(this);
//    optDlg->setModal(true);
//    optDlg->exec();

//    if(optDlg->result() == QDialog::Accepted){
//        //auto opt = ClientSettings("conf_qt_cert_manager.json");

//    }

//    qDebug() << __FUNCTION__;
//}

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

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }

//        if(_sett->launch_mode() != mixed){
//            getDataContainersList();
//        }

//        emit endInitConnection();
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

    auto message = QByteArray::fromBase64(resp.toUtf8());
    auto doc = QJsonDocument::fromJson(message);
    auto objResp = doc.object();
    QString uuid = objResp.value("uuid").toString();
    QString user_uuid = objResp.value("user_uuid").toString();
    QString name = objResp.value("user_name").toString();
    QString _host_name = objResp.value("host_name").toString();
    QString _ip_address = objResp.value("ip_address").toString();
    QString _app_name = objResp.value("app_name").toString();

    if(name == currentUser->name() && _host_name == currentUser->domain()){

    }else{

        auto obj = QJsonObject();
        obj.insert("Empty", "");
        obj.insert("uuid", uuid);
        obj.insert("name", name);
        obj.insert("user_uuid", user_uuid);
        obj.insert("app_name", _app_name);
        obj.insert("user_name", name);
        obj.insert("ip_address", _ip_address);
        obj.insert("host_name", _host_name);

        modelWsUsers->addRow(obj);
        modelWsUsers->setRowKey(modelWsUsers->rowCount() - 1, qMakePair(name, _host_name));
        //qDebug() << __FUNCTION__ << qPrintable(resp);

//        auto itr = m_users.find(qMakePair(name, _host_name));
//        if(itr != m_users.end()){

//        }

        updateCertUsersOnlineStstus();
        m_queue.append(uuid);
        onStartGetCertUsersData();
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

    auto doc = QJsonDocument::fromJson(resp.toUtf8());
    auto obj = doc.object();
    QString uuid = obj.value("uuid").toString();

    int ind = modelWsUsers->getColumnIndex("uuid");
    auto index = findInTable(modelWsUsers, uuid, ind, false);
    if(index.isValid()){
        int iName = modelWsUsers->getColumnIndex("user_name");
        int iHost = modelWsUsers->getColumnIndex("host_name");
        QString name = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iName).toString();
        QString host = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iHost).toString();
        if(!name.isEmpty() && !host.isEmpty()){
            auto itr = m_users.find(qMakePair(name, host));
            CertUser * usr = nullptr;
            if(itr != m_users.end()){
                usr = itr.value();
                usr->eraseData();
            }

            auto treeItem = findTreeItem(name + host);
            if(treeItem){
                while (treeItem->childCount() > 0) {
                    treeItem->removeChild(treeItem->child(0));
                }
            }

            emit ui->treeWidget->itemClicked(ui->treeWidget->currentItem(), 0);

            modelWsUsers->removeRow(index.row());
        }

    updateCertUsersOnlineStstus();

    }
}

void MainWindow::onMessageReceived(const QString &msg, const QString &uuid, const QString &recipient, const QString &recipientName)
{
    qDebug() << __FUNCTION__;
}

void MainWindow::onDisplayError(const QString &what, const QString &err)
{

    qCritical() << __FUNCTION__ << what << ": " << err ;

    ui->txtTerminal->setText("\n" + ui->txtTerminal->toPlainText() + "\n" + "ws error: " + what + ": " + err + "\n");
    ui->txtTerminal->verticalScrollBar()->setValue(ui->txtTerminal->verticalScrollBar()->maximum());

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
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
//    if(!isDbOpen())
//        return;

//    auto treeItem = ui->treeWidget->currentItem();
//    if(!treeItem){
//        return;
//    }

//    QString currentNode = treeItem->text(0);

//    auto index = ui->tableView->currentIndex();

//    if(!index.isValid())
//        return;

//    int row = index.row();

//    if(currentNode == "Реестр"){

//    }else if(currentNode == "Пользователи"){
////        auto reg = Registry();
////        QStringList users = reg.localUsers();
////        auto dlg = new DialogSelectInList(users, "Пользователи системы", this);
////        dlg->setModal(true);
////        dlg->exec();
////        if(dlg->result() == QDialog::Accepted){
////            QString userName = dlg->dialogResult();
////        }
//    }else if(currentNode == "Компьютеры"){
//        auto dlg = new DialogComputer(this);
//        QMap<QString, QVariant> _row;
//        _row.insert("FirstField", ui->tableView->model()->index(row, 2).data().toString());
//        _row.insert("SecondField", ui->tableView->model()->index(row, 3).data().toString());
//        _row.insert("Ref", ui->tableView->model()->index(row, 4).data().toString());
//        _row.insert("ipadrr", ui->tableView->model()->index(row, 5).data().toString());
//        _row.insert("isServer", ui->tableView->model()->index(row, 6).data().toInt());
//        dlg->setComputer(_row);
//        dlg->setModal(true);
//        dlg->exec();
//        if(dlg->result() == QDialog::Accepted){
//            auto result = dlg->computer();
//            if(!result["Ref"].toString().isEmpty()){
//                bool r = db->updateSqlTableRow("Servers", result, result["Ref"].toString());
////                auto model = (QSqlQueryModel*)ui->tableView->model();
////                model->setQuery(model->query().lastQuery());
////                UpdateRowIcons();
//                 loadCimputers();
//               // ui->tableView->resizeColumnsToContents();
//                if(!r)
//                    qCritical() << __FUNCTION__ << "Ошибка обновления строки в таблице 'Servers'!";
//            }
//        }
//    }
}

void MainWindow::onGetActiveUsers(const QString& resp){

    infoBar->setText("Получение данных о активных пользователях ...");

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
    int iHost = modelWsUsers->getColumnIndex("host_name");
    int iName = modelWsUsers->getColumnIndex("user_name");
    int iApp = modelWsUsers->getColumnIndex("app_name");
    int iUuid = modelWsUsers->getColumnIndex("uuid");
    for (int i = 0; i < modelWsUsers->rowCount(); ++i) {
        auto user = modelWsUsers->index(i, iName).data(Qt::UserRole + iName).toString();
        auto host = modelWsUsers->index(i, iHost).data(Qt::UserRole + iHost).toString();
        auto app = modelWsUsers->index(i, iApp).data(Qt::UserRole + iApp).toString();
        auto uuid = modelWsUsers->index(i, iUuid).data(Qt::UserRole + iUuid).toString();
        modelWsUsers->setRowKey(i, qMakePair(user, host));
        QList<QPair<QString, QString>> temp;
        if(app == "qt_mpl_client" && isCertUserExists(user, host)){
            if(!currentUser->thisIsTheUser(user, host)){
                auto p = qMakePair(user,host);
                if(temp.indexOf(p) != -1)
                    continue;
                //возмет у первого данные, остальнх если подключено нескольколько клиентов с одним пользоваетем игнорим
                m_queue.append(uuid); //получим данные контейнеров и сертификатов на удаленных клиентах
                temp.append(p);
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
        CertUser * user = nullptr;
        if(!currentUser->thisIsTheUser(name, host)){
            auto itr = m_users.find(qMakePair(name, host));
            if(itr != m_users.end())
                continue;
            user = new CertUser(this);
            user->setName(name);
            user->setDomain(host);
            m_users.insert(qMakePair(name, host), user);
        }
        else{
             user = currentUser;
//             if(_sett->launch_mode() == mixed){
//                 if(db->isOpen()){
//                     QString str = QString("select [Ref], [sid], [uuid] from [arcirk].[dbo].[CertUsers] where [FirstField] = '%1' AND [host] = '%2'").arg(name, host);
//                     QSqlQuery query = QSqlQuery(str, db->getDatabase());
//                     QString ref;
//                     QString sid;
//                     QString uuid;
//                     QMap<QString, QVariant> _row;

//                     while (query.next()) {
//                         ref = query.value(0).toString();
//                         sid = query.value(1).toString();
//                         uuid = query.value(2).toString();
//                         if(sid.isEmpty())
//                             _row.insert("sid", currentUser->sid());
//                         if(uuid.isEmpty())
//                             _row.insert("uuid", user_uuid.toString());
//                         break;
//                     }
//                     if(!ref.isEmpty() && _row.size() > 0){
//                         currentUser->setRef(ref);
//                         //update sid
//                         db->updateSqlTableRow("CertUsers", _row, ref);
//                     }
//                 }
//             }
        }
        user->setUuid(user_uuid);
        user->setOnline(true);
    }

    if(m_queue.size() > 0){
        emit startGetCertUsersData();
    }else
        updateStatusBar();
}

void MainWindow::onParseCommand(const QVariant &result, int command)
{
    if(command == CmdCommand::echoUserName){
        currentUser->setName(result.toString());
        auto treeItem = findTreeItem(СurrentUser);
        if(treeItem)
            treeItem->setText(0, QString("Текущий пользователь (%1)").arg(result.toString()));
        //terminal->send(QString("wmic useraccount where name='%1' get sid\n").arg(result), CommandLine::cmdCommand::wmicGetSID);
    }else if(command == CmdCommand::wmicGetSID){

        currentUser->setSid(result.toString());
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else if(command == CmdCommand::csptestGetConteiners){
        QString res = result.toString();
        res.replace("\r", "");
        csptestCurrentUserGetContainers(result.toString());

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
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

        resetUserCertModel(currentUser, modelUserCertificates);

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else if(command == CmdCommand::certmgrInstallCert){
        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
    }else if(command == CmdCommand::certmgrDeletelCert){
        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
    }
}

void MainWindow::onCommandError(const QString &result, int command)
{
    qCritical() << __FUNCTION__ << "error: " <<  result;
    //QMessageBox::critical(this, "Ошибка", result);
    ui->txtTerminal->setText(ui->txtTerminal->toPlainText() +  "\nerror:"  + result);

    if(command == CmdCommand::wmicGetSID){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }
}

void MainWindow::onCommandLineStart()
{
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
#else
        std::string envUSER = "USER";
        QString cryptoProDir = "/opt/cprocsp/bin/amd64/";
        terminal->send("echo $USER\n", 1); //CommandLine::cmdCommand::echoUserName);// ; exit\n
#endif

        createWsObject();

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
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
    qDebug() << __FUNCTION__ << command;
//    qDebug() <<  __FUNCTION__ << recipient << command;
//    qDebug() <<  __FUNCTION__ << qPrintable(message);
    if(command == AvailableContainers){
        QString s = fromBase64(message.toUtf8());
        int iUuid = modelWsUsers->getColumnIndex("uuid");
        auto index = findInTable(modelWsUsers, recipient, iUuid, false);
        if(index.isValid()){
            QStringList containers = s.split("\n");
            int iName = modelWsUsers->getColumnIndex("user_name");
            int iHost = modelWsUsers->getColumnIndex("host_name");
            QString name = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iName).toString();
            QString host = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iHost).toString();
            auto itr = m_users.find(qMakePair(name, host));
            if(itr != m_users.end()){
                CertUser *usr = itr.value();
                usr->setContainers(containers);
                if(ui->treeWidget->currentIndex().isValid())
                    emit ui->treeWidget->itemClicked(ui->treeWidget->currentItem(), 0);
            }
        }
    }else if(command == mplCertData){
        wsSetMplCertData(recipient, message);
    }else if(command == ToDatabase){
        //getDataContainersList();
    }else if(command == ToVolume || command == ToRegistry){
        QByteArray msg = QByteArray::fromBase64(message.toUtf8());
        auto obj = QJsonDocument::fromJson(msg).object();
        addContainer(obj.value("from").toString(), command, obj.value("data").toString());
    }else if(command == "deleteContainer"){
        if(message == "Ошибка удаления контейнера!"){
            QMessageBox::critical(this, "Ошибка", "Ошибка удаления контейнера! Возможно не достаточно прав у клиента!");
        }else{
            QMessageBox::information(this, "Удаление контейнера", "Контейнер успешно удален!");
            sendToRecipient(recipient, "get_available_containers", "get_available_containers", false);
        }
    }else if(command == "deleteCertificate"){
        if(message != "sucsess"){
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить сертификат!");
        }else{
            QMessageBox::information(this, "Удаление сертификата", "Сертификат успешно удален!");
            sendToRecipient(recipient, "get_installed_certificates", "get_installed_certificates", false);
        }
    }else if(command == InstallContainerToRemoteUser){
        if(message == "Ошибка выполнения операции!"){
            QMessageBox::critical(this, "Ошибка", "Ошибка установки контейнера! Возможно не достаточно прав у клиента!");
        }else{
            QMessageBox::information(this, "Установка контейнера", "Контейнер успешно установлен!");
            sendToRecipient(recipient, "get_available_containers", "get_available_containers", false);
        }
    }else if(command == InstalledCertificates){
        QString result = QByteArray::fromBase64(message.toUtf8());
        auto doc = QJsonDocument::fromJson(result.toUtf8());
        if(doc.isEmpty())
            return;

        auto obj = doc.object();
        auto certs = obj.value("certs").toObject();
        int iUuid = modelWsUsers->getColumnIndex("uuid");
        auto index = findInTable(modelWsUsers, recipient, iUuid, false);
        if(index.isValid()){
            int iName = modelWsUsers->getColumnIndex("name");
            int iHost = modelWsUsers->getColumnIndex("host_name");
            QString name = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iName).toString();
            QString host = modelWsUsers->index(index.row(), iName).data(Qt::UserRole + iHost).toString();
            auto itr = m_users.find(qMakePair(name, host));
            if(itr != m_users.end()){
                CertUser *usr = itr.value();
                usr->certsFromModel(certs);
                if(ui->treeWidget->currentIndex().isValid())
                    emit ui->treeWidget->itemClicked(ui->treeWidget->currentItem(), 0);
            }
        }
    }else if(command == "addCertificate"){
        //qDebug() << __FUNCTION__ << command  << message;
        if(message == "error"){
            QMessageBox::critical(this, "Ошибка", "Ошибка выполнения операции!");
        }else{
            addCertificateFromBase64ToDatabase(message);
            //QMessageBox::information(this, "Установка сертификата", "Сертификат успешно установлен!");
//            sendToRecipient(recipient, "get_installed_certificates", "get_installed_certificates", false);
        }
    }else if(command == ToRemoteRegistry){
        //qDebug() << __FUNCTION__ << command  << message;
        if(message == "error"){
            QMessageBox::critical(this, "Ошибка", "Ошибка выполнения операции!");
        }else{
            QMessageBox::information(this, "Установка контейнера", "Контейнер успешно установлен у пользователя!");
        }
    }else
        qDebug() << __FUNCTION__ << command << message;
}

void MainWindow::onWsMplClientFormLoaded(const QString &resp)
{
    qDebug() << __FUNCTION__ << resp;
}

void MainWindow::onWsExecQuery(const QString &result)
{
    // << result;

    auto doc = QJsonDocument::fromJson(result.toUtf8());
    auto obj = doc.object();
    if(doc.isEmpty()){
        qDebug() << __FUNCTION__;
        return;
    }

    QString id_command = obj.value("id_command").toString();

    qDebug() << __FUNCTION__ << id_command;

    if(id_command == "DataContainersList" || id_command == "DataCertificatesList" ){
        resetTableJsonModel(obj, id_command);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else if(id_command == insertContainerToData){
        qDebug() << __FUNCTION__ << "Контейнер успешно импортирован в на сервер!";
        getDataContainersList();
    }else if(id_command == deleteContainerFromData){
        qDebug() << __FUNCTION__ << "Контейнер успешно удален с сервера!";
        getDataContainersList();
    }else if(id_command == DataUsersList){\
        resetTableJsonModel(obj, id_command);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else if(id_command == "get_data"){
        QString base64 = obj.value("table").toString();
        if(base64.isEmpty())
            return;
        QString json = fromBase64(base64.toUtf8());
        QString run_on_return = obj.value("run_on_return").toString();
        if(!run_on_return.isEmpty()){
            onGetDataFromDatabase(json, run_on_return);
        }
    }else if(id_command == insertCertificateToData){
        qDebug() << __FUNCTION__ << "Сертификат успешно импортирован в на сервер!";
        QMessageBox::information(this, "Импорт сертификата", "Сертификат успешно импортирован в на сервер!");
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

    if(!db->isOpen())
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
        if(!db->isOpen())
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
        int iHost = modelWsUsers->getColumnIndex("host_name");
        int iName = modelWsUsers->getColumnIndex("user_name");
        int iUuid = modelWsUsers->getColumnIndex("user_uuid");
        auto user = modelWsUsers->index(row, iName).data(Qt::UserRole + iName).toString();
        auto host = modelWsUsers->index(row, iHost).data(Qt::UserRole + iHost).toString();
        auto uuid = modelWsUsers->index(row, iUuid).data(Qt::UserRole + iUuid).toString();

        if(isCertUserExists(host, user)){
            QMessageBox::critical(this, "Ошибка", "Пользователь уже зарегистрирован в базе!");
            return;
        }else{
            auto result =  QMessageBox::question(this, "Регистрация пользователя в базе", "Зарегистрировать пользователя в базе для использования сертификатов?");
            if(result != QMessageBox::Yes){
                return;
            }
        }
        QUuid _ref = QUuid::createUuid();
        QString ref = _ref.toString().mid(1, 36);
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
            bindQuery.addField("Ref", ref);
            bindQuery.addField("host", host);
            bindQuery.addField("uuid", uuid);

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
        addCertificate("", ToDatabase);
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
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    if(node == currentUserRegistry || node == currentUserDivace){
        int ind = modelUserContainers->getColumnIndex("nameInStorgare");
        auto object = proxyModelUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        object.replace("\r", "");
        addContainer(object, ToVolume);
    }else if(node == currentUserCertificates){
        saveAsCurrentUserCertificate();
    }else{
        auto m_key = node.split("/");
        QString name = m_key[0];
        QString host = m_key[1];
        QString Volume = ToRemoteVolume;
        QString object, command = "addContainer";
        if(node.left(4) == "reg_" || node.left(4) == "vol_"){
            int ind = modelCertUserContainers->getColumnIndex("nameInStorgare");
            object = proxyModeCertlUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
            object.replace("\r", "");
            name.replace(node.left(4), "");
        }else if(node.left(5) == "cert_"){
            int ind = modelCertUserCertificates->getColumnIndex("sha1");
            object = modelCertUserCertificates->index(index.row(), ind).data().toString();
            if(object.isEmpty()){
                qCritical() << __FUNCTION__ << "Слепок сертификата не найден!";
                return;
            }
            object.replace("cert_", "");
            object = QString(STORGARE_LOCALHOST) + "/" + object;
            Volume = ToRemoteCertificate;
            command = "addCertificate";
            name.replace(node.left(5), "");
        }
        QString sess = getSessionUuid(name, host);
        qDebug() << sess << name << host;
        auto param = QJsonObject();
        param.insert("command", command);
        param.insert("from", object);
        param.insert("to", Volume);

        sendToRecipient(sess, command, QJsonDocument(param).toJson().toBase64(), true);

    }


}


void MainWindow::on_btnCurrentCopyToRegistry_clicked()
{

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();
    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }
    if(node == currentUserRegistry || node == currentUserDivace){
        int ind = modelUserContainers->getColumnIndex("nameInStorgare");
        auto object = proxyModelUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        object.replace("\r", "");
        addContainer(object, ToRegistry);
    }else if(node == currentUserCertificates){
        //saveAsCurrentUserCertificate();
    }else{
        int ind = modelCertUserContainers->getColumnIndex("nameInStorgare");
        auto object = proxyModeCertlUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        object.replace("\r", "");
        auto m_key = node.split("/");
        QString name = m_key[0];
        QString host = m_key[1];
        if(node.left(4) == "reg_"){
            name.replace("reg_", "");
        }else if(node.left(4) == "vol_"){
            name.replace("vol_", "");
        }else if(node.left(5) == "cert_"){
            name.replace("cert_", "");
        }
        QString sess = getSessionUuid(name, host);
        auto param = QJsonObject();
        param.insert("command", "addContainer");
        param.insert("from", object);
        param.insert("to", ToRemoteRegistry);

        sendToRecipient(sess, "addContainer", QJsonDocument(param).toJson().toBase64(), true);

    }

}

void MainWindow::on_btnCurrentCopyToSql_clicked()
{
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();   auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    QString object;

    if(node == currentUserRegistry || node == currentUserDivace){
        int ind = modelUserContainers->getColumnIndex("nameInStorgare");
        object = proxyModelUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        object.replace("\r", "");
        addContainer(object, ToDatabase);
    }else if(node == currentUserCertificates){
        int ind = modelUserCertificates->getColumnIndex("sha1");
        object = modelUserCertificates->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        object.replace("\r", "");
        if(QMessageBox::question(this, "Копирование на сервер", QString("Копировать сертификат sha1: %1 на сервер?").arg(object)) == QMessageBox::No)
            return;
        addCertificate(STORGARE_LOCALHOST, STORGARE_DATABASE, object);
    }else{
        auto m_key = node.split("/");
        QString name = m_key[0];
        QString host = m_key[1];
        QString objType;
        QString command = "addContainer";
        if(node.left(4) == "reg_"){
            name.replace("reg_", "");
            objType = "cnt";
        }else if(node.left(4) == "vol_"){
            name.replace("vol_", "");
            objType = "cnt";
        }else if(node.left(5) == "cert_"){
            name.replace("cert_", "");
            objType = "cert";
        }

        if(objType == "cnt"){
            int ind = modelUserContainers->getColumnIndex("nameInStorgare");
            object = proxyModelUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
            object.replace("\r", "");
        }else{
            int ind = modelCertUserCertificates->getColumnIndex("sha1");
            object = modelCertUserCertificates->index(index.row(), ind).data(Qt::UserRole + ind).toString();
            object.replace("\r", "");
            command = "addCertificate";
        }

        QString sess = getSessionUuid(name, host);
        auto param = QJsonObject();
        param.insert("command", command);
        param.insert("from", QString(STORGARE_LOCALHOST) + "/" + object);
        param.insert("to", STORGARE_DATABASE);

        sendToRecipient(sess, command, QJsonDocument(param).toJson().toBase64(), true);
    }

//    QString volume = table->model()->index(index.row(), 1).data().toString();
//    QString name = table->model()->index(index.row(), 2).data().toString();

//    QDir folder;

//    if(QString(volume).left(6) == "FAT12_"){
//        QString tom = QString(volume).right(volume.length() - 6);

//        QStorageInfo storage(tom + ":" + QDir::separator());
//        if(!storage.isReady())
//        {
//            QMessageBox::critical(this, "Ошибка", QString("Не возможно прочитать данные с устройства '%1'").arg(volume));
//            return;
//        }

//        QStringList lst = name.split("@");
//        folder = QDir(storage.rootPath() + QDir::separator() +  lst[0] + ".000");

//        if(!folder.exists()){
//            return;
//        }
//    }
//    KeysContainer cnt = KeysContainer(this);
//    cnt.setName(name);
//    if(QString(volume).left(6) == "FAT12_"){
//        cnt.fromFolder(folder.path());
//    }else if(volume == "REGISTRY"){
//        if(currentUser->sid().isEmpty()){
//            qCritical() << __FUNCTION__ << "Требуется SID!";
//            return;
//        }
//        cnt.fromRegistry(currentUser->sid(), cnt.bindName());
//    }

//    if(!cnt.isValid())
//    {
//        qCritical() << __FUNCTION__ << "Ошибка: Ошибка загрузки данных контейнера с устройства!";
//        return;
//    }else{
//        if(isContainerExists(name)){
//            QMessageBox::critical(this, "Ошибка", QString("Контейнер с именем '%1' уже есть на сервере!").arg(name));
//            return;
//        }
//        auto bOK =  QMessageBox::question(this, "Экспорт контейнера", QString("Экспортировать на сервер контейнер %1?").arg(name));
//        if(bOK == QMessageBox::No){
//            return;
//        }

//        QString uuid = QUuid::createUuid().toString();
//        uuid = uuid.mid(1, uuid.length() - 2);
//        QByteArray data = cnt.toBase64();

//        auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlInsert, "Containers");
//        bindQuery.addField("Ref", uuid);
//        bindQuery.addField("FirstField", QString(name.toUtf8().toBase64()));
//        bindQuery.addField("SecondField", name);
//        bindQuery.addField("data", QString(data.toBase64()));

//        bindQuery.addFieldIsExists("FirstField", QString(name.toUtf8().toBase64()));
//        bindQuery.addFieldIsExists("SecondField", name);

//        if(_sett->launch_mode() == mixed){
//            QSqlQuery sql = bindQuery.query(db->getDatabase());
//            sql.exec();
//            if(sql.lastError().type() != QSqlError::NoError){
//                qDebug() << __FUNCTION__ << sql.lastError().text();
//            }else{
//                QMessageBox::information(this, "Копирование на сервер", "Контейнер успешно скопирован на сервер!");
//                getDataContainersList();
//            }
//        }else{
//            if(m_client->isStarted()){
//                QString result = bindQuery.to_json();
//                auto doc = QJsonDocument();
//                auto objMain = QJsonObject();
//                objMain.insert("query", result);
//                objMain.insert("id_command", insertContainerToData);
//                doc.setObject(objMain);
//                QString param = doc.toJson();
//                m_client->sendCommand("exec_query_qt", "", param);
//            }
//        }

//    }

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

    auto model = (QJsonTableModel*)table->model();
    int iRef = model->getColumnIndex("Ref");
    int iName = model->getColumnIndex("SecondField");
    QString ref = model->index(index.row(), iRef).data(Qt::UserRole + iRef).toString();
    QString name = model->index(index.row(), iName).data(Qt::UserRole + iName).toString();

   if(QMessageBox::question(this, "Удаление объекта", QString("Удалить объект '%1' с сервера?").arg(name)) == QMessageBox::No)
       return;

    if(node == SqlContainers){
        auto cnt = KeysContainer();
        if(_sett->launch_mode() == mixed){
            if(cnt.deleteContainer(db, ref)){
                QMessageBox::information(this, "Удаление контейнера", "Контейнер успшно удален!");
                getDataContainersList();
            }else
                QMessageBox::critical(this, "Удаление контейнера", "Ошибка удаления контейнера!");
        }else{
            cnt.deleteContainer(m_client, ref);
        }
    }if(node == SqlCertificates){
        delCertificate();
    }if(node == SqlUsers){
        delCertUser();
    }
}


void MainWindow::on_btnConInfo_clicked()
{
    qDebug() << __FUNCTION__;
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    if(node == currentUserDivace || node == currentUserRegistry){
        int col = modelUserContainers->getColumnIndex("nameInStorgare");

        QModelIndex _index = table->model()->index(index.row(), col);

        QString device = _index.model()->data(_index, Qt::UserRole + col).toString().replace("\r", "");


        QString cmd = QString("csptest -keyset -container \"%1\" -info").arg(device);
        terminal->send(cmd, csptestContainerFnfo);
    }else if(node == currentUserCertificates){
        int col = modelUserCertificates->getColumnIndex("serial");
        QString serial = modelUserCertificates->index(index.row(), col).data().toString();
        if(!serial.isEmpty()){
            auto iter = currentUser->certificates().find(serial);
            if(iter != currentUser->certificates().end()){
                auto dlg = DialogContainerInfo(iter.value()->getSourceObject(), iter.value()->bindName(), this);
                dlg.setModal(true);
                dlg.exec();
            }
        }
    }
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
        QMessageBox::critical(this, "Ошибка", "Не выбран объект!");
        return;
    }

    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();


    if(node == currentUserDivace || node == currentUserRegistry){

        QString deleteKey = node == currentUserRegistry ? "из реестра" : "с устройства";

        int ind = modelUserContainers->getColumnIndex("nameInStorgare");
        auto container = proxyModelUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();

        //QString name = table->model()->index(index.row(), 2).data().toString();
        auto result =  QMessageBox::question(this, "Удаление контейнера", QString("Удалить контейнер: \n%1 \n%2?").arg(container, deleteKey));

        if(result != QMessageBox::Yes){
            return;
        }

        QModelIndex _index = table->model()->index(index.row(), 1);
        QString device = _index.model()->data(_index, Qt::UserRole + 1).toString().replace("\r", "");

        QString cmd = QString("csptest -keyset -deletekeyset -container \"%1\"\n").arg(container);
        terminal->send(cmd, CmdCommand::csptestContainerDelete);

    }else if(node == currentUserCertificates){
        int col = modelUserCertificates->getColumnIndex("serial");
        QString serial = modelUserCertificates->index(index.row(), col).data().toString();
        if(!serial.isEmpty()){
            auto iter = currentUser->certificates().find(serial);
            if(iter != currentUser->certificates().end()){
                auto result =  QMessageBox::question(this, "Удаление сертификата", QString("Удалить сертификат: \n%1?").arg(iter.value()->bindName()));

                if(result != QMessageBox::Yes){
                    return;
                }

                if(iter != currentUser->certificates().end()){
                    QString sha1 = iter.value()->sha1Hash();
                    QString _qbyte = QString("certmgr -delete -thumbprint \"%1\"\n").arg(sha1);
                    if(!sha1.isEmpty()){
                        terminal->send(_qbyte, certmgrDeletelCert);
                    }
                }
            }
        }
    }else{

        QString Volume;
        QString command;
        QString msg;

        QMap<QString, QString> nodeParam = remoteItemParam(index, node);

        if(nodeParam["key"] == remoteUserRegistry){
            Volume = ToRemoteRegistry;
            command = "deleteContainer";
            msg = QString("Удалить контейнер: \n%1 \n%2?").arg(nodeParam["object"], "из реестра пользователя");
        }else if(nodeParam["key"] == remoteUserContainers){
            Volume = ToRemoteVolume;
            command = "deleteContainer";
            msg = QString("Удалить контейнер: \n%1 \n%2?").arg(nodeParam["object"], "с устройства у пользователя");
        }else if(nodeParam["key"] == remoteUserCertificates){
            Volume = ToRemoteCertificate;
            command = "deleteCertificate";
            msg = QString("Удалить сертификат sha1: \n%1").arg(nodeParam["object"]);
        }

        auto result =  QMessageBox::question(this, "Удаление объекта", msg);

        if(result != QMessageBox::Yes){
            return;
        }

        QString sess = getSessionUuid(nodeParam["name"], nodeParam["host"]);
        auto param = QJsonObject();
        param.insert("command", command);
        param.insert("from", nodeParam["object"]);
        param.insert("to", Volume);

        sendToRecipient(sess, command, QJsonDocument(param).toJson().toBase64(), true);


//        QString object;
//        QString _object;
//        auto m_key = node.split("/");
//        QString name = m_key[0];
//        QString host = m_key[1];

//        if(node.left(4) == "reg_"){
//            int ind = modelCertUserContainers->getColumnIndex("nameInStorgare");
//            object = proxyModeCertlUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
//            object.replace("\r", "");
//            name.replace("reg_", "");
//            deleteKey = "из реестра пользователя";
//            Volume = ToRemoteRegistry;
//            command = "deleteContainer";

//        }else if(node.left(4) == "vol_"){
//            int ind = modelCertUserContainers->getColumnIndex("nameInStorgare");
//            object = proxyModeCertlUserConteiners->index(index.row(), ind).data(Qt::UserRole + ind).toString();
//            object.replace("\r", "");
//            name.replace("vol_", "");
//            deleteKey = "с устройства у пользователя";
//            Volume = ToRemoteVolume;
//            command = "deleteContainer";
//        }else if(node.left(5) == "cert_"){
//            int ind = modelCertUserCertificates->getColumnIndex("sha1");
//            object = modelCertUserCertificates->index(index.row(), ind).data().toString();
//            if(object.isEmpty()){
//                qCritical() << __FUNCTION__ << "Слепок сертификата не найден!";
//                return;
//            }

//            _object = "сертификат sha1:" + object;
//            name.replace("cert_", "");
//            deleteKey = "у пользователя";
//            Volume = ToRemoteCertificate;
//            command = "deleteCertificate";
//        }

//        if(_object.isEmpty())
//            _object = object;
//        auto result =  QMessageBox::question(this, "Удаление объекта", QString("Удалить: \n%1 \n%2?").arg(_object, deleteKey));

//        if(result != QMessageBox::Yes){
//            return;
//        }


    }
}


void MainWindow::on_btnDatabaseSaveAs_clicked()
{
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();

    auto table = ui->tableView;
    auto index = table->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбран контейнер!");
        return;
    }

    if(node == SqlContainers){
        int ind = modelSqlContainers->getColumnIndex("Ref");
        auto ref = modelSqlContainers->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        ind = modelSqlContainers->getColumnIndex("FirstField");
        auto container = modelSqlContainers->index(index.row(), ind).data(Qt::UserRole + ind).toString();
        container.replace("\r", "");
        if(ref.isEmpty()){
            QMessageBox::critical(this, "Ошибка", "Идентификатор контейнера не действителен!");
            return;
        }
        auto param = QJsonObject();
        param.insert("command", "addContainer");
        param.insert("from", QString("\\\\.\\DATABASE\\%1").arg(container));
        param.insert("to", ToVolume);
        getDatabaseData("Containers", ref, param);
    }

//    if(node == SqlContainers){
//        //saveAsDatabaseContainer();

//    }else if(node == SqlCertificates){
//        saveAsDatabaseCertificate();
//    }
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
        if(!db->isOpen())
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
    auto rows = _table.value("rows").toArray();
    if(rows.isEmpty()){
        qCritical() << __FUNCTION__ << "Контейнер на сервере не найден!";
        return;
    }

    auto row = rows[0].toObject();
    QString dataBase64 = row.value("data").toString();
    QString command = _param.value("command").toString();
    QString name = _param.value("FirstField").toString();

    if(command == "addContainer"){
        QString from = _param.value("from").toString();
        if(!name.isEmpty())
            from = from + name;
        QString to = _param.value("to").toString();
        addContainer(from, to, dataBase64);
    }



//        QByteArray data = QByteArray::fromBase64(dataBase64.toUtf8());
//        auto cnt = KeysContainer();
//        cnt.fromJson(data);
//        if(!cnt.isValid()){
//            QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");
//            return;
//        }
//        cnt.fromContainerName(device);
//        bool result = cnt.sync(currentUser->sid());
//        if(result){
//            QMessageBox::information(this, "Копирование контейнера", "Контейнер успешно скопирован!");
//            terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
//        }else
//            QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");







//    if(command == "copy_container_from_data"){
//        QString device = _param.value("toDevice").toString();
//        if(device.isEmpty()){
//            return;
//        }else
//        {
//            if(currentUser->sid().isEmpty())
//                return;
//            auto rows = _table.value("rows").toArray();
//            auto row = rows[0].toObject();
//            if(!row.isEmpty()){
//                QString dataBase64 = row.value("data").toString();
//                QByteArray data = QByteArray::fromBase64(dataBase64.toUtf8());
//                auto cnt = KeysContainer();
//                cnt.fromJson(data);
//                if(!cnt.isValid()){
//                    QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");
//                    return;
//                }
//                cnt.fromContainerName(device);
//                bool result = cnt.sync(currentUser->sid());
//                if(result){
//                    QMessageBox::information(this, "Копирование контейнера", "Контейнер успешно скопирован!");
//                    terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
//                }else
//                    QMessageBox::critical(this, "Ошибка", "При копировании контейнера произошла ошибка!");
//            }
//        }
//    }

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

             int row = modelWsUsers->row(qMakePair(name, host));
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

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString node = treeItem->data(0, Qt::UserRole).toString();

    if(node == currentUserCertificates){
        QStringList select = {
            "Из хранилища сертификатов на сервере",
            "Из файла"
        };
        auto dlg = DialogSelectDevice(this, select, "Добавить сертификат");
        dlg.setModal(true);
        dlg.exec();

        if(dlg.result() != QDialog::Accepted){
            return;
        }
        //вызываем процедуру без указания ключа, откроется диалог выбора файла
        addCertificate(dlg.currentSelection() == 0 ? STORGARE_DATABASE : STORGARE_LOCALHOST, STORGARE_LOCALHOST);

    }else if(node == currentUserDivace || node == currentUserRegistry){
        QStringList dlgResult = getObjectsFromdataBase();
        if(dlgResult.size() == 0)
            return;
        QString ref = dlgResult[1];
        auto param = QJsonObject();
        param.insert("command", "addContainer");
        param.insert("from", "\\\\.\\DATABASE\\");
        param.insert("to", node == currentUserDivace ? ToVolume : ToRegistry);
        getDatabaseData("Containers", ref, param);
    }else{
        QString Volume;
        QString command;

        QMap<QString, QString> nodeParam = remoteItemParam(QModelIndex(), node);

        if(nodeParam["key"] == remoteUserRegistry){
            Volume = ToRegistry;
            command = "addContainer";
        }else if(nodeParam["key"] == remoteUserContainers){
            Volume = ToVolume;
            command = "addContainer";
        }else if(nodeParam["key"] == remoteUserCertificates){
            Volume = ToCertificate;
            command = "addCertificate";
        }

        QStringList dlgResult = getObjectsFromdataBase(); // dlg.dialogResult();
        if(dlgResult.size() == 0)
            return;
        QString ref = dlgResult[1];

        QString sess = getSessionUuid(nodeParam["name"], nodeParam["host"]);
        auto param = QJsonObject();
        param.insert("command", command);
        param.insert("from", FromDatabase);
        param.insert("to", Volume);
        param.insert("ref", ref);

        sendToRecipient(sess, command, QJsonDocument(param).toJson().toBase64(), true);

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
    if(m_client->isStarted())
        m_client->sendCommand("mpl_form_loaded");

}

void MainWindow::onStartGetCertUsersData()
{
    if(m_queue.size() == 0)
        return;

    //получим по очереди данные о контейнерах и сертификатх с удаленных клиентов
    auto uuid = m_queue.dequeue();
    sendToRecipient(uuid, "get_crypt_data", "get_crypt_data", false);

    if(m_queue.size() == 0){
        updateStatusBar();
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
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

void MainWindow::resetInfoUserContainers(CertUser* usr)
{
    qDebug() << __FUNCTION__;

    QJsonTableModel* model = nullptr;
    CertUser* _usr;
    if(!usr){
       model = modelUserContainers;
       _usr = currentUser;
    }else{
       model = modelCertUserContainers;
       _usr = usr;
    }
    //обновляем иформацию о контейнерах у пользователя
    bool isReset = false;
    int iSqlName = modelSqlContainers->getColumnIndex("FirstField");
    int iName = model->getColumnIndex("name");
    int indNameInStorgare = model->getColumnIndex("nameInStorgare");
    int indCache = modelSqlContainers->getColumnIndex("cache");

    for(int i = 0; i < model->rowCount(); ++i){
        QString name = model->index(i, iName).data(Qt::UserRole + iName).toString();
        QString storgare = model->index(i, indNameInStorgare).data(Qt::UserRole + indNameInStorgare).toString();
        auto index = findInTable(modelSqlContainers, name, iSqlName, false);
        if(index.isValid()){
            KeysContainer * cnt = _usr->container(storgare);
            if(cnt){
               if(cnt->cache().isEmpty()){
                   QString cache = modelSqlContainers->index(index.row(), indCache).data(Qt::UserRole + indCache).toString();
                   if(!cache.isEmpty()){
                        cnt->infoFromDataBaseJson(cache);
                        isReset = true;
                   }
               }

            }
        }
    }

    if(isReset){
        QString json = _usr->modelContainersText();
        model->setJsonText(json);
        model->reset();
    }

}


void MainWindow::on_toolCurrentUserUpdate_clicked()
{

    auto treeItem = ui->treeWidget->currentItem();
    if(!treeItem){
        return;
    }

    QString currentNode = treeItem->data(0, Qt::UserRole).toString();

    qDebug() << __FUNCTION__ << currentNode;
    resetCertData(currentUser, currentNode);


}


void MainWindow::on_mnuAbout_triggered()
{
    auto dlg = DialogAbout(this);
    dlg.setModal(true);
    dlg.exec();
}


void MainWindow::on_btnDataListUpdate_clicked()
{
    auto tree = ui->treeWidget;
    QString node = tree->currentItem()->data(0, Qt::UserRole).toString();


    if(node == SqlContainers){

        getDataContainersList();

    }else if(node == SqlCertificates){

    }else if(node == SqlUsers){

    }
}
