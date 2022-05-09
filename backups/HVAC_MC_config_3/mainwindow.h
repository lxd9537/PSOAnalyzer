#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMessageBox>
#include <QTextCodec>
#include <QFile>
#include <QFileDialog>
#include <qxtcsvmodel.h>

#include <QTextDocument>
#include <QPrinter>
#include <QTextBlock>
#include <QTextStream>
#include <QtDebug>

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

    void on_import_clicked();
    void on_import2_clicked();
    void on_export_clicked();
    void on_export2_clicked();
    void on_print_clicked();
    void on_print2_clicked();


};

#endif // MAINWINDOW_H
