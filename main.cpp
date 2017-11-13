#include <QApplication>
#include "master.h"
#include "worker.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Master master;
    master.show();


    return a.exec();
}
