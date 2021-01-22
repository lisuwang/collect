#include "forpublish.h"
#include <QStringList>
#include <QDebug>
#define ROOM_DEBUG
#ifndef ROOM_DEBUG
const QString SolStat::SOL_COMPUTED  = "SOL_COMPUTED";
const QString PosType::NARROW_INT = "NARROW_INT";
const QString ModeInd::A = "A";
const QString QFType::QF = "4";
#else
const QString SolStat::SOL_COMPUTED  = "SOL_COMPUTED"; //COLD_START
const QString PosType::NARROW_INT = "SINGLE"; //NONE
const QString ModeInd::A = "N";
const QString QFType::QF = "0";
#endif





const QString structType::BestPosa = "BestPosa";
const QString structType::Headinga = "Headinga";
const QString structType::XXTRA = "XXTRA";
const QString structType::XXVTG = "XXVTG";

const double PI = 3.1415926;
const double LightSpeed = 3*10^6;
#ifdef WINDOWS_DEBUG
const QString CmdEndChar = "\r\n";
const QString SerialPortName = "COM5";
#else
const QString CmdEndChar = "\n";
const QString SerialPortName = "ttyUSB0";
#endif


extern QMutex G_Mute;
extern QString G_GPSInfo;
ForPublish::ForPublish(QObject *parent) : QObject(parent)
{
    //connect(&thread, &SlaveThread::giveSerialData, this, &ForPublish::showResponse);
    //connect(&thread, &SlaveThread::request, this, &ForPublish::showResponse);
    //connect(&thread, &MasterThread::error, this, &ForPublish::processError);
    //connect(&thread, &MasterThread::timeout, this, &ForPublish::processTimeout);
}

void ForPublish::parseBestPosa(const QStringList &s)
{
    if(s.length() > 12 && s[9].contains(SolStat::SOL_COMPUTED))
    {
        this->SetSolStatus(SolStat::SOL_COMPUTED);
        if(s[10].contains(PosType::NARROW_INT))
        {
            this->bestposa_.lat = s.at(11).toDouble();
            this->bestposa_.lon = s.at(12).toDouble();
            this->SetPosType(PosType::NARROW_INT);
        }
        else
        {
            this->bestposa_.lat = -1;
            this->bestposa_.lon = -1;
        }
    }
    else
    {
        this->bestposa_.lat = -1;
        this->bestposa_.lon = -1;
    }
}

void ForPublish::parseHeadinga(const QStringList &s)
{
    if(s.length() > 20  && s[9].contains(SolStat::SOL_COMPUTED))
    {
        this->SetSolStatus(SolStat::SOL_COMPUTED);
        if(s[10].contains(PosType::NARROW_INT))
        {
            this->headinga_.heading = s[12].toDouble();
            this->headinga_.pitch = s[13].toDouble();
            this->SetPosType(PosType::NARROW_INT);
        }else{
            this->headinga_.heading = 2*PI+1;  //it must be error value
            this->headinga_.pitch = 2*PI+1;    //it must be error value
        }

    }else{
        this->headinga_.heading = 2*PI+1;  //it must be error value
        this->headinga_.pitch = 2*PI+1;    //it must be error value
    }
}

void ForPublish::parseXXVTG(const QStringList &s)
{
    if(s.length() > 9 && s[9].contains(ModeInd::A))
    {
        this->SetMode(ModeInd::A);
        this->xxvtg_.trackTrue = s[1].toDouble();
        this->xxvtg_.speedKm = s[7].toDouble();
    }else{
        this->xxvtg_.trackTrue = 361;  //it must be error value > 359
        this->xxvtg_.speedKm = LightSpeed; // it must be error value > 3*10^6
    }
}

void ForPublish::parseXXTRA(const QStringList &s)
{
    if(s.length() > 8 && s[5] == QFType::QF)
    {
        this->SetQF(QFType::QF);
        this->xxtra_.heading = s[2].toDouble();
        this->xxtra_.pitch = s[3].toDouble();
    }else{
        this->xxtra_.heading = 2*PI+1;  //it must be error value
        this->xxtra_.pitch = 2*PI+1;    //it must be error value
    }
}

void ForPublish::MainThreadReadGPSInfo()
{
    QString goalStr ;
    G_Mute.lock();
    if(!G_GPSInfo.isEmpty())
    {
        qDebug()<<G_GPSInfo;
        goalStr = G_GPSInfo;
        bool ret = false;
        QStringList tmp = TranscateBestPosaSegment(goalStr, ret);
        if(!ret){

            G_Mute.unlock();
            return;
        }
        this->parseBestPosa(tmp);
        tmp = TranscateHeadingaSegment(goalStr, ret);
        if(!ret){
            G_Mute.unlock();
            return;
        }
        this->parseHeadinga(tmp);
        tmp = TranscateXXTRASegment(goalStr, ret);
        if(!ret){
            G_Mute.unlock();
            return;
        }
        this->parseXXTRA(tmp);
        tmp = TranscateXXVTGSegment(goalStr, ret);
        if(!ret){
            G_Mute.unlock();
            return;
        }
        this->parseXXVTG(tmp);
        this->InformGPSStatus();
    }
    G_Mute.unlock();

}

