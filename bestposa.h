#ifndef BESTPOSA_H
#define BESTPOSA_H
#include <QString>
struct SolStat
{
    static const QString SOL_COMPUTED;
};

struct PosType
{
    static const QString NARROW_INT;
};

struct QFType
{
    static const QString QF;
};

struct ModeInd
{
    static const QString A;
};

struct structType
{
    static const QString BestPosa ;
    static const QString Headinga ;
    static const QString XXTRA ;
    static const QString XXVTG ;
    static const QString Reserved ;
};



struct BestPosa
{
    //编号从1开始
    QString header;  //1
    QString solStat; //2 SOL_COMPUTED,...
    QString posType; //3 NARROW_INT
    double lat;      //4 31.2321323
    double lon;      //5 116.3243242
    double hgt;      //6 19.754
    QString Datum;   //8 WG84
    double DiffAge;  //13  0.000
    double solAge;   //14 6223.0以秒计
    int SVs;         //15 跟踪到的卫星数
    int solnSVs;     //16 参与RTK解算的卫星数
    QString CRC;     //23 *292eba23
};


struct Headinga
{
    //编号从1开始
    QString header;  //1
    QString solStat; //2 SOL_COMPUTED,...
    QString posType; //3 NARROW_INT
    double Length;   //4 基线长度 1.2343
    double heading;  //5 方位角(0~360), 200.344
    double pitch;    //6 倾斜角(0~90),
    int SVs;         //11 跟踪到的卫星数
    int solnSVs;     //12 参与RTK解算的卫星数
    int obs;         //13 截止高度角以上的卫星数 12
    QString CRC;     //21 *292eba23

};

struct XXTRA //方位角信息 ，需要双天线
{
    //编号从1开始
    QString XXTRA;  //1 报文头
    double uts;     //2 UTC时间
    double heading; //3 方向角（0~360）, hhh.hh 044.56
    double pitch;   //4 俯仰角(-90~90), ppp.pp -09.74
    double roll;    //5 横滚角(-90~90), rrr.rr 0
    int QF;         //6 0 无效解 1 单点定位解 2 伪距差分 4 固定解 5 浮动解
    int satNo;      //7 卫星数
    double age;     //8 差分延迟
    int stnID;      //9 基站号
    QString Checksum;//10 *hh
    QString end;     //11 结束符<CR><LF>

};

struct XXVTG //地面速度信息 $GLVTG,46.954,T,46.954,M,0.436,N,0.807,K,A*33
{
    //编号从1开始
    QString XXVTG;      //1 报文头
    double trackTrue;   //2 运动角度(000~359,前导位数不足则补0)
    QString T;          //3 真北参照系
    double trackMag;    //4 运动角度(000~359,前导位数不足则补0)
    QString M;          //5 磁北参照系
    double speedKn;     //6 水平运动速度0.00(前导位数不足则补0)
    QString N;          //7 节,Knots
    double speedKm;     //8 水平运动速度0.00(前导位数不足则补0)
    QString K;          //9 公里/时, km/h
    QString modeInd;    //10 定位模式
    QString Checksum;   //11 *hh *24
    QString end;        //12 结束符
};

struct GPSInfo
{
    BestPosa bestposa_;
    Headinga headinga_;
    XXTRA xxtra_;
    XXVTG xxvtg_;

};

#endif // BESTPOSA_H
