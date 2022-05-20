#ifndef SQLINTERFACE_H
#define SQLINTERFACE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class SqlInterface : public QObject
{
    Q_OBJECT
public:
    explicit SqlInterface(QObject *parent = nullptr);
    ~SqlInterface();

    void setHost(const QString& value);
    QString host() const;
    void setSqlUser(const QString& value);
    QString user() const;
    void setSqlPwd(const QString& value);
    QString pwd() const;
    void setDatabaseName(const QString& value);
    QString databaseName() const;

    bool connect(const QString& driver);
    bool isOpen();

private:
    QString _host;
    QString _user;
    QString _pwd;
    QString _databaseName;
    QString _driverType;

    QSqlDatabase db;

signals:

};

#endif // SQLINTERFACE_H
