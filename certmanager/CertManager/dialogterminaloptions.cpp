#include "dialogterminaloptions.h"
#include "ui_dialogterminaloptions.h"
#include <QStringListModel>

DialogTerminalOptions::DialogTerminalOptions(CommandLine* cmd, Settings * sett, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTerminalOptions)
{
    ui->setupUi(this);

    _cmd = cmd;

    QStringListModel * model = new QStringListModel(enc);
    ui->cmEncoding->setModel(model);

    ui->cmEncoding->setEditable(false);
    //ui->cmEncoding->setEditText(_cmd->currentEncoding());
    ui->cmEncoding->setCurrentIndex(enc.indexOf(_cmd->currentEncoding()));
    ui->chVerefyDecode->setChecked(_cmd->verefyEncoding());

    _sett = sett;

    lst.append(ui->radioButton);
    lst.append(ui->radioButton_2);
    lst.append(ui->radioButton_3);
    lst.append(ui->radioButton_4);

    lst[_sett->method()]->setChecked(true);
}

DialogTerminalOptions::~DialogTerminalOptions()
{
    delete ui;
}

void DialogTerminalOptions::accept()
{
    _cmd->setCurrentEncoding(enc.at(ui->cmEncoding->currentIndex()));
    _cmd->setVerefyEncodingg(ui->chVerefyDecode->checkState() == Qt::Checked);
    _sett->setCharset(enc.at(ui->cmEncoding->currentIndex()));

    for (int i = 0; i < lst.size(); ++i) {
        if(lst[i]->isChecked()){
            _sett->setMethod(i);
            _cmd->setMethod(i);
            break;
        }
    }

    _sett->save();

    QDialog::accept();
}
