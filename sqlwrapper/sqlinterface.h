#ifndef SQLINTERFACE_H
#define SQLINTERFACE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QVector>
#include <QMap>

const QStringList tables = {
        "Users",
        "Messages",
        "Channels",
        "Subscribers",
        "TechnicalInformation",
        "Containers",
        "CertUsers"
};

class SqlInterface //: public QObject
{
    //Q_OBJECT
public:
    explicit SqlInterface(); //QObject *parent = nullptr);
    ~SqlInterface();

    void setHost(const QString& value);
    QString host() const;
    void setSqlUser(const QString& value);
    QString user() const;
    void setSqlPwd(const QString& value);
    QString pwd() const;
    void setDatabaseName(const QString& value);
    QString databaseName() const;
    QString driver();

    bool connect(const QString& driver);
    bool isOpen();

    bool verifyDatabase();
    bool verifyTable(int tableIndex);
    [[nodiscard]] QString queryTemplate(int tableIndex) const;
    QString tableFields(int tableIndex);
    void setIndexes(int tableIndex);
    bool verifyViews();
    bool verifyTables();

    int exec(const QString& query, QString& err);
    int execute(const std::string &query, QString &table, QString &error, bool header = false);

private:
    QString _host;
    QString _user;
    QString _pwd;
    QString _databaseName;
    QString _driverType;

    QSqlDatabase db;

//signals:

};

#endif // SQLINTERFACE_H
