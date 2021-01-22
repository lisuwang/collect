#ifndef SLAVEREADSERIAL_H
#define SLAVEREADSERIAL_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

//! [0]
class SlaveThread : public QThread
{
    Q_OBJECT

public:
    explicit SlaveThread(QObject *parent = nullptr);
    ~SlaveThread() override;

    void startSlave(const QString &portName, int waitTimeout, const QString &response);
    void run() Q_DECL_OVERRIDE;

signals:
    void request(const QString &s);
    void giveSerialData(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    QString portName;
    QString response;
    int waitTimeout;
    QMutex mutex;
    bool quit;
};
//! [0]

#endif // SLAVEREADSERIAL_H
