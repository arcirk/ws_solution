#ifndef QMLMESSAGE_H
#define QMLMESSAGE_H

#include <QObject>

class QmlMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Command READ getCommand WRITE setCommand);
    Q_PROPERTY(QString Message READ getMessage WRITE setMessage);
    Q_PROPERTY(QString uuidSender READ getUuidSender WRITE setUuidSender);
    Q_PROPERTY(QString uuidRecipient READ getUuidRecipient WRITE setUuidRecipient)
    Q_PROPERTY(int DateMessage READ getDateMessage WRITE setDateMessage)

public:
    explicit QmlMessage(QObject *parent = nullptr);

    void setCommand(const QString& cmd);
    QString getCommand() const;
    void setMessage(const QString& msg);
    QString getMessage() const;
    void setUuidSender(const QString& uuid);
    void setUuidRecipient(const QString& uuid);
    void setDateMessage(int _date);
    QString getUuidSender() const;
    QString getUuidRecipient() const;
    int getDateMessage() const;

private:
    QString Command;
    QString Message;
    QString uuidSender;
    QString uuidRecipient;
    int DateMessage;

signals:

};

#endif // QMLMESSAGE_H
