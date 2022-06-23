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

    connect(this, &DialogMainWindow::whenDataIsLoaded, this, &DialogMainWindow::onWhenDataIsLoaded);
    connect(this, &DialogMainWindow::endInitConnection, this, &DialogMainWindow::onEndInitConnection);

    formControl();

    initCsptest();

    _sett = new Settings(this, appHome.path());
    if(_sett->httpHost().isEmpty())
        _sett->setHttpHost(defaultHttpaddrr);

    createConnectionsObjects();

    m_async_await.append(std::bind(&DialogMainWindow::currentUserSid, this));
    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetConteiners, this));
    m_async_await.append(std::bind(&DialogMainWindow::currentUserGetCertificates, this));
    m_async_await.append(std::bind(&DialogMainWindow::getSettingsFromHttp, this));
    m_async_await.append(std::bind(&DialogMainWindow::connectToWsServer, this));
    m_async_await.append(std::bind(&DialogMainWindow::connectToDatabase, this));

    currentUser = new CertUser(this);
    QString curentHost = QSysInfo::machineHostName();
    currentUser->setDomain(curentHost);

    //запуск асинхронных вызовов
    createTerminal();





    //setWsConnectedSignals();

//    if(_sett->launch_mode() == mixed){
//        if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
//             connectToDatabase(_sett, _sett->pwd());
//    }else
//        connectToWsServer();


}

void DialogMainWindow::createConnectionsObjects()
{
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
    _profiles->saveSettings();
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
    QTableWidget * table = ui->tableWidget;
    _profiles->clear();

    for (int row = 0; row < table->rowCount(); row++) {
        auto prof = new UserProfile();
        auto line = getLineEdit(row, 0);
        if(line)
            prof->setName(line->text());
        auto item = ui->tableWidget->item(row, 1);
        if(item)
           prof->setTypeOperation(item->text());
        line = getLineEdit(row, 2);
        if(line)
            prof->setDefaultAddress(line->text());
        item = ui->tableWidget->item(row, 4);
        if(item)
           prof->setUuid(QUuid::fromString(item->text()));

        _profiles->setProfile(prof);
    }

}


void DialogMainWindow::setRow(UserProfile *prof)
{
    QTableWidget * table = ui->tableWidget;
    table->setRowCount(table->rowCount() + 1);
    int row = table->rowCount() -1;

    //имя профиля
    auto *pWidget = getItemWidget(prof->name(), row, 0, SLOT(onSelectProfile()));
    table->setCellWidget(table->rowCount()-1,0,pWidget);
    //вид операции
    auto itemName = new QTableWidgetItem(prof->typeOperation());
    table->setItem(row, 1, itemName);
    //Страница по умолчанию
    pWidget = getItemWidget(prof->defaultAddress(), row, 2, SLOT(onSelectDefaultAddress()));
    table->setCellWidget(table->rowCount()-1,2,pWidget);
    //Сертификаты
    pWidget = getItemWidget(prof->certToString(), row, 3, SLOT(onSelectDefaultCertificate()));
    table->setCellWidget(table->rowCount()-1,3,pWidget);
    //Идентификатор
    auto itemUuid = new QTableWidgetItem(prof->uuid().toString());
    itemUuid->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    table->setItem(row, 4, itemUuid);

}

void DialogMainWindow::updateRow(UserProfile *prof, int row)
{
    auto line = getLineEdit(row, 0);
    if(line)
        line->setText(prof->name());
    auto item = ui->tableWidget->item(row, 1);
    if(item)
        item->setText(prof->typeOperation());
    line = getLineEdit(row, 2);
    if(line)
        line->setText(prof->defaultAddress());
    item = ui->tableWidget->item(row, 4);
    if(item)
        item->setText(prof->uuid().toString());
}

void DialogMainWindow::setProfile(UserProfile *prof)
{
    auto items = _profiles->profiles();
    items.insert(prof->uuid(), prof);
}

QLineEdit *DialogMainWindow::getLineEdit(int row, int col)
{
    QWidget* pWidget = ui->tableWidget->cellWidget(row, col);
    if(!pWidget)
        return nullptr;
    auto *line = pWidget->findChild<QLineEdit*>("lineEdit" + QString::number(row));
    return line;
}

