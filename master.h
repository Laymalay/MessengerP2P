#ifndef MASTER_H
#define MASTER_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QTime>
#include <QThread>
#include <QApplication>


namespace Ui {
class master;
}

class Master : public QWidget
{
    Q_OBJECT

public:
    explicit Master(QWidget *parent = 0);
    ~Master();
    QStringList* listOfPorts;
    QString thisPort;
    QMap<int,QTcpSocket*>* portMap;

private:
    quint16 m_nNextBlockSize;  
    QTcpServer* server;
    QTcpSocket* socket;
    Ui::master *ui;

private:
    void sendMsgToSocket(QTcpSocket* pSocket, const QString& str);
    void startConnection();
private slots:
    void slotNewConnection();
    void slotReadSocket();

    void slotConnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotStartServer();
};

#endif // MASTER_H
