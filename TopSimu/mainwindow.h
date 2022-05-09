#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <modbusserverthread.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void onEventReported(int id, int code);

private:
    Ui::MainWindow *ui;

    CModbusServerThread *m_modbus_server_thread;
};
#endif // MAINWINDOW_H
