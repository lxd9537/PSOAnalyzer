#ifndef MYSQLTHREAD_H
#define MYSQLTHREAD_H

#include <QThread>

class MySqlThread : public QThread
{
public:
    explicit MySqlThread(QObject *parent = nullptr);



};

#endif // MYSQLTHREAD_H
