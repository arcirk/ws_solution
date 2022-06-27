#include "dialogmainwindow.h"
#include "ui_dialogmainwindow.h"

#include <QStandardPaths>
#include <QFile>
#include <QSettings>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QStorageInfo>
#include <QToolButton>
#include <QJsonDocument>
#include <QJsonObject>
#include "dialogselectprofile.h"
#include "userprofile.h"
#include <QJsonArray>
#include "dialogselectedrow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QSqlError>
#include "dialogconnection.h"
#include <tabledelegate.h>

//const static QString Cyrillic = "йцукенгшщзхъфывапролджэячсмитьё"
//        "ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁ";

//static bool isCyrillic(const QString &source)
//{
//    for (int i = 0; i < source.length();  ++i) {

//        if(Cyrillic.indexOf(source.mid(i, 1)) != -1)
//            return true;
//    }

//    return false;
//}

DialogMainWindow::DialogMainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Настройки профилей пользователя");

//    connect(this, &DialogMainWindow::whenDataIsLoaded, this, &DialogMainWindow::onWhenDataIsLoaded);
//    connect(this, &DialogMainWindow::endInitConnection, this, &DialogMainWindow::onEndInitConnection);

    _profiles = nullptr;
    terminal = nullptr;
    m_client = nullptr;
    db = nullptr;

    formControl();

    initCsptest();

    _sett = new Settings(this, appHome.path());
    if(_sett->httpHost().isEmpty())
        _sett->setHttpHost(defaultHttpaddrr);

    createConnectionsObjects();

    createColumnAliases();

    m_async_await.append(std::bind(&DialogMainWindow::currentUserSid, this));
    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetConteiners, this));
    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetCertificates, this));
    m_async_await.append(std::bind(&DialogMainWindow::createWsObject, this));
    m_async_await.append(std::bind(&DialogMainWindow::getSettingsFromHttp, this));
    m_async_await.append(std::bind(&DialogMainWindow::connectToWsServer, this));
    m_async_await.append(std::bind(&DialogMainWindow::connectToDatabase, this));
    m_async_await.append(std::bind(&DialogMainWindow::initProfiles, this));
    m_async_await.append(std::bind(&DialogMainWindow::getUserData, this));
    m_async_await.append(std::bind(&DialogMainWindow::setProfilesModel, this));
    m_async_await.append(std::bind(&DialogMainWindow::getAvailableCerts, this));

    currentUser = new CertUser(this);
    QString curentHost = QSysInfo::machineHostName();
    currentUser->setDomain(curentHost);

    mozillaApp = new QProcess(this);

    //запуск асинхронных вызовов
    createTerminal();

}

void DialogMainWindow::createConnectionsObjects()
{
    qDebug() << __FUNCTION__;

    db = new SqlInterface(this);

    //createWsObject(/*usr, pwd */);

}

DialogMainWindow::~DialogMainWindow()
{
    delete ui;
}

void DialogMainWindow::accept()
{

    updateData();
    if(_profiles){
        _profiles->saveSettings();        
    }
    createDynamicMenu();

    QDialog::accept();
}

void DialogMainWindow::setVisible(bool visible)
{
    QDialog::setVisible(visible);
}

void DialogMainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    //qDebug() << exitEvent;

    if (trayIcon->isVisible()) {
        QMessageBox::information(this, tr("Менеджер профилей"),
                                 tr("Программа продолжит работу в фоновом режиме "
                                    "Выберете <b>Выход</b> в контекстном меню системного трея"
                                    " для выхода."));
        hide();
        event->ignore();
    }
}

void DialogMainWindow::updateData()
{
//    if(!_profiles)
//        return;
//    QTableWidget * table = ui->tableWidget;
//    _profiles->clear();

//    for (int row = 0; row < table->rowCount(); row++) {
//        auto prof = new UserProfile();
//        auto line = getLineEdit(row, 0);
//        if(line)
//            prof->setProfile(line->text());
//        auto item = ui->tableWidget->item(row, 1);
//        if(item)
//           prof->setName(item->text());
//        line = getLineEdit(row, 2);
//        if(line)
//            prof->setDefaultAddress(line->text());
//        item = ui->tableWidget->item(row, 4);
//        if(item)
//           prof->setUuid(QUuid::fromString(item->text()));

//        _profiles->setProfile(prof);
//    }

}


void DialogMainWindow::setRow(UserProfile *prof)
{
//    QTableWidget * table = ui->tableWidget;
//    table->setRowCount(table->rowCount() + 1);
//    int row = table->rowCount() -1;

//    //имя профиля
//    auto *pWidget = getItemWidget(prof->profile(), row, 0, SLOT(onSelectProfile()));
//    table->setCellWidget(table->rowCount()-1,0,pWidget);
//    //вид операции
//    auto itemName = new QTableWidgetItem(prof->name());
//    table->setItem(row, 1, itemName);
//    //Страница по умолчанию
//    pWidget = getItemWidget(prof->defaultAddress(), row, 2, SLOT(onSelectDefaultAddress()));
//    table->setCellWidget(table->rowCount()-1,2,pWidget);
//    //Сертификаты
//    pWidget = getItemWidget(prof->certToString(), row, 3, SLOT(onSelectDefaultCertificate()));
//    table->setCellWidget(table->rowCount()-1,3,pWidget);
//    //Идентификатор
//    auto itemUuid = new QTableWidgetItem(prof->uuid().toString());
//    itemUuid->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//    table->setItem(row, 4, itemUuid);

    _profiles->setProfile(prof);

    modelMplProfiles->addRow(prof->to_modelObject());
    modelMplProfiles->reset();
    ui->tableView->resizeColumnsToContents();
    updateDataUserCache();

}

void DialogMainWindow::updateRow(UserProfile *prof, int row)
{
      modelMplProfiles->updateRow(prof->to_modelObject(), row);
      modelMplProfiles->reset();
      ui->tableView->resizeColumnsToContents();
      updateDataUserCache();
}

void DialogMainWindow::setProfile(UserProfile *prof)
{
    auto items = _profiles->profiles();
    items.insert(prof->uuid(), prof);
}

//QLineEdit *DialogMainWindow::getLineEdit(int row, int col)
//{
////    QWidget* pWidget = ui->tableWidget->cellWidget(row, col);
////    if(!pWidget)
////        return nullptr;
////    auto *line = pWidget->findChild<QLineEdit*>("lineEdit" + QString::number(row));
////    return line;
//}

//QWidget *DialogMainWindow::getItemWidget(const QString &text, int row, int col, const char* member)
//{
//    auto *pWidget = new QWidget();
//    auto *pToolBtn = new QToolButton();
//    auto *pLayoutVal = new QHBoxLayout(pWidget);
//    auto *pLineEdit = new QLineEdit();

//    pToolBtn->setAutoRaise(true);
//    pToolBtn->setText("...");
//    pToolBtn->setProperty("row", row);
//    pToolBtn->setProperty("col", col);
//    pToolBtn->setObjectName("btn" + QString::number(row));
//    pLineEdit->setObjectName("lineEdit" + QString::number(row));
//    pLineEdit->setFrame(false);
//    pLineEdit->setReadOnly(true);
//    pLineEdit->setProperty("row", row);
//    pLineEdit->setProperty("col", col);

//    pLayoutVal->addWidget(pLineEdit);
//    pLayoutVal->addWidget(pToolBtn);
//    pLayoutVal->setContentsMargins(0,0,0,0);
//    pWidget->setLayout(pLayoutVal);
//    pLineEdit->setText(text);

