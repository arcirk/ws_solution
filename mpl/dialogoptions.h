#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include <QDialog>
#include "profilemanager.h"
#include <user.h>

namespace Ui {
class DialogOptions;
}

class DialogOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOptions(ProfileManager* manager, CertUser* usr, QWidget *parent = nullptr);
    ~DialogOptions();

private:
    Ui::DialogOptions *ui;

    ProfileManager* _manager;
    CertUser * _usr;
};

#endif // DIALOGOPTIONS_H
