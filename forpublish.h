#ifndef FORPUBLISH_H
#define FORPUBLISH_H

#include <QObject>
#include "slavereadserial.h"
#include "bestposa.h"
//#define WINDOWS_DEBUG

#if defined MAKE_TEST_LIB
    #define TEST_LIB_EXPORT Q_DECL_EXPORT
#else
    #define TEST_LIB_EXPORT Q_DECL_IMPORT
#endif

class TEST_LIB_EXPORT ForPublish : public QObject
{
    Q_OBJECT
public:
    explicit ForPublish(QObject *parent = nullptr);
    void parseBestPosa(QStringList const& s);
    void parseHeadinga(QStringList const& s);
    void parseXXVTG(QStringList const& s);
    void parseXXTRA(QStringList const& s);
    void MainThreadReadGPSInfo();
    void InformGPSStatus();
    void SetSolStatus(const QString& status);
    void SetPosType(const QString& status);
    void SetQF(const QString& status);
    void SetMode(const QString& status);
    void SetGPSInfo();
    GPSInfo GetGPSInfo();
    int GetBestPosaCount()const;
    int GetHeadingaCount()const;
    int GetXXTRACount()const;
    int GetXXVTGCount()const;
    void transaction();
private:
    QString TranscateRightSegment(const QString& orgin, const QString& from, const QString& end);
    QStringList TranscateBestPosaSegment(const QString& orgin, bool& ret);
    QStringList TranscateHeadingaSegment(const QString& orgin, bool& ret);
    QStringList TranscateXXVTGSegment(const QString& orgin, bool& ret);
    QStringList TranscateXXTRASegment(const QString& orgin, bool& ret);

signals:

public slots:

    void showResponse(const QString &s);
    void processError(const QString &s);
    void processTimeout(const QString &s);

private:

    //MasterThread thread;
    SlaveThread thread;
    BestPosa bestposa_;
    Headinga headinga_;
    XXTRA xxtra_;
    XXVTG xxvtg_;
    struct StatusInfo
    {
        QString solStatus_;
        QString posType_;
        QString QF_;
        QString mode_;
    };
    StatusInfo tipInfo_;
    GPSInfo gpsInfo_;
    int resquestCount = 0;
    int reponseCount = 0;
    int bestposaCount = 0;
    int headingaCount = 0;
    int xxtraCount = 0;
    int xxvtgCount = 0;
};

#endif // FORPUBLISH_H