//    connect(pToolBtn, SIGNAL(clicked()), this, member);
//    connect(pLineEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onLineEditCursorPositionChanged(int, int)));

//    return pWidget;
//}

void DialogMainWindow::formControl()
{
    qDebug() << __FUNCTION__;

    ui->tableView->setItemDelegate(new TableDelegate);
    ui->tableView->setIconSize(QSize(16,16));

    QString dir = QDir::homePath();

#ifdef Q_OS_WINDOWS
    dir.append("/AppData/Roaming/");
    dirName = "mpl/";
#else
    dirName = ".mpl";
#endif

    appHome = QDir(dir + dirName);

    if(!appHome.exists())
       appHome.mkpath(".");

    infoBar = ui->lblStatus;

//


//    QTableWidget * table = ui->tableWidget;
//    table->setColumnCount(5);
//    table->setHorizontalHeaderLabels(QStringList{"Профиль", "Вид операции", "Адрес", "Сертификаты", "Идентификатор"}); // "IsRelative", "Default"});
//    table->setColumnHidden(3, true);
//    table->setColumnHidden(4, true);

//





//    updateFromData();

//    table->resizeColumnToContents(0);
//    table->resizeColumnToContents(1);
//    table->setSelectionMode(QTableWidget::SingleSelection);

    createTrayActions();
    createTrayIcon();
    trayIcon->show();

    //ui->tableWidget->resizeRowsToContents();
}

void DialogMainWindow::createTrayActions()
{
    quitAction = new QAction(tr("&Выйти"), this);
    connect(quitAction, &QAction::triggered, this, &DialogMainWindow::onAppExit);
    showAction = new QAction(tr("&Открыть менеджер профилей"), this);
    connect(showAction, &QAction::triggered, this, &DialogMainWindow::onWindowShow);

    trayIconMenu = new QMenu(this);

//    if(!_profiles){
//        trayIconMenu->addAction(showAction);
//        trayIconMenu->addSeparator();
//        trayIconMenu->addAction(quitAction);
//        return;
//    }

//    if(_profiles->profiles().size() == 0){
//        trayIconMenu->addAction(showAction);
//        trayIconMenu->addSeparator();
//        trayIconMenu->addAction(quitAction);
//    }
//    else{
        createDynamicMenu();
    //}
}

void DialogMainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon = QIcon(":/img/Firefox.ico"); //iconComboBox->itemIcon(index);
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip("Менеджер профилей Mozilla Firefox");
}

void DialogMainWindow::createDynamicMenu()
{

    trayIconMenu->clear();
    trayIconMenu->addAction(showAction);

    if(_profiles){
        trayIconMenu->addSeparator();

        auto items = _profiles->profiles();

        for (auto item : items){

            QString sz = item->name();
            sz.append(" / ");
            sz.append(item->profile());
            auto action = new QAction(sz, this);
            action->setProperty("uuid", item->uuid());
            trayIconMenu->addAction(action);
            connect(action, &QAction::triggered, this, &DialogMainWindow::onTrayTriggered);
        }
    }
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
}

QString DialogMainWindow::toBankClientFile()
{
    return "";
}

void DialogMainWindow::onWindowShow()
{
    setVisible(true);
}

