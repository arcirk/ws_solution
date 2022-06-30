#ifndef MPLSETTINGS_H
#define MPLSETTINGS_H

#include <QObject>
#include <QJsonObject>

enum bMplSettings{
    MplPassword = 0,
    MplServerHost,
    MplServerPort,
    MplServerUser,
    MplCharset,
    MplCustomWsUser,
    MplHttpHost,
    MplHttpPwd,
    MplHttpUsr,
    Mpllaunch_mode,
    MplMethod,
    MplPwd,
    MplServer,
    MplUseSettingsFromHttp,
    MplUser,
    MplMozillaExeFile,
    MplProfilesIniFile,
    MplBindCertificates
};

class MplSettings : public QObject
{
    Q_OBJECT
public:
    explicit MplSettings(const QString& home, QObject *parent = nullptr);

    QVariant & operator[] (bMplSettings index);
    QVariant const& operator[] (bMplSettings index) const;


    void save();
    bool parse();
    QString to_string() const;
    QJsonObject to_object();
    void fromObject(const QJsonObject& object);

private:
    QString _homeDir;
    std::vector<QVariant> m_vec;

signals:

};

#endif // MPLSETTINGS_H
