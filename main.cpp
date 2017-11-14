#include <QApplication>
#include "master.h"
#include "worker.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Master master;
    master.show();
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    QThread* thread = new QThread;
    Worker* worker = new Worker();
    QObject::connect(thread, &QThread::started, worker, &Worker::process);
    QObject::connect(worker, &Worker::finished, thread, &QThread::quit);
    QObject::connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    worker->moveToThread(thread);
    thread->moveToThread(thread);
    thread->start();

    QObject::connect(&master,&Master::signalStartServer,worker, &Worker::slotStartServer);
    QObject::connect(worker,&Worker::sendInfoMessage,&master, &Master::slotGetInfoMessage);

    return a.exec();
}