QWidget *DialogMainWindow::getItemWidget(const QString &text, int row, int col, const char* member)
{
    auto *pWidget = new QWidget();
    auto *pToolBtn = new QToolButton();
    auto *pLayoutVal = new QHBoxLayout(pWidget);
    auto *pLineEdit = new QLineEdit();

    pToolBtn->setAutoRaise(true);
    pToolBtn->setText("...");
    pToolBtn->setProperty("row", row);
    pToolBtn->setProperty("col", col);
    pToolBtn->setObjectName("btn" + QString::number(row));
    pLineEdit->setObjectName("lineEdit" + QString::number(row));
    pLineEdit->setFrame(false);
    pLineEdit->setReadOnly(true);
    pLineEdit->setProperty("row", row);
    pLineEdit->setProperty("col", col);

    pLayoutVal->addWidget(pLineEdit);
    pLayoutVal->addWidget(pToolBtn);
    pLayoutVal->setContentsMargins(0,0,0,0);
    pWidget->setLayout(pLayoutVal);
    pLineEdit->setText(text);

    connect(pToolBtn, SIGNAL(clicked()), this, member);
    connect(pLineEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onLineEditCursorPositionChanged(int, int)));

    return pWidget;
}

void DialogMainWindow::formControl()
{
    qDebug() << __FUNCTION__;

    isFormLoaded = false;

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

    QTableWidget * table = ui->tableWidget;
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels(QStringList{"Профиль", "Вид операции", "Адрес", "Сертификаты", "Идентификатор"}); // "IsRelative", "Default"});
    table->setColumnHidden(3, true);
    table->setColumnHidden(4, true);

    _profiles = new ProfileManager(appHome.path(), this);
    if(_profiles->mozillaExeFile().isEmpty()){
        _profiles->setMozillaExeFile("C:\\Program Files\\Mozilla Firefox\\firefox.exe");
    }
    updateFromData();

    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->setSelectionMode(QTableWidget::SingleSelection);

    createTrayActions();
    createTrayIcon();
    trayIcon->show();

    ui->tableWidget->resizeRowsToContents();
}

