#ifndef DIALOGSELECTEDROW_H
#define DIALOGSELECTEDROW_H

#include <QDialog>
//#include "userprofile.h"
#include <certificate.h>
#include <user.h>
#include <qjsontablemodel.h>

namespace Ui {
class DialogSelectedRow;
}

class DialogSelectedRow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectedRow(CertUser* user, const QJsonObject& row = {}, QWidget *parent = nullptr);
    ~DialogSelectedRow();

    void accept() override;

    QJsonObject resultObject();

private:
    Ui::DialogSelectedRow *ui;

    QStringList m_profileNames;
    QMap<int, Certificate*> _certsList;

    QJsonObject _currentObject;
};

#endif // DIALOGSELECTEDROW_H
