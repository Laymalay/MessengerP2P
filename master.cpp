
#include "master.h"
#include "ui_master.h"
#include "worker.h"


Master::Master(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::master)
{ 
    ui->setupUi(this);

    listOfPorts = new QStringList();
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
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
}



void Master::slotGetInfoMessage(QString txt)
{
     ui->info->append(txt);
}

void Master::slotStartServer()
{
    QString thisPort = ui->portBox->currentText();
    emit signalStartServer(listOfPorts,thisPort);
}

Master::~Master()
{
    delete ui;
}
