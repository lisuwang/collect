#include "slavereadserial.h"
#include <QtSerialPort>
#include <QTime>
QMutex G_Mute;
QString G_GPSInfo;
QT_USE_NAMESPACE
const static int OnceReadLimitLength = 1024;
SlaveThread::SlaveThread(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false)
{
}
//! [0]
SlaveThread::~SlaveThread()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}
//! [0]

//! [1] //! [2]
void SlaveThread::startSlave(const QString &portName, int waitTimeout, const QString &response)
{
//! [1]
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->response = response;
//! [3]
    if (!isRunning())
        start();
}
//! [2] //! [3]

//! [4]
void SlaveThread::run()
{
    bool currentPortNameChanged = false;

    mutex.lock();
//! [4] //! [5]
    QString currentPortName;
    if (currentPortName != portName) {
        currentPortName = portName;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QString currentRespone = response;
    mutex.unlock();
//! [5] //! [6]
    QSerialPort serial;

    while (!quit) {
//![6] //! [7]
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);
            serial.setBaudRate(QSerialPort::Baud115200);
            serial.setDataBits(QSerialPort::Data8);
            serial.setStopBits(QSerialPort::OneStop);
            serial.setParity(QSerialPort::NoParity);
            serial.setFlowControl(QSerialPort::NoFlowControl);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
        }
        QString str;
        if (serial.waitForReadyRead(currentWaitTimeout)) {
//! [7] //! [8]
            // read request
            QByteArray requestData = serial.readAll();
            while (serial.waitForReadyRead(1000) && requestData.length() < OnceReadLimitLength)
                requestData += serial.readAll();
            str = requestData;
//! [8] //! [10]
            //qDebug()<<QString(requestData.toStdString());
            G_Mute.lock();
            G_GPSInfo = str;
            G_Mute.unlock();
            emit this->request(str);
            requestData.clear();
            // write response
            /*QByteArray responseData = currentRespone.toLocal8Bit();
            serial.write(responseData);
            if (serial.waitForBytesWritten(waitTimeout)) {
                QString request(requestData);
//! [12]
                emit this->request(request);
//! [10] //! [11] //! [12]
            } else {
                emit timeout(tr("Wait write response timeout %1")
                             .arg(QTime::currentTime().toString()));
            }*/
//! [9] //! [11]
        } else {
            emit timeout(tr("Wait read request timeout %1")
                         .arg(QTime::currentTime().toString()));
        }
//! [9]  //! [13]
        mutex.lock();
        if (currentPortName != portName) {
            currentPortName = portName;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRespone = response;
        emit this->giveSerialData(str);
        mutex.unlock();
    }
//! [13]
}