void ForPublish::InformGPSStatus()
{
    this->SetGPSInfo();
}


void ForPublish::SetSolStatus(const QString &status)
{
    this->tipInfo_.solStatus_ = status;
}

void ForPublish::SetPosType(const QString &status)
{
    this->tipInfo_.posType_ = status;
}

void ForPublish::SetQF(const QString &status)
{
    this->tipInfo_.QF_ = status;
}

void ForPublish::SetMode(const QString &status)
{
    this->tipInfo_.mode_ = status;
}

void ForPublish::SetGPSInfo()
{
    gpsInfo_.bestposa_ = this->bestposa_;
    gpsInfo_.headinga_ = this->headinga_;
    gpsInfo_.xxtra_ = this->xxtra_;
    gpsInfo_.xxvtg_ = this->xxvtg_;
}

GPSInfo ForPublish::GetGPSInfo()
{
    this->resquestCount++;
    return gpsInfo_;
}

int ForPublish::GetBestPosaCount() const
{
    return this->bestposaCount;
}

int ForPublish::GetHeadingaCount() const
{
    return this->headingaCount;
}

int ForPublish::GetXXTRACount() const
{
    return this->xxtraCount;
}

int ForPublish::GetXXVTGCount() const
{
    return this->xxvtgCount;
}

QString ForPublish::TranscateRightSegment(const QString &orgin, const QString &from, const QString &end)
{
    Q_ASSERT(!orgin.isEmpty());
    int fromIndex = orgin.indexOf(from);
    int endIndex = orgin.indexOf(end, fromIndex);
    if(fromIndex == -1 || endIndex == -1)
        return QString{""};
    return orgin.mid(fromIndex, endIndex-fromIndex);
}

QStringList ForPublish::TranscateBestPosaSegment(const QString &orgin, bool& ret)
{
    QString str = TranscateRightSegment(orgin, "#BESTPOSA", "\r\n");
    QStringList list = str.split(",");
    if(list.length() > 12)
    {
        this->bestposaCount++;
        ret = true;
        return list;
    }
    ret = false;
    return list;
}

QStringList ForPublish::TranscateHeadingaSegment(const QString &orgin, bool& ret)
{
    QString str = TranscateRightSegment(orgin, "#HEADINGA", "\r\n");
    QStringList list = str.split(",");
    if(list.length() > 20)
    {
        this->headingaCount++;
        ret = true;
        return list;
    }
    ret = false;
    return list;
}

QStringList ForPublish::TranscateXXVTGSegment(const QString &orgin, bool& ret)
{
    QString str = TranscateRightSegment(orgin, "$GPVTG", "\r\n");
    QStringList list = str.split(",");
    if(list.length() > 9)
    {
        this->xxvtgCount++;
        ret = true;
        return list;
    }
    ret = false;
    return list;
}

QStringList ForPublish::TranscateXXTRASegment(const QString &orgin, bool& ret)
{
    QString str = TranscateRightSegment(orgin, "$GPTRA", "\r\n");
    QStringList list = str.split(",");
    if(list.length() > 8)
    {
        this->xxtraCount++;
        ret = true;
        return list;
    }
    ret = false;
    return list;
}

void ForPublish::transaction()
{

    thread.startSlave(SerialPortName,1000,"log version"+CmdEndChar);
}



void ForPublish::showResponse(const QString &s)
{
    this->reponseCount++;
    Q_ASSERT(this->resquestCount == this->reponseCount);
    QStringList list = s.split(",");
    if(list.length() != 0)
    {
        if(list.at(0).contains(structType::BestPosa))
        {
            this->parseBestPosa(list);
            return;
        }

        if(list.at(0).contains(structType::Headinga))
        {
            this->parseHeadinga(list);
            return;
        }

        if(list[0] == structType::XXTRA)
        {
            this->parseXXTRA(list);
            return;
        }

        if(list[0] == structType::XXVTG)
        {
            this->parseXXVTG(list);
            return;
        }
    }
    else
    {
        return;
    }
}

void ForPublish::processError(const QString &s)
{
    qDebug()<<tr("Status: Not running, %1.").arg(s);
}

void ForPublish::processTimeout(const QString &s)
{
    qDebug()<<tr("Status: Running, %1.").arg(s);
}
