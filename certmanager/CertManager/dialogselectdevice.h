#ifndef DIALOGSELECTDEVICE_H
#define DIALOGSELECTDEVICE_H

#include <QDialog>

namespace Ui {
class DialogSelectDevice;
}

class DialogSelectDevice : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectDevice(QWidget *parent = nullptr);
    ~DialogSelectDevice();

private slots:
    void on_DialogSelectDevice_accepted();

private:
    Ui::DialogSelectDevice *ui;
};

#endif // DIALOGSELECTDEVICE_H
