#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

enum launchMode{
    ws_srv = 0,
    mixed
};

class settings : public QObject
{
    Q_OBJECT
public:
    explicit settings(QObject *parent = nullptr);

    void setUser(const QString& usr);
    void setServer(const QString& val);
    void setPwd(const QString& val);
    void setLanchMode(launchMode value);

    launchMode launch_mode();
    QString user();
    QString server();
    QString pwd();

    void save();

    QString charset();
    void setCharset(const QString& cp);

    int method();
    void setMethod(int m);


private:
    const QString connectionStringTemplate = "DRIVER={SQL Server};"
                                             "SERVER=%1;DATABASE=%2;Persist Security Info=true;"
                                             "uid=%3;pwd=%4";
    const QString databaseName = "arcirk";

    QString _user;
    QString _pwd;
    QString _server;
    int _method;
    QString _charset;
    launchMode _launch_mode;

signals:

};

#endif // SETTINGS_H
