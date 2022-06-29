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
    explicit ProfileManager(const QString& homePath, const QJsonObject& object, QObject *parent = nullptr);

    QMap<QUuid, UserProfile*> &profiles();
    void getSettings();
    void saveSettings();


    QStringList profilesNames();

    void setProfile(UserProfile * prof);
    void clear();

    QJsonObject cache();
    QJsonArray profilesArray();

    void setCache(const QJsonObject& value);

    QString model();

    //QString mozillaProfilesFile();


    void getSettingsFromCache(const QJsonObject& cache);


    //sett
    //"mozilla":
    bool bindCertificates();
    void setBindCertificates(bool value);
    QString mozillaExeFile();
    void setMozillaExeFile(const QString& value);
    QString profilesIniFile();
    void setProfilesIniFile(const QString& value);

    QString _Password;
    QString _ServerHost;
    QString _ServerPort;
    QString _ServerUser;
    QString _charset;
    QString _customWsUser;
    QString _httpHost;
    QString _httpPwd;
    QString _httpUsr;
    QString _launch_mode;
    QString _method;
    QString _pwd;
    QString _server;
    QString _useSettingsFromHttp;
    QString _user;
private:
    //"mozilla":
    bool _bindCertificates;
    QString _mozillaExeFile;
    QString _profilesIniFile;
    //"mpl":
    QString _Password;
    QString _ServerHost;
    QString _ServerPort;
    QString _ServerUser;
    QString _charset;
    QString _customWsUser;
    QString _httpHost;
    QString _httpPwd;
    QString _httpUsr;
    QString _launch_mode;
    QString _method;
    QString _pwd;
    QString _server;
    QString _useSettingsFromHttp;
    QString _user;


    QMap<QUuid, UserProfile*> _profiles;
    QStringList _profilesNames;
    QString _mozillaProfilesFile;
    QString _homePath;


    QString _cache;

    void getConf();
    void loadProfilesFromINI();

signals:

};
#endif // PROFILEMANAGER_H
