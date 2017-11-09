#ifndef MASTER_H
#define MASTER_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include<QMap>
#include<QTcpSocket>
#include<QTcpServer>
#include<QMessageBox>
#include<QTime>

namespace Ui {
class master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    explicit Master(QWidget *parent = 0);
    ~Master();

private:
    quint16 m_nNextBlockSize;
    QStringList* listOfPorts;
    QString thisPort;
    QTcpServer* server;
    QTcpSocket* socket;
    QMap<int,QTcpSocket*>* portMap;
    Ui::master *ui;

private:
    void sendMsgToSocket(QTcpSocket* pSocket, const QString& str);
private slots:
    void slotStartConnection();
    void slotNewConnection();
    void slotReadSocket();

    void slotConnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotStartServer();
};

#endif // MASTER_H