void DialogMainWindow::onConnectionSuccess()
{
    qDebug() << __FUNCTION__;
    updateConnectionStatus();
//    if(!isFormLoaded)
//        emit endInitConnection();
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::onCloseConnection()
{
    qDebug() << __FUNCTION__;
}

void DialogMainWindow::onConnectedStatusChanged(bool status)
{
    qDebug() << __FUNCTION__;
    updateConnectionStatus();
}

void DialogMainWindow::onMessageReceived(const QString &msg, const QString &uuid, const QString &recipient, const QString &recipientName)
{
    qDebug() << __FUNCTION__;
}

void DialogMainWindow::onDisplayError(const QString &what, const QString &err)
{
    qCritical() << __FUNCTION__ << what << qPrintable(err);
//    if(err == "В соединении отказано"){
//        emit endInitConnection();
//    }
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::onWsExecQuery(const QString &result)
{


    auto doc = QJsonDocument::fromJson(result.toUtf8());
    auto obj = doc.object();
    if(doc.isEmpty())
        return;

    QString id_command = obj.value("id_command").toString();

    qDebug() << __FUNCTION__ << id_command;

    if(id_command == "deleteCertificateFromData"){
        qDebug() << __FUNCTION__ << "Сертификат успешно удален с сервера!";

    }else if(id_command == "deleteContainerFromData"){
        qDebug() << __FUNCTION__ << "Контейнер успешно удален с сервера!";

    }else if(id_command == "get_data"){
        QString table = obj.value("table").toString();
        if(table.isEmpty())
            return;

        //QString json = QByteArray::fromBase64(base64.toUtf8());
        QString run_on_return = obj.value("run_on_return").toString();
        if(!run_on_return.isEmpty()){
            onGetDataFromDatabase(table, run_on_return);
        }else
            qCritical() << __FUNCTION__ << "Не верные параметры команды!";
    }else if(id_command == "get_cert_user_cache"){
        setFromDataUserCache(obj);
    }else if(id_command == "get_available_certs"){
        setAvailableCerts(obj);
    }

}

void DialogMainWindow::sendToRecipient(const QString &recipient, const QString &command, const QString &message, bool to_agent)
{
    qDebug() << __FUNCTION__ << command;

    if(!m_client->isStarted())
        return;

    QString _message = QString("{\"command\": \"%1\", \"message\": \"%2\"}").arg(command, message);
    //qDebug() << message;
    QJsonObject obj = QJsonObject();
    //obj.insert("uuid_agent", m_client->getUuidSession());
    //obj.insert("uuid_agent", m_client->getUuidSession());
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

void DialogMainWindow::addContainer(const QString &from, const QString &to, const QString &byteArrayBase64, const QString& ref)
{
    qDebug() << __FUNCTION__ << from << to;

    KeysContainer* cnt = new KeysContainer(this);
    QString resultText;

    if(from == FromDatabase){
        auto param = QJsonObject();
        param.insert("command", "addContainer");
        param.insert("from", "\\\\.\\DATABASE\\");
        param.insert("to", to);
        getDatabaseData("Containers", ref, param);
        return;
    }
    else{
        if(from.isEmpty()){
            resultText = "Не указан источник для копирования контейнера!";
            qCritical() << __FUNCTION__ << resultText;
            delete cnt;
            if(!currentRecipient.isEmpty())
                sendToRecipient(currentRecipient, "error", resultText);
            return;
        }else
           cnt->fromContainerName(from);
    }

    if(byteArrayBase64.isEmpty())
        cnt->readData(currentUser->sid());
    else{
        QByteArray data = QByteArray::fromBase64(byteArrayBase64.toUtf8());
        cnt->fromJson(data);
    }

    if(!cnt->isValid()){
        resultText = "Ошибка инициализации источника!";
        qCritical() << __FUNCTION__ << resultText;
        delete cnt;
        if(!currentRecipient.isEmpty())
            sendToRecipient(currentRecipient, "error", resultText);
        return;
    }else{
//        //на клиенте автоматом корректируем имя
//        if(isCyrillic(cnt->originalName())){
//            QString dt = cnt->notValidAfter();
//            dt.replace(".", "-");
//            if(!dt.isEmpty())
//                dt.append("-");
//            cnt->setNewOriginalName(cnt->keyName() + "@" + dt + cnt->name().toUtf8().toBase64());
//            qDebug() << __FUNCTION__ << "В имени контейнера обнаружена кириллица. Переведено в base64 автоматичесвки";
//        }
    }

    bool result = false;
    QString _to;
    if(to == ToRegistry){
        result = cnt->sync(KeysContainer::TypeOfStorgare::storgareTypeRegistry, "", currentUser->sid());
        if(result && !currentRecipient.isEmpty()){
            lastResult.clear();
            lastResult.append("InstallContainerToRemoteUser");
            lastResult.append("sucsess");
            m_async_await.append(std::bind(&DialogMainWindow::currentUserGetConteiners, this));
            m_async_await.append(std::bind(&DialogMainWindow::sendResultToClient, this));
            asynAwait();
            return;
        }
//        if(!currentRecipient.isEmpty())
//            _to = "InstallContainerToRemoteUser";

    }else if(to == ToDatabase){
        if(_sett->launch_mode() == mixed)
            result = cnt->sync(db);
        else
            result = cnt->sync(m_client);
    }else if(to == ToRemoteVolume || to == ToRemoteRegistry){
        QString remoteCommand = to == ToRemoteVolume ? ToVolume : ToRegistry;
        QString base64 = cnt->toBase64();
        auto obj = QJsonObject();
        obj.insert("data", base64);
        obj.insert("from", QString("\\\\.\\%1\\%2").arg(REMOTEBASE_, cnt->originalName()));
        obj.insert("to", remoteCommand);
        QString resp = QJsonDocument(obj).toJson().toBase64();
        if(!currentRecipient.isEmpty())
            sendToRecipient(currentRecipient, remoteCommand, resp, true);
        delete cnt;
        currentRecipient = "";
        qDebug() << __FUNCTION__ << "Данные контейнера отправлены по удаленному запросу.";
        return;
    }

    if(result){
        if(!currentRecipient.isEmpty()){
            if(_to.isEmpty())
                _to = to;
            sendToRecipient(currentRecipient, _to, "Операция успешно выполнена!", true);
        }
    }else
        if(!currentRecipient.isEmpty())
            sendToRecipient(currentRecipient, "error", "Ошибка выполнения операции!", true);

    delete cnt;
    currentRecipient = "";
    return;

}

void DialogMainWindow::addCertificate(const QString &from, const QString &to, const QString &byteArrayBase64, const QString& ref, const QString& cntName, const QString& cntByteArrayBase64)
{
    qDebug() << __FUNCTION__ <<from << to;

    Certificate* cert = new Certificate(this);
    QString resultText;

    QStringList lst = from.split("/");
    QString storgare = lst.size() > 0 ? lst[0] :  STORGARE_LOCALHOST;
    QString _from = lst.size() > 1 ? lst[1] : from;
    bool result = false;

    QString resultData;
    QString command = "addCertificate";

    m_async_await.clear();

    if(storgare == STORGARE_DATABASE){
        QString _ref = from;
        if(!ref.isEmpty())
            _ref = ref;

        auto param = QJsonObject();
        param.insert("command", "addCertificate");
        param.insert("from", STORGARE_LOCALHOST);
        param.insert("to", STORGARE_LOCALHOST);
        getDatabaseData("CertificatesView", _ref, param);
        return;
    }else if(storgare == STORGARE_LOCALHOST){
        if(byteArrayBase64.isEmpty()){
            result = cert->fromSha1(_from);
            if(result){
                QJsonObject obj = cert->getObject();
                resultData  = QJsonDocument(obj).toJson().toBase64();
            }
        }else{
            if(to == STORGARE_LOCALHOST){
                command = "installCertificate";
                ByteArray data = Base64Converter::base64_to_byte(QString(byteArrayBase64.toUtf8()).toStdString());
                cert->setData(data);

                QString container;
                if(!cntName.isEmpty()){
                    //Устанавливаем при необходимости контейнер в реестр и указываем для инсталяции
                    QString strCnts = currentUser->getRigstryData().join("|");
                    if(strCnts.indexOf(cntName) != -1)
                        container = QString("\\\\.\\%1\\%2").arg(REGISTRY_, cntName);

                    if(container.isEmpty() && !cntByteArrayBase64.isEmpty()){
                        //Устанавливаем контейнер
                        auto cnt = KeysContainer(this);
                        QByteArray data = QByteArray::fromBase64(cntByteArrayBase64.toUtf8());
                        cnt.fromJson(data);
                        bool rs = cnt.syncRegystry(currentUser->sid());
                        if(!rs)
                            qCritical() << __FUNCTION__ << "Ошибка установки контейнера при установке сертификата! Сертификат будет установлен без привязки к контейнеру.";
                        else
                            container = QString("\\\\.\\%1\\%2").arg(REGISTRY_, cntName);
                    }
                    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetConteiners, this));
                    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetCertificates, this));
                }else
                    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetCertificates, this));

                result = cert->install(container);
                resultData = "Сертификат успешно установлен!";
            }
        }
    }



    if(result){  
        if(!currentRecipient.isEmpty()){
            lastResult.clear();
            lastResult.append(command);
            lastResult.append(resultData);
            m_async_await.append(std::bind(&DialogMainWindow::sendResultToClient, this));

            asynAwait();
//            sendToRecipient(currentRecipient, command, resultData, true);
//            currentRecipient = "";
        }
    }else{
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, command, "error", true);
            currentRecipient = "";
        }
    }

}

void DialogMainWindow::onGetCryptData(const QString &recipient)
{
    qDebug() << __FUNCTION__ << recipient;

    auto doc = QJsonDocument();
    auto obj = QJsonObject();
    auto objCerts = QJsonObject();
    objCerts.insert("cnt", currentUser->containers().join("\n"));
    objCerts.insert("certs", currentUser->certModel());
    obj.insert("command", "mpl_cert_data");
    obj.insert("message", QString(QJsonDocument(objCerts).toJson().toBase64()));
    QString object = QJsonDocument(obj).toJson(QJsonDocument::Indented);

    auto objMain = QJsonObject();
    objMain.insert("command", "mpl_cert_data");
    objMain.insert("message", object);

    objMain.insert("uuid_agent", recipient);
    objMain.insert("uuid_client", m_client->getUuidSession());

    doc.setObject(objMain);
    QString param = doc.toJson();// QJsonDocument(objMain).toJson(QJsonDocument::Indented);
    m_client->sendCommand("command_to_qt_agent", "", param);
}

bool DialogMainWindow::deleteLocalObject(const QString& objectName, const QString& objectType)
{

    qDebug() << __FUNCTION__ << objectName << objectType;

    bool result = false;

    if(objectType == OBJECT_TYPE_CONTAINER){
        auto cnt = KeysContainer();
        cnt.fromContainerName(objectName);
        if(cnt.typeOfStorgare() == KeysContainer::TypeOfStorgare::storgareTypeRegistry){
            result = cnt.deleteContainer(currentUser->sid());
        }else if(cnt.typeOfStorgare() == KeysContainer::TypeOfStorgare::storgareTypeLocalVolume){
            result = cnt.deleteContainer();
        }
    }else if(objectType == OBJECT_TYPE_CERTIFICATE){
        QString sha1 = objectName;
        QString _qbyte = QString("certmgr -delete -thumbprint \"%1\"\n").arg(sha1);
        if(!sha1.isEmpty())
            terminal->send(_qbyte, certmgrDeletelCert);
        else
            return false;
       result = true;
    }

    return result;

}