void DialogMainWindow::createTrayActions()
{
    quitAction = new QAction(tr("&Выйти"), this);
    connect(quitAction, &QAction::triggered, this, &DialogMainWindow::onAppExit);
    showAction = new QAction(tr("&Открыть менеджер профилей"), this);
    connect(showAction, &QAction::triggered, this, &DialogMainWindow::onWindowShow);

    if(_profiles->profiles().size() == 0){
        trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(showAction);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(quitAction);
    }
    else{
        trayIconMenu = new QMenu(this);
        createDynamicMenu();
    }
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

    trayIconMenu->addSeparator();

    auto items = _profiles->profiles();

    for (auto item : items){

        QString sz = item->name();
        sz.append(" / ");
        sz.append(item->typeOperation());
        auto action = new QAction(sz, this);
        action->setProperty("uuid", item->uuid());
        trayIconMenu->addAction(action);
        connect(action, &QAction::triggered, this, &DialogMainWindow::onTrayTriggered);
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
    qCritical() << __FUNCTION__ << what << err;
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
            asyncWait();
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

void DialogMainWindow::addCertificate(const QString &from, const QString &to, const QString &byteArrayBase64, const QString& ref)
{
    qDebug() << __FUNCTION__ <<from << to;

    Certificate* cert = new Certificate(this);
    QString resultText;

    QStringList lst = from.split("/");
    QString storgare = lst.size() > 0 ? lst[0] :  STORGARE_LOCALHOST;
    QString _from = lst.size() > 0 ? lst[1] : from;
    bool result = false;

    QString resultData;

    if(storgare == STORGARE_DATABASE){

    }else if(storgare == STORGARE_LOCALHOST){
        result = cert->fromSha1(_from);
        if(result){
            QJsonObject obj = cert->getObject();
            resultData  = QJsonDocument(obj).toJson().toBase64();
        }
    }

    if(result){
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, "addCertificate", resultData, true);
            currentRecipient = "";
        }
    }else{
        if(!currentRecipient.isEmpty()){
            sendToRecipient(currentRecipient, "addCertificate", "error", true);
            currentRecipient = "";
        }
    }

//    if(!from.isEmpty())
//        cnt->fromContainerName(from);
//    else{
//        resultText = "Не указан источник для копирования контейнера!";
//        qCritical() << __FUNCTION__ << resultText;
//        delete cnt;
//        if(!currentRecipient.isEmpty())
//            sendToRecipient(currentRecipient, "error", resultText);
//        return;
//    }
//    cnt->readData(currentUser->sid());

//    if(!cnt->isValid()){
//        resultText = "Ошибка инициализации источника!";
//        qCritical() << __FUNCTION__ << resultText;
//        delete cnt;
//        if(!currentRecipient.isEmpty())
//            sendToRecipient(currentRecipient, "error", resultText);
//        return;
//    }else{
//        //на клиенте автоматом корректируем имя
//        if(isCyrillic(cnt->originalName())){
//            QString dt = cnt->notValidAfter();
//            dt.replace(".", "-");
//            if(!dt.isEmpty())
//                dt.append("-");
//            cnt->setNewOriginalName(cnt->keyName() + "@" + dt + cnt->name().toUtf8().toBase64());
//            qDebug() << __FUNCTION__ << "В имени контейнера обнаружена кириллица. Переведено в base64 автоматичесвки";
//        }
//    }

//    bool result = false;

//    if(to == ToRegistry){
//        result = cnt->sync(KeysContainer::TypeOfStorgare::storgareTypeRegistry, "", currentUser->sid());
//    }else if(to == ToDatabase){
//        if(_sett->launch_mode() == mixed)
//            result = cnt->sync(db);
//        else
//            result = cnt->sync(m_client);
//    }else if(to == ToRemoteVolume || to == ToRemoteRegistry){
//        QString remoteCommand = to == ToRemoteVolume ? ToVolume : ToRegistry;
//        QString base64 = cnt->toBase64();
//        auto obj = QJsonObject();
//        obj.insert("data", base64);
//        obj.insert("from", QString("\\\\.\\%1\\%2").arg(REMOTEBASE_, cnt->originalName()));
//        obj.insert("to", remoteCommand);
//        QString resp = QJsonDocument(obj).toJson().toBase64();
//        if(!currentRecipient.isEmpty())
//            sendToRecipient(currentRecipient, remoteCommand, resp, true);
//        delete cnt;
//        currentRecipient = "";
//        qDebug() << __FUNCTION__ << "Данные контейнера отправлены по удаленному запросу.";
//        return;
//    }

//    if(result){
//        if(!currentRecipient.isEmpty())
//            sendToRecipient(currentRecipient, to, "Операция успешно выполнена!");
//    }else
//        if(!currentRecipient.isEmpty())
//            sendToRecipient(currentRecipient, "error", "Ошибка выполнения операции!");

//    delete cnt;
//    currentRecipient = "";
//    return;

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

}

void DialogMainWindow::getDatabaseData(const QString& table, const QString& ref, const QJsonObject& param)
{
        qDebug() << __FUNCTION__ << table;

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

        addCertificate(from, to);
    }

}



void DialogMainWindow::onParseCommand(const QVariant &result, int command)
{

    if(command == CmdCommand::wmicGetSID){
//        QString sid = result.toString();
//        qDebug() << __FUNCTION__ << "set sid:" <<  sid;
//        currentUser->setSid(sid);
//        //получаем список контейнеров
//        terminal->send("csptest -keyset -enum_cont -fqcn -verifyc\n", CmdCommand::csptestGetConteiners);
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
            currentRecipient = "";
        }

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }

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

        if(m_async_await.size() > 0){
            auto f = m_async_await.dequeue();
            f();
        }

//        if(!isFormLoaded) //оповещяем о окончании загрузки данных
//            emit whenDataIsLoaded();

        if(!currentRecipient.isEmpty()){
            auto objCerts = QJsonObject();
            objCerts.insert("certs", currentUser->certModel());
            QString object = QJsonDocument(objCerts).toJson(QJsonDocument::Indented);
            sendToRecipient(currentRecipient, "installed_certificates", object.toUtf8().toBase64(), true);
            currentRecipient = "";
        }
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

