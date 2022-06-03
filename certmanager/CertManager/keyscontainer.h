#ifndef KEYSCONTAINER_H
#define KEYSCONTAINER_H

#include <QObject>
#include <QSettings>
#include <sqlinterface.h>

const QStringList KeyFiles = {
    "header.key",
    "masks.key",
    "masks2.key",
    "name.key",
    "primary.key",
    "primary2.key"
};

typedef std::function<void(const QByteArray&)> set_keys;

class KeysContainer : public QObject
{
    Q_OBJECT
public:
    explicit KeysContainer(QObject *parent = nullptr);
    explicit KeysContainer(const QString& sid, const QString& localName, SqlInterface * db, QObject *parent = nullptr);

    void fromFolder(const QString& folder);

    bool isValid();

    void setName(const QString& value);
    QString name();

    QByteArray header_key();
    QByteArray masks_key();
    QByteArray masks2_key();
    QByteArray name_key();
    QByteArray primary_key();
    QByteArray primary2_key();

    void set_header_key(const QByteArray& value);
    void set_masks_key(const QByteArray& value);
    void set_masks2_key(const QByteArray& value);
    void set_name_key(const QByteArray& value);
    void set_primary_key(const QByteArray& value);
    void set_primary2_key(const QByteArray& value);

    void fromDatabase();
    bool toDataBase();
    QSettings toQSettings();
    void fromQSettings(const QSettings& value);

    QByteArray toByteArray();

private:
    QByteArray _header_key;
    QByteArray _masks_key;
    QByteArray _masks2_key;
    QByteArray _name_key;
    QByteArray _primary_key;
    QByteArray _primary2_key;

    SqlInterface * _db;
    QString _name;

    bool _isValid;

    std::map<std::string, set_keys> set_function();

signals:

};

#endif // KEYSCONTAINER_H
