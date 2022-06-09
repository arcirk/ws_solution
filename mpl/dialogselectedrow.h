#ifndef DIALOGSELECTEDROW_H
#define DIALOGSELECTEDROW_H

#include <QDialog>
#include "userprofile.h"

namespace Ui {
class DialogSelectedRow;
}

class DialogSelectedRow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectedRow(UserProfile * prof, QWidget *parent = nullptr);
    ~DialogSelectedRow();

    void accept() override;

private:
    Ui::DialogSelectedRow *ui;

    UserProfile * _prof;

    void loadProfiles();
    QStringList m_profileNames;
};

#endif // DIALOGSELECTEDROW_H