void DialogMainWindow::on_btnAdd_clicked()
{
    UserProfile * prof = new UserProfile();
    prof->setTypeOperation("Новая настройка");
    auto dlg = new DialogSelectedRow(prof);
    dlg->setModal(true);
    dlg->exec();

    if(dlg->result() == QDialog::Accepted){
        setRow(prof);
        _profiles->setProfile(prof);
    }

}

void DialogMainWindow::onSelectProfile()
{
    auto dlg = new DialogSelectProfile(_profiles->profilesNames(), this);
    dlg->setModal(true);
    dlg->exec();
    if(dlg->result() == QDialog::Accepted){

        auto *button = dynamic_cast<QToolButton*>( sender() );
        QVariant vRow = button->property("row");
        if(vRow.isValid()){
            int row = vRow.toInt();
            int curRow = ui->tableWidget->selectionModel()->currentIndex().row();
            if(row != curRow)
                ui->tableWidget->selectRow(row);

             QWidget* pWidget = ui->tableWidget->cellWidget(row, 0);
             if(pWidget){
                 auto *line = pWidget->findChild<QLineEdit*>("lineEdit" + QString::number(row));
                 if(line)
                     line->setText(dlg->getResult());
             }
             ui->tableWidget->resizeColumnsToContents();
        }
    }
}

void DialogMainWindow::onSelectDefaultAddress()
{
    auto *btn = dynamic_cast<QToolButton*>( sender() );
    if(btn){
        int row = btn->property("row").toInt();
        int col = btn->property("col").toInt();
        ui->tableWidget->setCurrentCell(row, col);
        auto *lbl = getLineEdit(row, col);
        if(lbl){
            bool bOk;
            QString str = QInputDialog::getText( this,
                                                 "Адрес страницы",
                                                 "Адрес:",
                                                 QLineEdit::Normal,
                                                 lbl->text(),
                                                 &bOk
                                                );
            if (bOk) {
                lbl->setText(str);
            }
        }
    }
}

void DialogMainWindow::onSelectDefaultCertificate()
{
    auto *btn = dynamic_cast<QToolButton*>( sender() );
    if(btn){
        int row = btn->property("row").toInt();
        int col = btn->property("col").toInt();
        ui->tableWidget->setCurrentCell(row, col);
    }
}

void DialogMainWindow::updateFromData() {

    QTableWidget * table = ui->tableWidget;
    table->setRowCount(0);

    auto items = _profiles->profiles();
    for (auto itr : items) {
        setRow(itr);
    }
}

void DialogMainWindow::onLineEditCursorPositionChanged(int oldPos, int newPos)
{
    auto *edit = dynamic_cast<QLineEdit*>( sender() );
    if(edit){
        int row = edit->property("row").toInt();
        int col = edit->property("col").toInt();
        ui->tableWidget->setCurrentCell(row, col);
    }

    qDebug() << oldPos << newPos;
}

void DialogMainWindow::currentUserSid()
{
    qDebug() << __FUNCTION__;

    infoBar->setText("Получение данных пользователя ..");

#ifdef _WINDOWS
    if(currentUser->sid().isEmpty())
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

    if(!currentRecipient.isEmpty()){
        sendToRecipient(currentRecipient, "available_containers", currentUser->containers().join("\n"), true);
        currentRecipient = "";
    }
}

void DialogMainWindow::createTerminal()
{

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
    int row = ui->tableWidget->currentRow();
    auto item = ui->tableWidget->item(row, 4);
    if(item){
        auto profs = _profiles->profiles();
        auto itr = profs.find(QUuid::fromString(item->text()));
        if(itr != profs.end()){
            auto dlg = new DialogSelectedRow(itr.value());
            dlg->setModal(true);
            dlg->exec();

            if(dlg->result() == QDialog::Accepted){
                updateRow(itr.value(), row);
            }
        }
    }
}


