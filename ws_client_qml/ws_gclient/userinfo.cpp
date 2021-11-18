#include "userinfo.h"

#include <QJsonDocument>
#include <QJsonObject>

UserInfo::UserInfo(QObject *parent)
    : QObject(parent)
{
    m_status = "Не всети";
}

QString UserInfo::userName() const
{
    return m_userName;
}

QString UserInfo::city() const
{
    return m_organization;
}

QString UserInfo::department() const
{
    return m_department;
}

QString UserInfo::jsonText() const
{
    return m_json;
}

QString UserInfo::status() const
{
    return m_status;
}

QString UserInfo::userUuid() const
{
    return m_userUuid;
}

void UserInfo::setUserName(const QString &name)
{
    m_userName = name;
}

void UserInfo::setCity(const QString &name)
{
    m_organization = name;
}

void UserInfo::setDepartment(const QString &name)
{
    m_department = name;
}

void UserInfo::setJsonText(const QString &jtext)
{
    //qDebug() << jtext;
    setServerResponse(jtext);
}

void UserInfo::setServerResponse(const QString &resp)
{

    QJsonDocument json = QJsonDocument::fromJson(resp.toUtf8());

    if(json.isNull())
        return;


    QJsonObject obj = json.object();
    if(obj.contains("performance")){
        m_userName = obj.value("performance").toString();
    }
    if(obj.contains("subdivision")){
        QJsonObject val = obj.value("subdivision").toObject();
        m_organization = val.value("name").toString();
    }
    if(obj.contains("department")){
        QJsonObject val = obj.value("department").toObject();
        m_department = val.value("name").toString();
    }
    if(obj.contains("Active")){
        int m_active = obj.value("Active").toInt();
        if (m_active > 0){
            m_status = "В сети";
        }else
            m_status = "Не всети";

    }
    emit parseJson();
    //qDebug() << resp;

}