void DialogMainWindow::onGetDataFromDatabase(const QString &table, const QString param)
{
    qDebug() << __FUNCTION__;

    auto _table = QJsonDocument::fromJson(table.toUtf8()).object();
    auto _param = QJsonDocument::fromJson(param.toUtf8()).object();
    auto rows = _table.value("rows").toArray();
    if(rows.isEmpty()){
        qCritical() << __FUNCTION__ << "Объект на сервере не найден!";
        return;
    }

    QString command = _param.value("command").toString();
    QStringList dataCommand = {"addContainer", "addCertificate"};
    auto row = rows[0].toObject();

    if(dataCommand.indexOf(command) != -1){

        QString dataBase64 = row.value("data").toString();

        QString name = _param.value("FirstField").toString();

        if(command == "addContainer"){
            QString from = _param.value("from").toString();
            if(!name.isEmpty())
                from = from + name;
            QString to = _param.value("to").toString();
            addContainer(from, to, dataBase64);
        }else if(command == "addCertificate"){
            QString from = _param.value("from").toString();
            if(!name.isEmpty())
                from = from + name;
            QString to = _param.value("to").toString();
            QString cntName = row.value("CntFirstField").toString();
            QString cntData = row.value("CntData").toString();
            addCertificate(from, to, dataBase64, "", cntName, cntData);
        }
    }else if(command == "getCache"){

        QString cache = row.value("cache").toString();
        QString name = _param.value("FirstField").toString();

        QString to = _param.value("to").toString();

        if(to == "userCache"){
            if(!_profiles){
                if(m_async_await.size() > 0){
                    auto f = m_async_await.dequeue();
                    f();
                }
                return;
            }

            auto doc = QJsonDocument::fromJson(cache.toUtf8());
            _profiles->setCache(doc.object());

            if(m_async_await.size() > 0){
                auto f = m_async_await.dequeue();
                f();
            }
        }
    }
}

void DialogMainWindow::getDatabaseData(const QString& table, const QString& ref, const QJsonObject& param)
{
        qDebug() << __FUNCTION__ << table;

        auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, table);

        bindQuery.addField("Ref", "Ref");
        bindQuery.addField("FirstField", "FirstField");
        bindQuery.addField("data", "data");
        if(table == "CertificatesView"){
           bindQuery.addField("CntData", "CntData");
           bindQuery.addField("CntFirstField", "CntFirstField");
        }
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

void DialogMainWindow::getDatabaseCache(const QString& table, const QString& ref, const QJsonObject& param)
{
        qDebug() << __FUNCTION__ << table;

        auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, table);

        bindQuery.addField("Ref", "Ref");
        bindQuery.addField("FirstField", "FirstField");
        bindQuery.addField("cache", "data");

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

void DialogMainWindow::updateDataUserCache()
{
    qDebug() << __FUNCTION__;

    if(!_profiles)
        return;

    if(currentUser->ref().isEmpty())
        return;

    auto obj = _profiles->cache();

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlUpdate, "CertUsers");
    bindQuery.addField("cache", QJsonDocument(_profiles->cache()).toJson());

    bindQuery.addWhere("Ref", currentUser->ref());
    QString query = bindQuery.to_json();

    QJsonObject cmd = QJsonObject();
    cmd.insert("command", "update_cert_user_cache");

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
        QString _error;
        db->exec_qt(query, _error);
    }else{
        if(m_client->isStarted()){
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("id_command", "update_cert_user_cache");
            obj.insert("run_on_return", QString(QJsonDocument(cmd).toJson()));
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString paramData = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", paramData);
        }
    }

}

void DialogMainWindow::setFromDataUserCache(const QJsonObject &resp)
{
    qDebug() << __FUNCTION__;

    QString table = resp.value("table").toString();
    if(table.isEmpty())
        return;

    auto _table = QJsonDocument::fromJson(table.toUtf8()).object();
    auto rows = _table.value("rows").toArray();
    if(rows.isEmpty()){
        qCritical() << __FUNCTION__ << "Объект на сервере не найден!";
        return;
    }

    auto row = rows[0].toObject();

    currentUser->setRef(row.value("Ref").toString());
    currentUser->setUuid(QUuid::fromString(row.value("uuid").toString()));

    QString jCache = row.value("cache").toString();
    if(!jCache.isNull()){

        if(_profiles){
            auto doc = QJsonDocument::fromJson(jCache.toUtf8());
            _profiles->setCache(doc.object());
        }

    }

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }

}

void DialogMainWindow::getUserData()
{

    qDebug() << __FUNCTION__;

    if(!_profiles)
        return;

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "CertUsers");

    bindQuery.addField("Ref", "Ref");
    bindQuery.addField("cache", "cache");
    bindQuery.addField("uuid", "uuid");

    bindQuery.addWhere("FirstField", currentUser->name());
    bindQuery.addWhere("host", currentUser->domain());

    QString query = bindQuery.to_json();

    QJsonObject cmd = QJsonObject();
    cmd.insert("command", "get_cert_user_cache");

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);
        auto doc = QJsonDocument::fromJson(resultQuery.toUtf8());
        auto obj = doc.object();
        setFromDataUserCache(obj);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else{
        if(m_client->isStarted()){
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("id_command", "get_cert_user_cache");
            obj.insert("table", true);
            obj.insert("run_on_return", QString(QJsonDocument(cmd).toJson()));
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString paramData = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", paramData);
        }
    }
}

void DialogMainWindow::getAvailableCerts()
{
    //доступные севртификаты
    qDebug() << __FUNCTION__;

    if(!_profiles)
        return;

    auto bindQuery = QBSqlQuery(QBSqlCommand::QSqlGet, "AvailableCertsView");

    bindQuery.addField("FirstField", "FirstField");
    bindQuery.addField("CertRef", "CertRef");
    bindQuery.addField("UserRef", "UserRef");
    bindQuery.addField("sha1", "sha1");

    bindQuery.addWhere("UserRef", currentUser->ref());

    QString query = bindQuery.to_json();

    QJsonObject cmd = QJsonObject();
    cmd.insert("command", "get_available_certs");

    if(_sett->launch_mode() == mixed){
        if(!db->isOpen())
            return;
        QString resultQuery;
        QString _error;
        db->exec_qt(query, resultQuery, _error);
        auto doc = QJsonDocument::fromJson(resultQuery.toUtf8());
        auto obj = doc.object();
        setAvailableCerts(obj);
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }else{
        if(m_client->isStarted()){
            auto obj = QJsonObject();
            obj.insert("query", query);
            obj.insert("id_command", "get_available_certs");
            obj.insert("table", true);
            obj.insert("run_on_return", QString(QJsonDocument(cmd).toJson()));
            auto doc = QJsonDocument();
            doc.setObject(obj);
            QString paramData = doc.toJson();
            m_client->sendCommand("exec_query_qt", "", paramData);
        }
    }
}

