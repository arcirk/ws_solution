#ifndef DIALOGSELECTPROFILE_H
#define DIALOGSELECTPROFILE_H

#include <QDialog>

namespace Ui {
class DialogSelectProfile;
}

class DialogSelectProfile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectProfile(const QStringList& list, QWidget *parent = nullptr);
    ~DialogSelectProfile();

    void accept() override;

    QString getResult();

private slots:

private:
    Ui::DialogSelectProfile *ui;
    QString m_result;
};

#endif // DIALOGSELECTPROFILE_H
