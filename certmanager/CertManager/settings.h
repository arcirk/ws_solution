#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class settings : public QObject
{
    Q_OBJECT
public:
    explicit settings(QObject *parent = nullptr);

    void setUser(const QString& usr);
    void setServer(const QString& val);
    void setPwd(const QString& val);
    QString user();
    QString server();
    QString pwd();

    void save();

private:
    const QString connectionStringTemplate = "DRIVER={SQL Server};"
                                             "SERVER=%1;DATABASE=%2;Persist Security Info=true;"
                                             "uid=%3;pwd=%4";
    const QString databaseName = "arcirk";

    QString _user;
    QString _pwd;
    QString _server;

signals:

};

#endif // SETTINGS_H