#ifndef DIALOGCOMPUTER_H
#define DIALOGCOMPUTER_H

#include <QDialog>
#include <QMap>
#include <QMessageBox>

namespace Ui {
class DialogComputer;
}

class DialogComputer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogComputer(QWidget *parent = nullptr);
    ~DialogComputer();

    QMap<QString, QVariant> computer();
    void setComputer(QMap<QString, QVariant>& comp);

    void accept() override;

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_2_textChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_lineEdit_3_textChanged(const QString &arg1);

private:
    Ui::DialogComputer *ui;
    QMap<QString, QVariant> _comp;
};

#endif // DIALOGCOMPUTER_H
