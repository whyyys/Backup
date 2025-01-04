#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QDir"
#include "QFileDialog"
#include "Backup_Functions.h"
#include <QMessageBox>

#include "dialog.h"

#define MOD_COMPRESS 1;
#define MOD_ENCRYPT 2;

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_backup_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_pushButton_restore_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_information_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_toolButton_choose_floder_clicked()
{
    ui->textEdit_backuppath->clear();
    QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("View Directory"), QDir::currentPath()));
    ui->textEdit_backuppath->setText(path);
}


void MainWindow::on_toolButton_shoose_file_clicked()
{
    ui->textEdit_backuppath->clear();
    QFileInfo fileinfo;
    QString file_full, path;
    file_full = QFileDialog::getOpenFileName(this, tr("View File"), QDir::currentPath());
    fileinfo = QFileInfo(file_full);
    path = fileinfo.absoluteFilePath();
    ui->textEdit_backuppath->setText(path);
}


void MainWindow::on_toolButton_dstfloder_clicked()
{
    ui->textEdit_dstpath->clear();
    QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("View Directory"), QDir::currentPath()));
    ui->textEdit_dstpath->setText(path);
}


void MainWindow::on_pushButton_run_clicked()
{
    QString root_str = ui->textEdit_backuppath->toPlainText();
    QString dst_str = ui->textEdit_dstpath->toPlainText();

    fs::path root_path(root_str.toLocal8Bit().constData());
    fs::path dst_path(dst_str.toLocal8Bit().constData());

    QString comment = ui->textEdit_comment->toPlainText();
    QString password = ui->textEdit_password->toPlainText();

    bool compress_button = ui->checkBox_compress->isChecked();
    bool encrypt_button = ui->checkBox_encrypt->isChecked();

    if(encrypt_button && ui->textEdit_password->document()->isEmpty()){
        QMessageBox::critical(this, "错误", "请设置加密密码。");
        return;
    }
    BackupFunctions task(root_path, dst_path, "", "", comment.toLocal8Bit().constData(), password.toLocal8Bit().constData());

    /* 过滤器 */
    task.SetFilter(mainop.type_,mainop.reg_name_,mainop.file_type_,
                   mainop.atime_start_,mainop.atime_end_,
                   mainop.mtime_start_,mainop.mtime_end_,
                   mainop.ctime_start_,mainop.ctime_end_);
    // task.SetFilter(0,".*test.*",0,0,0,0,0,0,0);
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << mainop.type_<<mainop.reg_name_<<
    //     mainop.file_type_;

    unsigned char mod = 0;
    if(compress_button) mod |= MOD_COMPRESS;
    if(encrypt_button) mod |= MOD_ENCRYPT;
    task.SetMod(mod);

    task.CreateBackup();
    std::vector<std::string> outinfo = task.Getoutinfo();
    outinfo.insert(outinfo.begin(), "------Pack information------");
    for(const auto& str : outinfo)
        ui->textBrowser->append(QString::fromStdString(str));
}


void MainWindow::on_toolButton_shoose_file_2_clicked()
{
    ui->textEdit_backuppath_2->clear();
    QFileInfo fileinfo;
    QString file_full, path;
    file_full = QFileDialog::getOpenFileName(this, tr("View File"), QDir::currentPath());
    fileinfo = QFileInfo(file_full);
    path = fileinfo.absoluteFilePath();
    ui->textEdit_backuppath_2->setText(path);
}


void MainWindow::on_toolButton_dstfloder_2_clicked()
{
    ui->textEdit_dstpath_2->clear();
    QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("View Directory"), QDir::currentPath()));
    ui->textEdit_dstpath_2->setText(path);
}


void MainWindow::on_pushButton_unpack_clicked()
{
    QString file_str = ui->textEdit_backuppath_2->toPlainText();
    QString restore_str = ui->textEdit_dstpath_2->toPlainText();

    fs::path file_path(file_str.toLocal8Bit().constData());
    fs::path restore_path(restore_str.toLocal8Bit().constData());

    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << restore_path;
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << file_path;
    QString password = ui->textEdit_password_2->toPlainText();

    BackupFunctions task("", "", restore_path, file_path, "", password.toLocal8Bit().constData());
    task.RestoreBackup();
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << statu;
    std::vector<std::string> outinfo = task.Getoutinfo();
    outinfo.insert(outinfo.begin(), "------Unpack information------");
    for(const auto& str : outinfo)
        ui->textBrowser_2->append(QString::fromStdString(str));
}


void MainWindow::on_toolButton_dstfloder_3_clicked()
{
    ui->textEdit_dstpath_3->clear();
    QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("View Directory"), QDir::currentPath()));
    ui->textEdit_dstpath_3->setText(path);
}


void MainWindow::on_pushButton_unpack_2_clicked()
{
    QList<BackupInformation>backupinfolist;
    QString path = ui->textEdit_dstpath_3->toPlainText();
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << path;
    BackupFunctions tmp_task("","","","","","");
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << path;
    QDir dir(path);
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << path;
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (auto fileInfo, fileInfoList) {
        if(fileInfo.isFile() && (fileInfo.suffix() == "pak" || fileInfo.suffix() == "cps" || fileInfo.suffix() == "ept"))
        {
            BackupInformation tmpinfo;
            fs::path filepath(fileInfo.absoluteFilePath().toLocal8Bit().constData());
            tmp_task.GetBackupInfo(filepath, tmpinfo);
            backupinfolist.append(tmpinfo);
        }
    }
    ui->tableWidget->setRowCount(backupinfolist.size()); // 设置行数
    ui->tableWidget->setColumnCount(5); // 5列: 时间戳, 备份路径, 描述信息, 压缩加密选项, 校验和
    ui->tableWidget->setHorizontalHeaderLabels({"Timestamp", "Backup Path", "Comment", "Compression/Encryption", "Checksum"}); // 设置表头

    // 填充数据
    for (int i = 0; i < backupinfolist.size(); ++i) {
        BackupInformation backup = backupinfolist[i];

        // 时间戳转换为日期时间格式
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(backup.timestamp);
        QString timestampStr = dateTime.toString("yyyy-MM-dd HH:mm:ss");
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(timestampStr));

        // 备份路径
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString(backup.backup_path)));

        // 描述信息
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString(backup.comment)));

        // 压缩加密选项 (可以用数字或字符串表示，示例中使用 0x03 -> "Compression+Encryption")
        // QString modStr = QString("0x%1").arg(backup.mod, 0, 16).toUpper();
        QString modStr = "PACK";
        // qDebug() << __FUNCTION__ << __LINE__ << "  : " << backup.mod;
        if(backup.mod == 1) modStr = "COMPRESS";
        if(backup.mod == 3) modStr = "ENCRYPT";
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(modStr));

        // 校验和
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(backup.checksum)));
    }
}


void MainWindow::on_pushButton_filter_clicked()
{
    Dialog d;
    d.exec();
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << d.diaop.type_<<d.diaop.reg_name_<<
        // d.diaop.file_type_;
    mainop = d.diaop;
    // qDebug() << __FUNCTION__ << __LINE__ << "  : " << mainop.type_<<mainop.reg_name_<<
        // mainop.file_type_;
}


