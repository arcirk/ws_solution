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

DialogMainWindow::DialogMainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMainWindow)
{
    ui->setupUi(this);

    QString dir = QDir::homePath();

#ifdef Q_OS_WINDOWS
    dir.append("/AppData/Roaming");
    dirName = "mpl";
#else
    dirName = ".mpl";
#endif

    appHome = QDir(dir + "/" + dirName);

    if(!appHome.exists())
       appHome.mkpath(".");

    ui->horizontalLayout->addStretch();

    setWindowTitle("Настройки профилей пользователя");

    QTableWidget * table = ui->tableWidget;
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels(QStringList{"Профиль", "Вид операции", "Адрес", "Сертификаты", "Идентификатор"}); // "IsRelative", "Default"});
    table->setColumnHidden(3, true);
    table->setColumnHidden(4, true);

    _profiles = new ProfileManager(appHome.path(), this);
    updateFromData();

    table->resizeColumnToContents(0);
    table->resizeColumnToContents(1);
    table->setSelectionMode(QTableWidget::SingleSelection);

    createTrayActions();
    createTrayIcon();
    trayIcon->show();

    mozillaApp = new QProcess(this);

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
    QApplication::exit();
}

