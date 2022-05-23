#ifndef KEYSCONTAINER_H
#define KEYSCONTAINER_H

#include <QObject>
#include <QSettings>

class KeysContainer : public QObject
{
    Q_OBJECT
public:
    explicit KeysContainer(QObject *parent = nullptr);
    explicit KeysContainer(const QString& sid, const QString& localName, QObject *parent = nullptr);

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

private:
    QByteArray _header_key;
    QByteArray _masks_key;
    QByteArray _masks2_key;
    QByteArray _name_key;
    QByteArray _primary_key;
    QByteArray _primary2_key;

    QString _name;

signals:

};

#endif // KEYSCONTAINER_H
