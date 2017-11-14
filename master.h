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
private:
     QStringList* listOfPorts;
     Ui::master *ui;
public slots:
     void slotGetInfoMessage(QString txt);

signals:
    void signalStartServer(QStringList *listOfPorts,QString *thisPort);
private slots:
    void slotStartServer();
};

#endif // MASTER_H
