#include "mainwindow.h"
#include <QApplication>
#include <global.h>
#include <ActivationCode.h>


bool creatConnect();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString MAC;
    unsigned long ulActivationCode;
    unsigned long ulPassword;
    char chMacAddr[12];

    MAC = getHostMacAddress();
    MAC.remove(":");
    memcpy( chMacAddr, MAC.toStdString().c_str() ,static_cast<unsigned int> (MAC.size()));
    ActivationCodeGen(chMacAddr, &ulActivationCode);

    QString path = QDir::currentPath()+"/password.ini";
    QSettings *configIniWrite = new QSettings(path, QSettings::IniFormat);
    configIniWrite->setValue("/MAC/mac", MAC);
    QSettings *configIniRead = new QSettings(path, QSettings::IniFormat);
    ulPassword = configIniRead->value("/password/pw").toString().toULong();

    qDebug()<<ulActivationCode;
    qDebug()<<ulPassword;
    qDebug()<<MAC;

    if(ulActivationCode != ulPassword)
    {
        QMessageBox::warning(nullptr,"warning","Wrong password! Application terminated");
        return 0;
    }

    MainWindow w;
    w.showMaximized();

    return a.exec();
}