void DialogMainWindow::setAvailableCerts(const QJsonObject& resp)
{
    qDebug() << __FUNCTION__;

    QString table = resp.value("table").toString();
    if(table.isEmpty()){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    auto _table = QJsonDocument::fromJson(table.toUtf8()).object();
    auto rows = _table.value("rows").toArray();
    if(rows.isEmpty()){
        //qCritical() << __FUNCTION__ << "Объект на сервере не найден!";
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    currentUser->set_available_certificates(_table);

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::onWsGetAvailableContainers(const QString &recipient)
{
    qDebug() << __FUNCTION__;
    if(currentUser->containers().size() > 0){
        sendToRecipient(recipient, "available_containers", currentUser->containers().join("\n").toUtf8().toBase64(), true);
    }else{
        currentRecipient = recipient;
        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
    }
}

void DialogMainWindow::onWsGetInstalledCertificates(const QString &recipient)
{
    qDebug() << __FUNCTION__;
//    if(currentUser->certificates().size() > 0){
//        auto objCerts = QJsonObject();
//        objCerts.insert("certs", currentUser->certModel());
//        QString object = QJsonDocument(objCerts).toJson(QJsonDocument::Indented);
//        sendToRecipient(recipient, "installed_certificates", object.toUtf8().toBase64(), true);
//    }else{
        currentRecipient = recipient;
        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
//    }
}

void DialogMainWindow::onWsCommandToClient(const QString &recipient, const QString &command, const QString &message)
{

    qDebug() << __FUNCTION__ << recipient << command;

    if(command == "addContainer"){

        currentRecipient = recipient;
        auto doc = QJsonDocument::fromJson(QByteArray::fromBase64(message.toUtf8()));
        auto obj = doc.object();
        QString from = obj.value("from").toString();
        QString to = obj.value("to").toString();
        QString ref = obj.value("ref").toString();
        if(ref.isEmpty())
            addContainer(from, to);
        else
           addContainer(from, to, "", ref);
    }else if(command == "deleteContainer"){

        currentRecipient = recipient;
        auto doc = QJsonDocument::fromJson(QByteArray::fromBase64(message.toUtf8()));
        auto obj = doc.object();
        QString from = obj.value("from").toString();
        QString to = obj.value("to").toString();

        bool result = deleteLocalObject(from, OBJECT_TYPE_CONTAINER);

        if(!currentRecipient.isEmpty()){
            if(!result){
                sendToRecipient(currentRecipient, command, "Ошибка удаления контейнера!", true);
                currentRecipient = "";
            }else{
                sendToRecipient(currentRecipient, command, "Контейнер успешно удален!", true);
                currentRecipient = "";
                terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
            }
        }


    }else if(command == "deleteCertificate"){

        currentRecipient = recipient;
        auto doc = QJsonDocument::fromJson(QByteArray::fromBase64(message.toUtf8()));
        auto obj = doc.object();
        QString from = obj.value("from").toString();
        QString to = obj.value("to").toString();

        deleteLocalObject(from, OBJECT_TYPE_CERTIFICATE);

    }else if(command == "addCertificate"){

        currentRecipient = recipient;
        auto doc = QJsonDocument::fromJson(QByteArray::fromBase64(message.toUtf8()));
        auto obj = doc.object();
        QString from = obj.value("from").toString();
        QString to = obj.value("to").toString();
        QString ref = obj.value("ref").toString();
        if(ref.isEmpty())
           addCertificate(from, to);
        else
           addCertificate(from, to, "", ref);
    }

}



void DialogMainWindow::onParseCommand(const QVariant &result, int command)
{

    qDebug() << __FUNCTION__ << command;

    if(command == CmdCommand::wmicGetSID){
        currentUser->setSid(result.toString());
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
     }else if(command == CmdCommand::csptestGetConteiners){
//        QString res = result.toString();
//        res.replace("\r", "");
//        QStringList keys = res.split("\n");
//        currentUser->setContainers(keys);
//        if(!currentRecipient.isEmpty()){
//            sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n").toUtf8().toBase64(), true);
//            currentRecipient = "";
//        }

//        //получаем список сертификатов
//        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
        QString res = result.toString();
        res.replace("\r", "");
        csptestCurrentUserGetContainers(result.toString());

        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n").toUtf8().toBase64(), true);
            if(lastResult.size() == 0)
                currentRecipient = "";
        }

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }else
            updateConnectionStatus();

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

        if(!currentRecipient.isEmpty()){
            auto objCerts = QJsonObject();
            objCerts.insert("certs", currentUser->certModel());
            QString object = QJsonDocument(objCerts).toJson(QJsonDocument::Indented);
            sendToRecipient(currentRecipient, "installed_certificates", object.toUtf8().toBase64(), true);
            if(lastResult.size() == 0)
                currentRecipient = "";
        }

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }else
            updateConnectionStatus();

//        if(!isFormLoaded) //оповещяем о окончании загрузки данных
//            emit whenDataIsLoaded();


    }else if(command == CmdCommand::certmgrDeletelCert){
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, "deleteCertificate", "sucsess", true);
            currentRecipient = "";
        }
    }else if(command == CmdCommand::certmgrInstallCert){
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, "installCertificate", "sucsess", true);
            currentRecipient = "";
        }
    }
}

