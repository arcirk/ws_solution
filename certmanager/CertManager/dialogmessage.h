#ifndef DIALOGMESSAGE_H
#define DIALOGMESSAGE_H

#include <QDialog>

namespace Ui {
class DialogMessage;
}

class DialogMessage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMessage(QWidget *parent = nullptr);
    ~DialogMessage();

    QString resultMessage() const;

    void accept() override;

private:
    Ui::DialogMessage *ui;
    QString _message;
};

#endif // DIALOGMESSAGE_H