void DialogMainWindow::on_btnDelete_clicked()
{
    int row = ui->tableWidget->currentRow();
    if(row == -1)
    {
        QMessageBox::critical(this, "Ошибка", "Не выбрана строка!");
        return;
    }
   int result = QMessageBox::information(this,
                                         "Удаление",
                                         "Удалить текущую настройку?",
                                         QMessageBox::Yes | QMessageBox::No);
   if(result == QMessageBox::Yes){
        ui->tableWidget->removeRow(row);
        updateData();
   }
}

void DialogMainWindow::onTrayTriggered()
{
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
        profName = itr.value()->name();
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
    terminal->stop();
    if(m_client->isStarted())
        m_client->close(true);
    if(db->isOpen())
        db->close();
    delete m_client;
    delete db;
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

    status.append(_sett->launch_mode() == mixed ? " Lanch mode: mixed" : " Lanch mode: ws_server");

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

void DialogMainWindow::sendResultToClient()
{
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
    }
}

void DialogMainWindow::asyncWait()
{
    if(m_async_await.size() > 0){
        auto f = m_async_await.dequeue();
        f();
    }
}

void DialogMainWindow::on_btnSettings_clicked()
{
    QMap<QString, QVariant> clientSett;
    clientSett.insert("ServerHost", m_client->options()[bConfFieldsWrapper::ServerHost].toString());
    clientSett.insert("ServerPort", m_client->options()[bConfFieldsWrapper::ServerPort].toInt());
    clientSett.insert("ServerUser", m_client->options()[bConfFieldsWrapper::User].toString());
    clientSett.insert("Password", "***");
    clientSett.insert("pwdEdit", false);
    clientSett.insert("mozillaExeFile", _profiles->mozillaExeFile());

    auto dlg = DialogConnection(_sett, clientSett, this);
    dlg.setModal(true);
    dlg.exec();

    if(dlg.result() == QDialog::Accepted){

        _profiles->setMozillaExeFile(clientSett["mozillaExeFile"].toString());

        //if(!_sett->useSettingsFromHttp()){
            if(db->isOpen())
                db->close();
            if(m_client->isStarted())
                m_client->close();

            if(_sett->launch_mode() == mixed){
                if(_sett->customWsUser()){
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
                connectToDatabase();
            }else{
                QString pwd = clientSett["Password"].toString();
                QString usr = clientSett["ServerUser"].toString();
                if(!_sett->useSettingsFromHttp()){
                    if(clientSett["pwdEdit"].toBool()){
                        QString  hash = bWebSocket::generateHash(usr, pwd);
                        m_client->options()[bConfFieldsWrapper::Hash] = hash;
                    }
                    m_client->options()[bConfFieldsWrapper::User] = usr;
                    m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
                    m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
                    m_client->options().save();
                }else{
                    if(_sett->customWsUser()){
                        if(clientSett["pwdEdit"].toBool()){
                            QString  hash = bWebSocket::generateHash(usr, pwd);
                            m_client->options()[bConfFieldsWrapper::Hash] = hash;
                        }
                        m_client->options()[bConfFieldsWrapper::User] = usr;
                        m_client->options()[bConfFieldsWrapper::ServerHost] = clientSett["ServerHost"].toString();
                        m_client->options()[bConfFieldsWrapper::ServerPort] = clientSett["ServerPort"].toInt();
                        m_client->options().save();
                    }
                }
                connectToWsServer();
            }

//        }else{

//        }
        _sett->save();

        updateConnectionStatus();
    }
}

void DialogMainWindow::onWhenDataIsLoaded()
{

    qDebug() << __FUNCTION__ << currentUser->containers().size() << currentUser->certificates().size();


    if(_sett->launch_mode() == mixed){
        if(!db->isOpen()){
            if(!_sett->server().isEmpty() && !_sett->pwd().isEmpty() && !_sett->user().isEmpty())
                 connectToDatabase();
        }
    }else{
        if(!m_client->isStarted())
            connectToWsServer();
    }
}

void DialogMainWindow::onEndInitConnection()
{
    qDebug() << __FUNCTION__;
    isFormLoaded = true;
    if(m_client->isStarted())
        m_client->sendCommand("mpl_form_loaded");
}


void DialogMainWindow::initCsptest()
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
        isUseCsptest = true;

    }


}
