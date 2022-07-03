#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include "userprofile.h"
#include <QUuid>
#include <QMap>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <settings.h>
#include "mplsettings.h"


class ProfileManager : public QObject
{
    Q_OBJECT
public:
    explicit ProfileManager(const QString& homePath, QObject *parent = nullptr);
    //explicit ProfileManager(const QString& homePath, const QJsonObject& object, QObject *parent = nullptr);

    QHash<QUuid, UserProfile*> &profiles();
    MplSettings& settings();

//    void getSettings();
//    void saveSettings();


    QStringList profilesNames();

    void setProfile(UserProfile * prof);

    void clear();

    //QJsonObject cache();
    //QJsonArray profilesArray();

    void setCache(const QJsonObject& value);

    QString model();
    QJsonObject to_profiles_table();
    void save();
    QJsonObject to_object();

    //QString mozillaProfilesFile();


//    void getSettingsFromCache(const QJsonObject& cache);


//    //sett
//    //"mozilla":
//    bool bindCertificates();
//    void setBindCertificates(bool value);
//    QString mozillaExeFile();
//    void setMozillaExeFile(const QString& value);
//    QString profilesIniFile();
//    void setProfilesIniFile(const QString& value);

private:

    QHash<QUuid, UserProfile*> _profiles;
    QStringList _profilesNames;
    QString _homePath;
    MplSettings _settings;

    //QString _mozillaProfilesFile;



//    QString _cache;

//    void getConf();

    void readProfiles();

signals:

};
#endif // PROFILEMANAGER_H