void DialogMainWindow::onCommandError(const QString &result, int command)
{
//    if(command == CmdCommand::wmicGetSID){
////        QString sid = result.toString();
////        qDebug() << __FUNCTION__ << "set sid:" <<  sid;
////        currentUser->setSid(sid);
////        //получаем список контейнеров
////        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
//        //QMessageBox::critical(this, "tmp error", "wmicGetSID");

//     }else if(command == CmdCommand::csptestGetConteiners){
////        QString res = result.toString();
////        res.replace("\r", "");
////        QStringList keys = res.split("\n");
////        currentUser->setContainers(keys);
////        if(!currentRecipient.isEmpty()){
////            sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n").toUtf8().toBase64(), true);
////            currentRecipient = "";
////        }
////        //получаем список сертификатов
////        terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
//        //QMessageBox::critical(this, "tmp error", "csptestGetConteiners");
//    }else if(command == CmdCommand::csptestGetCertificates){
//        //QMessageBox::critical(this, "tmp error", "csptestGetCertificates");
//    }

    qCritical() << __FUNCTION__ << "error: " <<  result;
    if(result.indexOf("Empty certificate list") != 1){
//        if(!isFormLoaded) //оповещяем о окончании загрузки данных
//            emit whenDataIsLoaded();

        currentUser->certificates().clear();

        if(!currentRecipient.isEmpty()){
            auto objCerts = QJsonObject();
            objCerts.insert("certs", currentUser->certModel());
            QString object = QJsonDocument(objCerts).toJson(QJsonDocument::Indented);
            sendToRecipient(currentRecipient, "installed_certificates", object.toUtf8().toBase64(), true);
            currentRecipient = "";
        }

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    if(command == CmdCommand::wmicGetSID){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
    }
}

void DialogMainWindow::onOutputCommandLine(const QString &data, int command)
{
    //qDebug() << __FUNCTION__ << "command: " << command;

    if(data.indexOf("Error:") > 0)
        return;

    terminal->parseCommand(data, command);
}

void DialogMainWindow::onCommandLineStart()
{
    qDebug() << __FUNCTION__;

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

        //createWsObject();

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
}

void DialogMainWindow::on_btnAdd_clicked()
{
    UserProfile * prof = new UserProfile(this);
    prof->setName("Новая настройка");
    auto dlg = DialogSelectedRow(prof, currentUser, this);
    dlg.setModal(true);
    dlg.exec();

    if(dlg.result() == QDialog::Accepted){
        setRow(prof);
    }else
        delete prof;

}

void DialogMainWindow::onSelectProfile()
{
    qDebug() << __FUNCTION__;

    auto dlg = new DialogSelectProfile(_profiles->profilesNames(), this);
    dlg->setModal(true);
    dlg->exec();
    if(dlg->result() == QDialog::Accepted){

        auto *button = dynamic_cast<QToolButton*>( sender() );
        QVariant vRow = button->property("row");
        if(vRow.isValid()){
//            int row = vRow.toInt();
//            int curRow = ui->tableWidget->selectionModel()->currentIndex().row();
//            if(row != curRow)
//                ui->tableWidget->selectRow(row);

//             QWidget* pWidget = ui->tableWidget->cellWidget(row, 0);
//             if(pWidget){
//                 auto *line = pWidget->findChild<QLineEdit*>("lineEdit" + QString::number(row));
//                 if(line)
//                     line->setText(dlg->getResult());
//             }
//             ui->tableWidget->resizeColumnsToContents();
        }
    }
}

void DialogMainWindow::onSelectDefaultAddress()
{
//    qDebug() << __FUNCTION__;

//    auto *btn = dynamic_cast<QToolButton*>( sender() );
//    if(btn){
//        int row = btn->property("row").toInt();
//        int col = btn->property("col").toInt();
//        ui->tableWidget->setCurrentCell(row, col);
//        auto *lbl = getLineEdit(row, col);
//        if(lbl){
//            bool bOk;
//            QString str = QInputDialog::getText( this,
//                                                 "Адрес страницы",
//                                                 "Адрес:",
//                                                 QLineEdit::Normal,
//                                                 lbl->text(),
//                                                 &bOk
//                                                );
//            if (bOk) {
//                lbl->setText(str);
//            }
//        }
//    }
}

void DialogMainWindow::onSelectDefaultCertificate()
{
//    qDebug() << __FUNCTION__;

//    auto *btn = dynamic_cast<QToolButton*>( sender() );
//    if(btn){
//        int row = btn->property("row").toInt();
//        int col = btn->property("col").toInt();
//        ui->tableWidget->setCurrentCell(row, col);
//    }
}

void DialogMainWindow::updateFromData() {

//    QTableWidget * table = ui->tableWidget;
//    table->setRowCount(0);

//    auto items = _profiles->profiles();
//    for (auto itr : items) {
//        setRow(itr);
//    }
}

//void DialogMainWindow::onLineEditCursorPositionChanged(int oldPos, int newPos)
//{
//    auto *edit = dynamic_cast<QLineEdit*>( sender() );
//    if(edit){
//        int row = edit->property("row").toInt();
//        int col = edit->property("col").toInt();
//        ui->tableWidget->setCurrentCell(row, col);
//    }

//    qDebug() << oldPos << newPos;
//}

void DialogMainWindow::currentUserSid()
{
    qDebug() << __FUNCTION__;

    infoBar->setText("Получение данных пользователя ..");

#ifdef _WINDOWS
    terminal->send("WHOAMI /USER\n", CmdCommand::wmicGetSID);
#else
    onParseCommand("", CmdCommand::wmicGetSID);
#endif
}

void DialogMainWindow::currentUserGetConteiners()
{
    infoBar->setText("Получение данных о доступных контейнерах ...");
    qDebug() << __FUNCTION__;
    terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
}

void DialogMainWindow::currentUserGetCertificates()
{
    infoBar->setText("Получение данных о доступных сертификатах ...");
    qDebug() << __FUNCTION__;
    terminal->send("certmgr -list -store uMy\n", CmdCommand::csptestGetCertificates);
}

void DialogMainWindow::csptestCurrentUserGetContainers(const QString &result)
{
    qDebug() << __FUNCTION__;

    if(result.isEmpty())
        return;

    if(!currentUser)
        return;

    QStringList keys = result.split("\n");
    currentUser->setContainers(keys);

//    if(!currentRecipient.isEmpty()){
//        sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n").toUtf8().toBase64(), true);
//        if(lastResult.size() == 0)
//            currentRecipient = "";
//    }
}

void DialogMainWindow::createTerminal()
{

    qDebug() << __FUNCTION__;
    terminal = new CommandLine(this, false, _sett->charset());
    terminal->setMethod(_sett->method());

    connect(terminal, &CommandLine::output, this, &DialogMainWindow::onOutputCommandLine);
    connect(terminal, &CommandLine::endParse, this, &DialogMainWindow::onParseCommand);
    connect(terminal, &CommandLine::error, this, &DialogMainWindow::onCommandError);
    connect(terminal, &CommandLine::cmdStarted, this, &DialogMainWindow::onCommandLineStart);

    terminal->start();

//    if(isUseCsptest)
//        terminal->send(QString("cd \"%1\"\n").arg(_cprocsp_dir), CmdCommand::unknown);

//#ifdef _WINDOWS
//    terminal->send("WHOAMI /USER\n", CmdCommand::wmicGetSID);
//#endif

}


void DialogMainWindow::on_btnEdit_clicked()
{
    qDebug() << __FUNCTION__;

    auto index = ui->tableView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }

    int iUuid = modelMplProfiles->getColumnIndex("uuid");
    auto uuid = modelMplProfiles->index(index.row(), iUuid).data(Qt::UserRole + iUuid).toString();
    const auto itr = _profiles->profiles().find(QUuid::fromString(uuid));
    if(itr != _profiles->profiles().end()){
        auto dlg = DialogSelectedRow(itr.value(), currentUser, this);
        dlg.setModal(true);
        dlg.exec();

        if(dlg.result() == QDialog::Accepted){
            updateRow(itr.value(), index.row());
        }
    }

}


