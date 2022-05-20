#include "sqlinterface.h"
#include <QDir>

SqlInterface::SqlInterface(QObject *parent)
: QObject{parent}
{

}

SqlInterface::~SqlInterface()
{
    if(db.isOpen())
        db.close();
}

void SqlInterface::setHost(const QString &value)
{
    _host = value;
}

QString SqlInterface::host() const
{
    return _host;
}

void SqlInterface::setSqlUser(const QString &value)
{
    _user = value;
}

QString SqlInterface::user() const
{
    return _user;
}

void SqlInterface::setSqlPwd(const QString &value)
{
    _pwd = value;
}

QString SqlInterface::pwd() const
{
    return _pwd;
}

void SqlInterface::setDatabaseName(const QString &value)
{
    _databaseName = value;
}

QString SqlInterface::databaseName() const
{
    return _databaseName;
}

bool SqlInterface::connect(const QString &driver)
{
    db = QSqlDatabase::addDatabase(driver);

    if (driver == "QSQLITE") {
        QString dbPath = QDir::fromNativeSeparators(databaseName());

        QFile dbFile(dbPath);
        if(!dbFile.exists()){
            return false;
        }
        db.setDatabaseName(dbPath);
    }else if (driver == "QODBC") {
        db.setConnectOptions();
        db.setDatabaseName(QString("DRIVER={SQL Server};"
                    "SERVER=%1;DATABASE=%2;Persist Security Info=true;"
                    "uid=%3;pwd=%4").arg(host(), databaseName(), user(), pwd()));
    }

    return db.open();
}

bool SqlInterface::isOpen()
{
    return db.isOpen();
}
