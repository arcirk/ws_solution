#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>

class UserInfo : public QObject
{
    Q_OBJECT
public:
    explicit UserInfo(QObject *parent = 0);

    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString department READ department WRITE setDepartment NOTIFY departmentChanged)
    Q_PROPERTY(QString jsonText READ jsonText WRITE setJsonText NOTIFY jsonTextChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged);
    Q_PROPERTY(QString userUuid READ userUuid NOTIFY userUuidChanged);

    QString userName() const;
    QString city() const;
    QString department() const;
    QString jsonText() const;
    QString status() const;
    QString userUuid() const;

    void setUserName(const QString& name);
    void setCity(const QString& name);
    void setDepartment(const QString& name);
    void setJsonText(const QString& jtext);


    Q_INVOKABLE void setServerResponse(const QString& resp);

signals:
    void userNameChanged();
    void cityChanged();
    void departmentChanged();
    void jsonTextChanged();
    void statusChanged();
    void parseJson();
    void userUuidChanged();

private:
    QString m_userName;
    QString m_organization;
    QString m_department;
    QString m_json;
    QString m_status;
    QString m_userUuid;
};

#endif // USERINFO_H
