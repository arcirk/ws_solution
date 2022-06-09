#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include "userprofile.h"
#include <QUuid>
#include <QMap>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class ProfileManager : public QObject
{
    Q_OBJECT
public:
    explicit ProfileManager(const QString& homePath, QObject *parent = nullptr);

    QMap<QUuid, UserProfile*> profiles();
    void getSettings();
    void saveSettings();
    QString mozillaProfilesFile();
    QString mozillaExeFile();
    void setMozillaExeFile(const QString& value);
    QStringList profilesNames();

    void setProfile(UserProfile * prof);
    void clear();

private:
    QMap<QUuid, UserProfile*> _profiles;
    QStringList _profilesNames;
    QString _mozillaProfilesFile;
    QString _mozillaExeFile;
    QString _homePath;

    void getConf();
    void loadProfilesFromINI();

signals:

};
#endif // PROFILEMANAGER_H
