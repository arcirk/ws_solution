#include "registry.h"
#include <QDir>
#include <QTextCodec>

Registry::Registry(QObject *parent)
    : QObject{parent}
{

}

QStringList Registry::currentUserContainers(const QString& sid)
{
    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys").arg(sid);
    QSettings reg = QSettings(root, QSettings::NativeFormat);
    return reg.childGroups();
}

//QString Registry::currentSID(const QString& name )
//{

//    //auto cmd = QProcess();
//    return "";
//}

//QString Registry::currentWindowsName()
//{
//    QProcess myProcess;

//      QString program;
//      program.append("powershell");
//      QStringList arguments;


//      arguments<<"WHOAMI";
//      QString commandLine = "WHOAMI /USER";
//        commandLine.append("\n");

//      myProcess.start(program);
//      myProcess.write(commandLine.toUtf8());

//      //myProcess.start(program, arguments);

//      if(!myProcess.waitForStarted(500)) //default wait time 30 sec
//      {
//          qDebug() << "Cannot execute:" << program;
//      }
//      myProcess.waitForFinished(500);
//      myProcess.setProcessChannelMode(QProcess::MergedChannels);

////      myProcess.start(program, arguments);
////      myProcess.waitForFinished();

////      QString strTemp = QString::fromLocal8Bit(myProcess.readAllStandardOutput());
////      qDebug() << strTemp;
//      if(myProcess.exitStatus() == QProcess::NormalExit
//         && myProcess.exitCode() == QProcess::NormalExit){;
//          QString r = QString::fromStdString(myProcess.readAllStandardOutput().toStdString());
//          qDebug() << qPrintable(r);
//          qDebug() << "--------------";
//          //QString r1(qPrintable(r));
//          //qDebug() << r;
//          r.replace("\r", "");
//          QStringList l = r.split("\n");
//          qDebug() << l[l.size() - 2].replace("\\", " ");

////          QTextStream stream(myProcess);
////          while (!stream.atEnd()) {
////          qDebug()<<stream.readLine();
////          }

//          //result = r.contains("active (running)") !=  0;
//      } else {
//          qDebug() << qPrintable(QString::fromStdString(myProcess.readAllStandardError().toStdString()));
//      }
//      myProcess.close();

//      return "";

//}

QStringList Registry::currentUser(CertUser * usr)
{
    QProcess cmd;
    QString program;
    program.append("powershell");

    QStringList lstResult;

    //QString commandLine = "chcp 1251 | WHOAMI /USER";
    QString commandLine = "WHOAMI /USER";
    commandLine.append("\n");

    cmd.start(program);
    cmd.write(commandLine.toUtf8());

    if(!cmd.waitForStarted(500))
    {
        qCritical() << __FUNCTION__ << "Cannot execute:" << program;
    }
    cmd.waitForFinished(500);
    cmd.setProcessChannelMode(QProcess::MergedChannels);

    if(cmd.exitStatus() == QProcess::NormalExit
       && cmd.exitCode() == QProcess::NormalExit){;
        //QString r = QString::fromLocal8Bit(cmd.readAll());
        QString r = QTextCodec::codecForName("CP866")->toUnicode(cmd.readAllStandardOutput());
        //qDebug() << __FUNCTION__ << qPrintable(r);
        r.replace("\r", "");
        lstResult = r.split("\n");
        QString result = lstResult[lstResult.size() - 2];
        result.replace("\\", " ");
        QStringList mResult = result.split(" ");
        usr->setDomain(mResult[0]);
        usr->setName(mResult[1]);
        usr->setSid(mResult[2]);       
    } else {
        qCritical() << __FUNCTION__ << qPrintable(QString::fromStdString(cmd.readAllStandardError().toStdString()));
    }
    cmd.close();

    return lstResult;
}

bool Registry::deleteContainer(CertUser *usr, const QString &containerName)
{
    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys").arg(usr->sid());
    QSettings settings(root, QSettings::NativeFormat);

    settings.remove(containerName);
    settings.sync();

    if(settings.status() != QSettings::NoError){
        return false;
    }

    return true;
}

QStringList Registry::localUsers()
{
    QProcess cmd;
    QString program;
    program.append("cmd");

    QStringList lstResult;

    //QString commandLine = QString("net user");
    QString commandLine = QString("wmic useraccount get name");
    commandLine.append("\n");

    cmd.start(program);
    cmd.write(commandLine.toUtf8());

    if(!cmd.waitForStarted(500))
    {
        qCritical() << __FUNCTION__ << "Cannot execute:" << program;
    }
    cmd.waitForFinished(500);
    cmd.setProcessChannelMode(QProcess::MergedChannels);

    if(cmd.exitStatus() == QProcess::NormalExit
       && cmd.exitCode() == QProcess::NormalExit){;
        QString r = QTextCodec::codecForName("CP866")->toUnicode(cmd.readAllStandardOutput());
        r.replace("\r", "");
        lstResult = r.split("\n");
    } else {
        qDebug() << qPrintable(QString::fromStdString(cmd.readAllStandardError().toStdString()));
    }
    cmd.close();

    qDebug() << __FUNCTION__ << "---------------";

    QStringList result{};
    for (int i = lstResult.size()-2; i >= 0; i--) {
        if(lstResult[i].trimmed() == "Name")
            break;
        if(lstResult[i].trimmed().isEmpty())
            continue;
        result.append(lstResult[i].trimmed());
        qDebug() << lstResult[i];
    }
    return result;
}

void Registry::usersFromRegistry(const QString rootNode, const QString &group, QStringList lst)
{
    QSettings settings = QSettings(QString(rootCertNode + "\\%1").arg(group), QSettings::NativeFormat);
    if(settings.value("ProfileImagePath").isValid()){
        QString home = QDir::homePath();

    }
}
