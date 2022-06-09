#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QObject>
#include <QList>
#include <QUuid>

class UserProfile : public QObject
{
    Q_OBJECT
public:
    explicit UserProfile(QObject *parent = nullptr);

    QString name();
    void setName(const QString& value);

    QString defaultAddress();
    void setDefaultAddress(const QString& value);

    QList<QUuid> serificates();
    void setSertificates(const QList<QUuid>& certs);

    void setTypeOperation(const QString& value);
    QString typeOperation();

    void setUuid(const QUuid& value);
    QUuid uuid();

    QString certToString();
    bool isNew();

private:
    QString _address;
    QString _name;
    QList<QUuid> _serificates;
    QString _nameItem;
    QString _type_of_operation;
    QUuid _uuid;   

signals:

};

#endif // USERPROFILE_H
