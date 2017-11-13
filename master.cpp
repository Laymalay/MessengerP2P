
#include "master.h"
#include "ui_master.h"
#include "worker.h"


Master::Master(QWidget *parent) :
    QWidget(parent),
    m_nNextBlockSize(0),
    ui(new Ui::master)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    listOfPorts = new QStringList();
    server = new QTcpServer(this);
    portMap = new QMap<int,QTcpSocket*>();


    QFile file(":/config/ports.txt");
        if(file.open(QIODevice::ReadOnly |QIODevice::Text))
        {
            while(!file.atEnd())
            {
                QString str = file.readLine();
                str.remove(str.length()-1,1);
                listOfPorts->append(str);
            }
        }
        else
        {
            qDebug()<< "can't open file";
        }
        ui->portBox->addItems(*listOfPorts);
        connect(ui->startbtn, SIGNAL(clicked()),this, SLOT(slotStartServer()));
//        connect(ui->connectbtn, SIGNAL(clicked()),this, SLOT(slotStartConnection()));
}



void Master::slotStartServer()
{

    thisPort = ui->portBox->currentText();
    qDebug()<<"server: "+thisPort;
    ui->info->append("server "+ thisPort + " has started");

    if(!server->listen(QHostAddress::Any, thisPort.toInt())){
        QMessageBox::critical(0,"server error",
                              "unable to start the server:"+server->errorString());
        server->close();
        return;
    }
    connect(server, SIGNAL(newConnection()),this,SLOT(slotNewConnection()));

    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    QThread* thread = new QThread;
    Worker* worker = new Worker(this);
    QObject::connect(thread, &QThread::started, worker, &Worker::process);
    QObject::connect(worker, &Worker::finished, thread, &QThread::quit);
    QObject::connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    worker->moveToThread(thread);
    thread->moveToThread(thread);
    thread->start();
//    QStringList *notConnectedYet = this->listOfPorts;
//    notConnectedYet->removeAt(notConnectedYet->indexOf(this->thisPort));
//    for(;;){
//      for (int i=0;i<notConnectedYet->size();i++){
//              QTime timer;
//              timer.start();
//              QTcpSocket* socket = new QTcpSocket();
//              //socket->moveToThread(mainThread);
//              socket->connectToHost("localhost",(notConnectedYet->at(i)).toInt());
//              connect(socket, SIGNAL(connected()),this, SLOT(slotConnected()));
//              connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));
//              connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
//                      this, SLOT(slotError(QAbstractSocket::SocketError)));
//              if(socket->waitForConnected(500000)){
//                  qDebug()<<this->thisPort+"+"+notConnectedYet->at(i)<< "Connected in"<<timer.elapsed();
//                  qDebug()<<socket->state();
//                  this->portMap->insert((notConnectedYet->at(i)).toInt(),socket);
//                  notConnectedYet->removeAt(i);
//              }
//              else{
//                  socket->deleteLater();
//                  qDebug()<<this->thisPort+"+"+notConnectedYet->at(i)<< "Not connected in"<<timer.elapsed();
//              }

//         }
//      QThread::sleep(5);
//      if (notConnectedYet->size()==0){
//          qDebug()<<"finished";
//          break;
//      }
//    }

}



void Master::sendMsgToSocket(QTcpSocket* pSocket, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    pSocket->write(arrBlock);
}

void Master::slotNewConnection() {
    qDebug()<<"NEW CONNECTION";
    for(auto e : portMap->toStdMap())
    {
      qDebug()<< e.first << "," << e.second;
    }
    QTcpSocket* socket = server->nextPendingConnection();
    qDebug()<<socket<<socket->peerPort();
    connect(socket, SIGNAL(disconnected()),socket, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadSocket()));
    sendMsgToSocket(socket, "Response: Connected! " + thisPort);
}

void Master::slotReadSocket()
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
        ui->info->append(strMessage);
        m_nNextBlockSize = 0;
     }
}
void Master::slotConnected()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    qDebug()<<socket->peerPort();
    qDebug()<<"Received the connected() signal   ";
    ui->info->append("Received the connected() signal");
}

void Master::slotReadyRead()
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
        ui->info->append(time.toString()+" Server: "+str);
        m_nNextBlockSize = 0;
    }
}

void Master::slotError(QAbstractSocket::SocketError err)
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
   ui->info->append(strError);
}

Master::~Master()
{
    delete ui;
}
