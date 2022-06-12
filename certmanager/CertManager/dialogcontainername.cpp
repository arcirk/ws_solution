#include "dialogcontainername.h"
#include "ui_dialogcontainername.h"
#include "keyscontainer.h"
#include <QMessageBox>

const static QString Cyrillic = "йцукенгшщзхъфывапролджэячсмитьё"
        "ЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁ";

DialogContainerName::DialogContainerName(const QString& currName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogContainerName)
{
    ui->setupUi(this);

    auto cnt = KeysContainer();
    cnt.fromContainerName(currName);

    key_name = cnt.keyName();

    ui->lineKeyName->setText(key_name);
    end_date = cnt.notValidAfter();
    end_date.replace(".", "-");
    ui->lineDate->setText(end_date);
    new_name = cnt.name();
    bool is_base64 = currName.indexOf(new_name.toUtf8().toBase64()) != -1;
    ui->checkEncode->setChecked(is_base64);
    ui->lineContainerName->setText(new_name); //currName.right(currName.length() - (ind+1))
    onCheckEncodeToggled(is_base64);

    setWindowTitle("Имя контейнера");

    QString str = ui->lineKeyName->text() + "@" +  ui->lineDate->text() + "-" + fromBase64(ui->lineContainerName->text());
    ui->lblTitle->setText("Представление: " + str);

    connect(ui->checkEncode, &QCheckBox::toggled, this, &DialogContainerName::onCheckEncodeToggled);
    connect(ui->lineContainerName, &QLineEdit::textChanged, this, &DialogContainerName::onContainerNameTextChanged);
}

DialogContainerName::~DialogContainerName()
{
    delete ui;
}

void DialogContainerName::accept()
{
    QString m_name = ui->lineContainerName->text();
    if(isCyrillic(m_name)){
        auto _result = QMessageBox::question(this, "Не допустимые символы", "Использование кириллицы в наименовании не рекомендутеся! Продолжить?");
        if(_result == QMessageBox::No)
            return;
    }
    _name = ui->lineKeyName->text() + "@" +  ui->lineDate->text() + "-" + ui->lineContainerName->text();
    QDialog::accept();
}

QString DialogContainerName::name() const
{
    return _name;
}

QString DialogContainerName::keyName()
{
    return key_name;
}

void DialogContainerName::onCheckEncodeToggled(bool checked)
{
    QString str = ui->lineContainerName->text();

    if(checked)
        ui->lineContainerName->setText(str.toUtf8().toBase64());
    else{
        ui->lineContainerName->setText(fromBase64(str));
    }

    ui->lineContainerName->setEnabled(!checked);
}

QString DialogContainerName::fromBase64(const QString &value)
{
    if(!isBase64(value))
       return value;

    try {
        return QByteArray::fromBase64(value.toUtf8());
    }  catch (std::exception&) {
        return value;
    }
}

bool DialogContainerName::isBase64(const QString &value)
{
    QString s = value.trimmed();
    QRegularExpression re("^[a-zA-Z0-9\\+/]*={0,3}$");
    bool isBase64 = (s.length() % 4 == 0) && re.match(s).hasMatch();
   return isBase64;
}

bool DialogContainerName::isCyrillic(const QString &source)
{
    for (int i = 0; i < source.length();  ++i) {

        if(Cyrillic.indexOf(source.mid(i, 1)) != -1)
            return true;
    }

    return false;
}


void DialogContainerName::onContainerNameTextChanged(const QString &arg1)
{
    QString str = ui->lineKeyName->text() + "@" +  ui->lineDate->text() + "-" + fromBase64(arg1);
    ui->lblTitle->setText("Представление: " + str);
}

