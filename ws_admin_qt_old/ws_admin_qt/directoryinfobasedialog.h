#ifndef DIRECTORYINFOBASEDIALOG_H
#define DIRECTORYINFOBASEDIALOG_H

#include <QDialog>

namespace Ui {
class DirectoryInfoBaseDialog;
}

class DirectoryInfoBaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DirectoryInfoBaseDialog(QWidget *parent = nullptr);
    ~DirectoryInfoBaseDialog();

private:
    Ui::DirectoryInfoBaseDialog *ui;
};

#endif // DIRECTORYINFOBASEDIALOG_H
