#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QJsonObject>

enum bConfFieldsWrapper{
    ServerHost = 0,
    ServerPort,
    User,
    Hash,
    ServerName,
    AppName,
    AutoConnect,
    SaveHash,
    ServerWorkingDirectory,
    ClientWorkingDirectory,
    isLocalInstallation,
    RunAsService,
    UseLocalWebDavDirectory,
    LocalWebDavDirectory,
    WebDavHost,
    WebDavUser,
    WebDavPwd,
    WebDavSSL
};

class ClientSettings{

public:
    explicit
    ClientSettings();
    explicit
    ClientSettings(const QString& file_name = "", bool public_settings = false);

    QVariant & operator[] (bConfFieldsWrapper index);
    QVariant const& operator[] (bConfFieldsWrapper index) const;

    void save();

    bool parse();
//    QVariant & operator[] (bConfFieldsWrapper index);
//
//    QVariant const& operator[] (bConfFieldsWrapper index) const;

//    ClientSettings &operator=(const ClientSettings& from);
//
//    bool init();
//    void save_settings();
//    QString getJson();
    QJsonObject getJsonObject();
//
//    QString ServerHost;
//    int ServerPort;
//    QString RootUser;
//    QString Hash;
//    QString ServerName;
//    QString AppName;
//    bool AutoConnect;
//    bool SaveHash;
//    QString ServerWorkingDirectory;
//    bool isLocalInstallation;
//    bool RunAsService;
//    bool UseLocalWebDAvDirectory;
//    QString LocalWebDavDirectory;
//    QString WebdavHost;
//    QString WebdavUser;
//    QString WebdavPwd;
//    bool WebdavSSL;

    QString password;
    bool pwdEdit;

    void setSettingsFileName(const QString& fname);
    bool installService(const QString &pwd);
    bool startService(const QString &pwd);
    bool stopService(const QString &pwd);
    bool statusService();

    static std::string crypt(const std::string &source, const std::string& key);


private:
    static void* _crypt(void* data, unsigned data_size, void* key, unsigned key_size);
//    QString fileName;
//    QString ServerStatus;
//    QString ServerWorkingDirectory;

    bool createDaemonFile();
    bool installDaemonFile(const QString &pwd);

    QString output_directory;
    QString output_filename;
    void init(bool server);

    std::vector<QVariant> m_vec;

    static QString get_conf_directory();
    static QString get_conf_dirname();
    static QString get_home();
    static bool verify_directory(const QString& dir_path);
};

#endif // APPSETTINGS_H
