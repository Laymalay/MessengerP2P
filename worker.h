#ifndef WORKER_H
#define WORKER_H

#include <QObject>

#include "master.h"

class Worker : public QObject
{
    Q_OBJECT
public:
     explicit Worker(Master *master, QObject* parent = 0);
    ~Worker();
public slots:
    void process();

signals:
    void finished();
    void newConnection();

private:
    Master *master;
};

#endif // WORKER_H