void DialogMainWindow::on_btnDelete_clicked()
{
    qDebug() << __FUNCTION__;

    auto index = ui->tableView->currentIndex();
    if(!index.isValid()){
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }

    if(QMessageBox::information(this,
                                 "Удаление",
                                 "Удалить текущую настройку?",
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;


    int iUuid = modelMplProfiles->getColumnIndex("uuid");
    auto uuid = modelMplProfiles->index(index.row(), iUuid).data(Qt::UserRole + iUuid).toString();
    const auto itr = _profiles->profiles().find(QUuid::fromString(uuid));
    if(itr != _profiles->profiles().end())
        _profiles->profiles().erase(itr);

    modelMplProfiles->removeRow(index.row());

    updateDataUserCache();
}

void DialogMainWindow::onTrayTriggered()
{
    qDebug() << __FUNCTION__;

    bool isExists = true;
    if(_profiles->mozillaExeFile().isEmpty()){
        isExists = false;
    }
    if(isExists)
        isExists = QFile::exists(_profiles->mozillaExeFile());

    if(!isExists){
        QString exeFile = "firefox";
#ifdef Q_OS_WINDOWS
      exeFile.append(".exe");
#endif
        QString file = QFileDialog::getOpenFileName(this, tr("Путь к firefox"),
                                                     QDir::homePath(),
                                                     exeFile);
        if(file != ""){
            _profiles->setMozillaExeFile(QDir::toNativeSeparators(file));
            isExists = true;

        }
    }

    auto *action = dynamic_cast<QAction*>( sender() );

    QUuid uuid = action->property("uuid").toUuid();
    auto profs = _profiles->profiles();
    auto itr = profs.find(uuid);

    QString profName = "";
    QString defPage = "";

    if(itr != profs.end()){
        profName = itr.value()->profile();
        defPage = itr.value()->defaultAddress();
    }

    //открываем адрес указанный на флешке банка
    if(defPage == _bakClientUsbKey){
        foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
            if (storage.isValid()) {
                qDebug() << storage.rootPath() + QDir::separator() + _bankClientFile;
                QFile file = QFile(storage.rootPath() + QDir::separator() + _bankClientFile);
                if(file.exists()){
                    QSettings lnk(file.fileName(), QSettings::IniFormat);
                    QStringList keys = lnk.allKeys();
                    foreach(const QString& key, keys){
                        if(key.compare("InternetShortcut")){
                            if(key.endsWith("/URL")){
                                defPage = lnk.value(key).toString();
                                break;
                            }

                        }
                    }
                }
            }
        }
    }

    QStringList args;

    if(!profName.isEmpty()){
        args.append("-P");
        args.append(profName);
        if(!defPage.isEmpty()){
             QFile file = QFile(defPage);
             if(file.exists())
                 defPage = "file:///" + defPage;
            args.append("-URL");
            args.append(defPage);
        }
    }
    if(isExists){
//        QProcess kill;
//        //kill.setProgram("cmd");

//        //kill.waitForFinished();
//        kill.start("cmd" );
//        kill.write("killall firefox \n");

////        QString program;
////        program.append("powershell");
////        QStringList arguments;
////        arguments<<"killall firefox";
////        QString commandLine = "WHOAMI /USER";
////        commandLine.append("\n");
////
////        myProcess.start(program);
////        myProcess.write(commandLine.toUtf8());
////
////        //myProcess.start(program, arguments);

//        if(!kill.waitForStarted(500)) //default wait time 30 sec
//        {
//            qDebug() << "Cannot execute:" << "cmd";
//        }
//        kill.waitForFinished(500);
//        kill.setProcessChannelMode(QProcess::MergedChannels);

        qDebug() << mozillaApp->state();
        mozillaApp->terminate();
        mozillaApp->kill();
        mozillaApp->waitForFinished();
        mozillaApp->start(_profiles->mozillaExeFile(), args);

    }


}

void DialogMainWindow::onAppExit()
{
    if(terminal)
        terminal->stop();
    if(m_client)
        if(m_client->isStarted())
            m_client->close(true);
    if(db)
        if(db->isOpen())
            db->close();
//    delete m_client;
//    delete db;
    QApplication::exit();
}


void DialogMainWindow::connectToDatabase()
{

    qDebug() << __FUNCTION__;

    if(_sett->launch_mode() != mixed)
    {
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    infoBar->setText("Подключение к SQL серверу ...");

    QString host = _sett->server();
    QString database = "arcirk";
    QString userName = _sett->user();
    QString pwd = _sett->pwd();
    QString password = bWebSocket::crypt(pwd, "my_key");

    db->setSqlUser(userName);
    db->setSqlPwd(password);
    db->setHost(host);
    db->setDatabaseName(database);
    db->connect();

    connectToWsServer();

    _sett->save();

    if (!db->isOpen()){
        QMessageBox::critical(this, "Ошибка", QString("Ошибка подключения к базе данных: %2").arg(db->lastError()));
    }else{
        //getDataContainersList();
    }

    updateConnectionStatus();
}

void DialogMainWindow::updateConnectionStatus()
{
    qDebug() << __FUNCTION__;
    QString status;

    if(db->isOpen()){
        status = "SQL Server: " + _sett->server() + "  ";

    }

    if(m_client->isStarted()){
        status.append("WS: " + m_client->getHost() + ":" + QString::number(m_client->getPort()));
    }

    if(status.isEmpty()){
        status.append("Не подключен.");
    }

    status.append(_sett->launch_mode() == mixed ? " mode: mixed" : " mode: ws_server");

    infoBar->setText(status);

}

void DialogMainWindow::getSettingsFromHttp()
{
    qDebug() << __FUNCTION__;

    if(!_sett->useSettingsFromHttp()){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }

    infoBar->setText("Получение данных с http сервиса 1C ...");

    if(_sett->httpHost().isEmpty())
        return;

    QString httpPwd = _sett->httpPwd();
    if(!httpPwd.isEmpty())
        httpPwd = QString::fromStdString(ClientSettings::crypt(httpPwd.toStdString(), "my_key"));
    else
        httpPwd = "";

    QEventLoop loop;
    HttpService * httpService = new HttpService(this, _sett->httpHost(),
            _sett->httpUsr(),
            httpPwd);

    QString result;

    auto finished = [&result, &loop](QNetworkReply* reply) -> void
    {
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
       if(status_code.isValid()){
           int status = status_code.toInt();
           if(status != 200)
                qDebug() << __FUNCTION__ << "Error: " << status << " " + reply->errorString() ;
           else
           {
               QByteArray data = reply->readAll();
               result = data;
           }
       }
       loop.quit();
    };

    loop.connect(httpService, &HttpService::finished, finished);

    httpService->request("getConSett_" + currentUser->name()); //формат процедуры GET на http сервисе: getConSett_UserName

    loop.exec();

    //qDebug() << result;

    if(result.isEmpty())
        return;

    auto doc = QJsonDocument::fromJson(result.toUtf8());
    if(doc.isEmpty())
        return;

    auto obj = doc.object();
    auto arr = obj.value("Rows").toArray();
    if(arr.isEmpty())
        return;

    auto row = arr[0].toObject();

    m_client->options()[bConfFieldsWrapper::ServerHost] = row.value("wsServer").toString();
    m_client->options()[bConfFieldsWrapper::ServerPort] = row.value("wsPort").toString().toInt();

    if(!_sett->customWsUser()){
        m_client->options()[bConfFieldsWrapper::User] = row.value("userName").toString();
        m_client->options()[bConfFieldsWrapper::Hash] = bWebSocket::generateHash(row.value("userName").toString(), row.value("userPwd").toString());
    }

    _sett->setServer( row.value("sqlServer").toString());
    _sett->setUser(row.value("sqlUser").toString());

    QString pwd = row.value("sqlPwd").toString();
    if(!pwd.isEmpty())
        _sett->setPwd(bWebSocket::crypt(pwd, "my_key"));

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }

}

//void DialogMainWindow::getCurrentUser()
//{
////    currentUser = new CertUser(this);
////    QString curentHost = QSysInfo::machineHostName();
////    currentUser->setDomain(curentHost);
////#ifdef _WINDOWS
////        std::string envUSER = "username";
////        QByteArray data(std::getenv(envUSER.c_str()));
////        QString uname = QString::fromLocal8Bit(data);
////        currentUser->setName(uname);
////#else
//////        std::string envUSER = "USER";
//////        QString cryptoProDir = "/opt/cprocsp/bin/amd64/";
//////        terminal->send("echo $USER\n", 1); //CommandLine::cmdCommand::echoUserName);// ; exit\n
////#endif

////    if(uname.isEmpty()){
////        qCritical() << __FUNCTION__ << "Ошибка получения имени пользователя!";
////    }
//}

void DialogMainWindow::connectToWsServer()
{
    qDebug() << __FUNCTION__;

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

    infoBar->setText("Подключение к серверу WS ...");

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
    }else{
        QString _host = m_client->options()[bConfFieldsWrapper::ServerHost].toString();
        int _port = m_client->options()[bConfFieldsWrapper::ServerPort].toInt();
        m_client->setHost(_host);
        m_client->setPort(_port);
        m_client->open(m_client->options()[bConfFieldsWrapper::User].toString(), "");
    }

    updateConnectionStatus();
}

