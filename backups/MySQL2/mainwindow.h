#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pump_db.h>
#include <dialog_getdbpara.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Dialog_GetDbPara *m_dialog_getdbpara;
    void on_SetDbPara_clicked();
    void on_connect_clicked();
    void on_family_changed();
    void on_model_changed();
    void on_speed_changed();

    Pump_db m_pumpDb;
};

#endif // MAINWINDOW_H
