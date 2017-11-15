
#include "master.h"
#include "worker.h"

Worker::Worker(QObject *parent):
        m_nNextBlockSize(0)
{
    listOfPorts = new QStringList();
    server = new QTcpServer(this);
    portMap = new QMap<int,QTcpSocket*>();

}

void Worker::process(){
    timer = new QTimer();
    timer->setInterval(5000);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotLookUpNewConnections()));
}

void Worker::slotStartServer(QStringList *_listOfPorts, QString _thisPort)
{

    this->listOfPorts = _listOfPorts;
    this->thisPort = _thisPort;
    this->notConnectedYet = listOfPorts;
    notConnectedYet->removeAt(notConnectedYet->indexOf(thisPort));
    qDebug()<<"server: "+thisPort;
    emit sendInfoMessage("server "+ thisPort + " has started");

    if(!server->listen(QHostAddress::Any, thisPort.toInt())){
        QMessageBox::critical(0,"server error",
                              "unable to start the server:"+server->errorString());
        server->close();
        return;
    }
    connect(server, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));
    timer->start();
}


void Worker::sendMsgToSocket(QTcpSocket* pSocket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}

void Worker::slotNewConnection() {
    qDebug()<<"NEW CONNECTION";
    QTcpSocket* socket = server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()),socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadSocket()));
    sendMsgToSocket(socket, "Response: Connected! " + thisPort);
}

void Worker::slotReadSocket()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }
        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime time;
        QString str;
        in >> time >> str;
        QString strMessage = time.toString() + " " + "Client: " + str;
        emit sendInfoMessage(strMessage);
        m_nNextBlockSize = 0;
     }
}
void Worker::slotConnected()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    qDebug()<<socket->peerPort();
    qDebug()<<"Received the connected() signal   ";
    emit sendInfoMessage("Received the connected() signal");
}

void Worker::slotReadyRead()
{
   QTcpSocket* socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_3);
    for(;;){
        if(!m_nNextBlockSize){
            if(socket->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in >> m_nNextBlockSize;
        }
        if(socket->bytesAvailable()<m_nNextBlockSize){
            break;
        }
        QTime time;
        QString str;
        in>>time>>str;
        emit sendInfoMessage(time.toString()+" Server: "+str);
        m_nNextBlockSize = 0;
    }
}

void Worker::slotError(QAbstractSocket::SocketError err)
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    QString strError =
            "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                         "the host wasn't found." :
                         err == QAbstractSocket::RemoteHostClosedError ?
                         "the remote host is closed." :
                         err == QAbstractSocket::ConnectionRefusedError ?
                         "the connection was refused." :
                         QString(socket->errorString())
            );
   emit sendInfoMessage(strError);

}
Worker::~Worker()
{

}

void Worker::slotLookUpNewConnections(){
    for (int i=0;i<notConnectedYet->size();i++){
          QTime timer;
          timer.start();
          QTcpSocket* socket = new QTcpSocket();
          connect(socket, SIGNAL(connected()),this, SLOT(slotConnected()));
          connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
          connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                  this, SLOT(slotError(QAbstractSocket::SocketError)));
          socket->connectToHost("localhost",(notConnectedYet->at(i)).toInt());
          if(socket->waitForConnected(500000)){
              qDebug()<<thisPort+"+"+notConnectedYet->at(i)<< "Connected in"<<timer.elapsed();
              portMap->insert((notConnectedYet->at(i)).toInt(),socket);
              notConnectedYet->removeAt(i);
          }
          else{
              disconnect(socket, SIGNAL(connected()),this, SLOT(slotConnected()));
              disconnect(socket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
              disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                      this, SLOT(slotError(QAbstractSocket::SocketError)));
              socket->deleteLater();
              qDebug()<<thisPort+"+"+notConnectedYet->at(i)<< "Not connected in"<<timer.elapsed();
          }
     }
}