void DialogMainWindow::createWsObject()
{
    qDebug() << __FUNCTION__;
    m_client = new bWebSocket(this, "conf_qt_mpl.json", currentUser->name());
    m_client->setAppName("qt_mpl_client");
    setWsConnectedSignals();

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::setWsConnectedSignals()
{
    qDebug() << __FUNCTION__;
    connect(m_client, &bWebSocket::connectionSuccess, this, &DialogMainWindow::onConnectionSuccess);
    connect(m_client, &bWebSocket::closeConnection, this, &DialogMainWindow::onCloseConnection);
    connect(m_client, &bWebSocket::connectedStatusChanged, this, &DialogMainWindow::onConnectedStatusChanged);
    connect(m_client, &bWebSocket::displayError, this, &DialogMainWindow::onDisplayError);
    connect(m_client, &bWebSocket::messageReceived, this, &DialogMainWindow::onMessageReceived);
    connect(m_client, &bWebSocket::execQuery, this, &DialogMainWindow::onWsExecQuery);
    connect(m_client, &bWebSocket::wsGetAvailableContainers, this, &DialogMainWindow::onWsGetAvailableContainers);
    connect(m_client, &bWebSocket::wsGetCryptData, this, &DialogMainWindow::onGetCryptData);
    connect(m_client, &bWebSocket::wsCommandToClient, this, &DialogMainWindow::onWsCommandToClient);
    connect(m_client, &bWebSocket::wsGetInstalledCertificates, this, &DialogMainWindow::onWsGetInstalledCertificates);
}

void DialogMainWindow::initProfiles()
{
    qDebug() << __FUNCTION__;

    modelMplProfiles = new QJsonTableModel(this);
    modelMplProfiles->setColumnAliases(m_colAliases);
    //modelMplProfiles->setRowsIcon(QIcon(":/img/cont.png"));
    ui->tableView->setModel(modelMplProfiles);
     ui->tableView->resizeColumnsToContents();

    _profiles = new ProfileManager(appHome.path(), this);
    if(_profiles->mozillaExeFile().isEmpty()){
        _profiles->setMozillaExeFile("C:\\Program Files\\Mozilla Firefox\\firefox.exe");
    }
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::getInDataCache()
{

    qDebug() << __FUNCTION__;
    if(currentUser->ref().isEmpty()){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }
    auto param = QJsonObject();
    param.insert("command", "getCache");
    param.insert("from", "\\\\.\\DATABASE\\");
    param.insert("to", "userCache");
    getDatabaseCache("CertUsers", currentUser->ref(), param);


}

void DialogMainWindow::setProfilesModel()
{
    if(!_profiles || !modelMplProfiles){
        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }
        return;
    }


    modelMplProfiles->setJsonText(_profiles->model());
    modelMplProfiles->reset();

    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }

}

void DialogMainWindow::sendResultToClient()
{
    qDebug() << __FUNCTION__;
    if(lastResult.size() == 2){
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, lastResult[0], lastResult[1], true);
            currentRecipient = "";
        }
        lastResult.clear();
    }
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }else
        updateConnectionStatus();
}

void DialogMainWindow::asynAwait()
{
    qDebug() << __FUNCTION__;
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
    if(m_async_await.size() == 0){
        currentRecipient = "";
    }
}

void DialogMainWindow::on_btnSettings_clicked()
{
    qDebug() << __FUNCTION__;


    auto dlg = DialogOptions(_profiles, currentUser, this);
    dlg.setModal(true);
    dlg.exec();

    return;


//    QMap<QString, QVariant> clientSett;
//    clientSett.insert("ServerHost", m_client->options()[bConfFieldsWrapper::ServerHost].toString());
//    clientSett.insert("ServerPort", m_client->options()[bConfFieldsWrapper::ServerPort].toInt());
//    clientSett.insert("ServerUser", m_client->options()[bConfFieldsWrapper::User].toString());
//    clientSett.insert("Password", "***");
//    clientSett.insert("pwdEdit", false);
//    clientSett.insert("mozillaExeFile", _profiles->mozillaExeFile());

//    auto dlg = DialogConnection(_sett, clientSett, this);
//    dlg.setModal(true);
//    dlg.exec();

//    if(dlg.result() == QDialog::Accepted){

//        _profiles->setMozillaExeFile(clientSett["mozillaExeFile"].toString());

//        if(db->isOpen())
//            db->close();
//        if(m_client->isStarted())
//            m_client->close();

//        if(_sett->launch_mode() == mixed){
//            if(_sett->customWsUser()){
//                QString pwd = clientSett["Password"].toString();
//                QString usr = clientSett["ServerUser"].toString();
//                if(clientSett["pwdEdit"].toBool()){
//                    QString  hash = bWebSocket::generateHash(usr, pwd);
//                    m_client->options()[bConfFieldsWrapper::Hash] = hash;
//                }
//                m_client->options()[bConfFieldsWrapper::User] = usr;
//                m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
//                m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
//                m_client->options().save();
//            }
//            connectToDatabase();
//        }else{
//            QString pwd = clientSett["Password"].toString();
//            QString usr = clientSett["ServerUser"].toString();
//            if(!_sett->useSettingsFromHttp()){
//                if(clientSett["pwdEdit"].toBool()){
//                    QString  hash = bWebSocket::generateHash(usr, pwd);
//                    m_client->options()[bConfFieldsWrapper::Hash] = hash;
//                }
//                m_client->options()[bConfFieldsWrapper::User] = usr;
//                m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
//                m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
//                m_client->options().save();
//            }else{
//                if(_sett->customWsUser()){
//                    if(clientSett["pwdEdit"].toBool()){
//                        QString  hash = bWebSocket::generateHash(usr, pwd);
//                        m_client->options()[bConfFieldsWrapper::Hash] = hash;
//                    }
//                    m_client->options()[bConfFieldsWrapper::User] = usr;
//                    m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
//                    m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
//                    m_client->options().save();
//                }
//            }
//            connectToWsServer();
//        }

//        _sett->save();

//        updateConnectionStatus();
//    }
}

//void DialogMainWindow::onWhenDataIsLoaded()
//{

//    qDebug() << __FUNCTION__ << currentUser->containers().size() << currentUser->certificates().size();


//    if(_sett->launch_mode() == mixed){
//        if(!db->isOpen()){
//            if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
//                 connectToDatabase();
//        }
//    }else{
//        if(!m_client->isStarted())
//            connectToWsServer();
//    }
//}

//void DialogMainWindow::onEndInitConnection()
//{
//    qDebug() << __FUNCTION__;
//    //sFormLoaded = true;
//    if(m_client->isStarted())
//        m_client->sendCommand("mpl_form_loaded");
//}


void DialogMainWindow::initCsptest()
{

    qDebug() << __FUNCTION__;

    //isUseCsptest = false;

#ifdef _WINDOWS
   //_cprocsp_exe = "C:/Program Files (x86)/Crypto Pro/CSP/csptest.exe";
   _cprocsp_dir = "C:/Program Files (x86)/Crypto Pro/CSP/";
#else
    //_cprocsp_exe = "/opt/cprocsp/bin/amd64/cprocsp";
    _cprocsp_dir = "/opt/cprocsp/bin/amd64/"
#endif

//    QFile csptest(_cprocsp_exe);
//    QString inf;
//    if(!csptest.exists()){
//        inf = "Ошибка: КриптоПро не найден в каталоге установки по умолчанию!";
//        qCritical() << __FUNCTION__ << inf;

//    }
//    else{
//        inf = "КриптоПро найден в каталоге установки по умолчанию.";
//        isUseCsptest = true;

//    }


}

void DialogMainWindow::createColumnAliases()
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
    m_colAliases.insert("typeOperation", "Операция");
    m_colAliases.insert("address", "Ссылка");
    m_colAliases.insert("cert", "Сертификат");
    m_colAliases.insert("profile", "Профиль");
}


void DialogMainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    int iUuid = modelMplProfiles->getColumnIndex("uuid");
    auto uuid = modelMplProfiles->index(index.row(), iUuid).data(Qt::UserRole + iUuid).toString();
    const auto itr = _profiles->profiles().find(QUuid::fromString(uuid));
    if(itr != _profiles->profiles().end()){
        auto dlg = DialogSelectedRow(itr.value(), currentUser, this);
        dlg.setModal(true);
        dlg.exec();

        if(dlg.result() == QDialog::Accepted){
            updateRow(itr.value(), index.row());
        }
    }
}

