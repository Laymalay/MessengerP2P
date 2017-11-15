#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QTimer>
#include "master.h"

class Worker : public QObject
{
    Q_OBJECT
public:
     explicit Worker(QObject* parent = 0);
    ~Worker(); 
    QStringList* listOfPorts;
    QString thisPort;
    QMap<int,QTcpSocket*>* portMap;
public slots:
    void process();
    void slotStartServer(QStringList *listOfPorts, QString thisPort);

signals:
    void finished();
    void newConnection();
    void sendInfoMessage(QString txt);

private:
    quint16 m_nNextBlockSize;
    QTcpServer* server;
    QTcpSocket* socket;
    QStringList* notConnectedYet;
    QTimer* timer;
private:
    void sendMsgToSocket(QTcpSocket* pSocket, const QString& str);
    void startConnection();
private slots:
    void slotNewConnection();
    void slotReadSocket();
    void slotConnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotLookUpNewConnections();
};

#endif // WORKER_H
