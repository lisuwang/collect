//#define WINDOWS_QT

#ifdef WINDOWS_QT
#include <QCoreApplication>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QThread>
#include <QTextStream>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QSemaphore>
#endif


#include <vector>
#include <string>
#include <QMutex>
#include "forpublish.h"


/*int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    ForPublish readyer;
    readyer.transaction();
    while(true)
    {
        readyer.MainThreadReadGPSInfo();
        //Sleep(100);
    }
    QThread::currentThread()->wait();
    return a.exec();


}
*/
