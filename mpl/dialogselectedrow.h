#ifndef DIALOGSELECTEDROW_H
#define DIALOGSELECTEDROW_H

#include <QDialog>
#include "userprofile.h"
#include <certificate.h>
#include <user.h>

namespace Ui {
class DialogSelectedRow;
}

class DialogSelectedRow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectedRow(UserProfile * prof, CertUser* user, bool bindCert, QWidget *parent = nullptr);
    ~DialogSelectedRow();

    void accept() override;

private:
    Ui::DialogSelectedRow *ui;

    UserProfile * _prof;

    void loadProfiles();
    QStringList m_profileNames;
    QMap<int, Certificate*> _certsList;
    bool _bindCert;
    CertUser* _user;

};

#endif // DIALOGSELECTEDROW_H
