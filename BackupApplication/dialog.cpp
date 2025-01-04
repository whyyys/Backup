#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    diaop = {0};
    bool fifo_button = ui->checkBox_fifo->isChecked();
    bool softlink_button = ui->checkBox_softlink->isChecked();
    bool block_button = ui->checkBox_blockdevice->isChecked();
    bool character_button = ui->checkBox_characterdevice->isChecked();

    if(!ui->lineEdit_namereg->text().isEmpty())
    {
        diaop.type_ |= FILTER_FILE_NAME;
        diaop.reg_name_ = ui->lineEdit_namereg->text().toLocal8Bit().constData();
    }
    if(fifo_button ||
        softlink_button || block_button || character_button)
    {
        diaop.type_ |= FILTER_FILE_TYPE;
        if(fifo_button) diaop.file_type_ |= 16;
        if(softlink_button) diaop.file_type_ |= 4;
        if(block_button) diaop.file_type_ |= 32;
        if(character_button) diaop.file_type_ |= 64;
    }
    diaop.type_ |= FILTER_FILE_ACCESS_TIME;
    diaop.atime_start_ = ui->dateTimeEdit_ats->dateTime().toSecsSinceEpoch();
    diaop.atime_end_ = ui->dateTimeEdit_ate->dateTime().toSecsSinceEpoch();
    diaop.type_ |= FILTER_FILE_MODIFY_TIME;
    diaop.mtime_start_ = ui->dateTimeEdit_mts->dateTime().toSecsSinceEpoch();
    diaop.mtime_end_ = ui->dateTimeEdit_mte->dateTime().toSecsSinceEpoch();
    diaop.type_ |= FILTER_FILE_CHANGE_TIME;
    diaop.ctime_start_ = ui->dateTimeEdit_cts->dateTime().toSecsSinceEpoch();
    diaop.ctime_end_ = ui->dateTimeEdit_cte->dateTime().toSecsSinceEpoch();
}
