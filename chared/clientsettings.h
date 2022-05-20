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
    WebDavSSL,
    SQLFormat,
    SQLHost,
    SQLUser,
    SQLPassword
};

class ClientSettings{

public:

    explicit
    ClientSettings(const QString& file_name = "", bool public_settings = false, bool no_save_def = false);

    QVariant & operator[] (bConfFieldsWrapper index);
    QVariant const& operator[] (bConfFieldsWrapper index) const;

    void save();
    bool parse();
    QString to_string() const;
    QJsonObject getJsonObject();

    QString password;
    bool pwdEdit;

    void setSettingsFileName(const QString& fname);
    bool installService(const QString &pwd);
    bool startService(const QString &pwd);
    bool stopService(const QString &pwd);
    bool statusService();

    static bool verifyLocalRoomCacheDirectory(const QString& roomToken);
    static QString getUserRoomsCacheDirectory();

    static std::string crypt(const std::string &source, const std::string& key);

    [[nodiscard]] QString confFileName() const;

private:
    static void* _crypt(void* data, unsigned data_size, void* key, unsigned key_size);

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
