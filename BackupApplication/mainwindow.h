#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Backup_Functions.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FilterOp mainop = {0};
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_backup_clicked();

    void on_pushButton_restore_clicked();

    void on_pushButton_information_clicked();

    void on_toolButton_choose_floder_clicked();

    void on_toolButton_shoose_file_clicked();

    void on_toolButton_dstfloder_clicked();

    void on_pushButton_run_clicked();

    void on_toolButton_shoose_file_2_clicked();

    void on_toolButton_dstfloder_2_clicked();

    void on_pushButton_unpack_clicked();

    void on_toolButton_dstfloder_3_clicked();

    void on_pushButton_unpack_2_clicked();

    void on_pushButton_filter_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
