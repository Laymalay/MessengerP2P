
#include "master.h"
#include "worker.h"

Worker::Worker(Master *master, QObject *parent)
{
    this->master = master;
}

void Worker::process(){
    QThread* mainThread = QApplication::instance()->thread();
    QStringList *notConnectedYet = master->listOfPorts;
    notConnectedYet->removeAt(notConnectedYet->indexOf(master->thisPort));
    for(;;){
      for (int i=0;i<notConnectedYet->size();i++){
              QTime timer;
              timer.start();
              QTcpSocket* socket = new QTcpSocket();
              //socket->moveToThread(mainThread);
              connect(socket, SIGNAL(connected()),master, SLOT(slotConnected()));
              connect(socket, SIGNAL(readyRead()),master, SLOT(slotReadyRead()));
              connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                      master, SLOT(slotError(QAbstractSocket::SocketError)));
              socket->connectToHost("localhost",(notConnectedYet->at(i)).toInt());
              if(socket->waitForConnected(500000)){
                  qDebug()<<master->thisPort+"+"+notConnectedYet->at(i)<< "Connected in"<<timer.elapsed();
                  master->portMap->insert((notConnectedYet->at(i)).toInt(),socket);
                  notConnectedYet->removeAt(i);
              }
              else{
                  disconnect(socket, SIGNAL(connected()),master, SLOT(slotConnected()));
                  disconnect(socket, SIGNAL(readyRead()),master, SLOT(slotReadyRead()));
                  disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                          master, SLOT(slotError(QAbstractSocket::SocketError)));
                  socket->deleteLater();
                  qDebug()<<master->thisPort+"+"+notConnectedYet->at(i)<< "Not connected in"<<timer.elapsed();
              }

         }
      QThread::sleep(3);
      if (notConnectedYet->size()==0){
//          emit finished();
//          qDebug()<<"worker has finished";
//          break;
      }
    }
}

Worker::~Worker()
{
}
