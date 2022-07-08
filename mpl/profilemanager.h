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

//#include <boost/uuid/uuid.hpp>
//#include <boost/unordered_map.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include "arcirk.h"

//#include <boost/uuid/uuid.hpp>
//#include <boost/unordered_map.hpp>
//#include <boost/uuid/uuid_generators.hpp>

//namespace arcirk {
//bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result) {
//    using namespace boost::uuids;

//    try {
//        result = string_generator()(maybe_uuid);
//        return result.version() != uuid::version_unknown;
//    } catch(...) {
//        return false;
//    }
//}

//}


//#include <boost/functional/hash.hpp>

//template<>
//struct std::hash<boost::uuids::uuid>
//{
//    size_t operator () (const boost::uuids::uuid& uid)
//    {
//        return boost::hash<boost::uuids::uuid>()(uid);
//    }
//};


class ProfileManager : public QObject
{
    Q_OBJECT
public:
    explicit ProfileManager(const QString& homePath, QObject *parent = nullptr);
    //explicit ProfileManager(const QString& homePath, const QJsonObject& object, QObject *parent = nullptr);

    QMap<QUuid, UserProfile*> &profiles();
    QVector<QUuid> order();
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

//    static bool is_valid_uuid(std::string const& maybe_uuid, boost::uuids::uuid& result) {
//        using namespace boost::uuids;

//        try {
//            result = string_generator()(maybe_uuid);
//            return result.version() != uuid::version_unknown;
//        } catch(...) {
//            return false;
//        }
//    }

private:

    QMap<QUuid, UserProfile*> _profiles;
    QVector<QUuid> _order;

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
