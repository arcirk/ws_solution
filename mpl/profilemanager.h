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

    std::unordered_map<QString, UserProfile*> &profiles();
    MplSettings& settings();


    QStringList profilesNames();

    void setProfile(UserProfile * prof);

    void clear();


    void setCache(const QJsonObject& value);

    QString model();
    void fromModel(const QString& modelText);
    QJsonObject to_profiles_table();
    void save();
    QJsonObject to_object();


private:

    std::unordered_map<QString, UserProfile*> _profiles;
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
