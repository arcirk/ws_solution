#ifndef DIALOGTERMINALOPTIONS_H
#define DIALOGTERMINALOPTIONS_H

#include <QDialog>
#include "commandline.h"
#include "settings.h"
#include <QRadioButton>

const QStringList enc = {"Big5"
                   ,"Big5-HKSCS"
                   ,"CP949"
                   ,"EUC-JP"
                   ,"EUC-KR"
                   ,"GB18030"
                   ,"HP-ROMAN8"
                   ,"IBM 850"
                   ,"IBM 866"
                   ,"IBM 874"
                   ,"ISO 2022-JP"
                   ,"ISO 8859-1"
                   ,"ISO 8859-13"
                   ,"KOI8-R"
                   ,"KOI8-U"
                   ,"Macintosh"
                   ,"Shift-JIS"
                   ,"TIS-620"
                   ,"TSCII"
                   ,"UTF-8"
                   ,"UTF-16"
                   ,"UTF-16BE"
                   ,"UTF-16LE"
                   ,"UTF-32"
                   ,"UTF-32BE"
                   ,"UTF-32LE"
                   ,"Windows-1250"
                   ,"CP866"
                   ,"CP1251"};
namespace Ui {
class DialogTerminalOptions;
}

class DialogTerminalOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTerminalOptions(CommandLine* cmd, settings * sett, QWidget *parent = nullptr);
    ~DialogTerminalOptions();

    void accept() override;

private:
    Ui::DialogTerminalOptions *ui;
    CommandLine* _cmd;
    settings * _sett;
    QList<QRadioButton*> lst;
};

#endif // DIALOGTERMINALOPTIONS_H
