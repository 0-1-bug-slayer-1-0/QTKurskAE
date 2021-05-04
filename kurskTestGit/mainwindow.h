#ifndef WIDGET_H
#define WIDGET_H
#include <QtWidgets>
#include <QApplication>
#include <QModbusRtuSerialMaster>
#include <QVector>
#include <QTimerEvent>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QSpinBox>
#include <QSerialPort>
#include <QSlider>
#include <QMutex>
#include <QQueue>
#include <QRadioButton>

class Widget: public QWidget
{
    Q_OBJECT


public:

    void StartReading();
    void StopReading();
    void StartMotor();
    void StopMotor();
    void Levo();
    void Pravo();
    int readData[20];


    Widget(QWidget *parent=0);


private:

    //design

    QLabel *pConnState= new QLabel("State: Disconnect");
    QLabel *pTextRes = new QLabel;
    QLabel *pLeftPos = new QLabel;
    QLabel *pRightPos = new QLabel;

    QPushButton *pConnect= new QPushButton ("Connect");
    QPushButton *pDisConnect= new QPushButton ("Disconnect");
    QPushButton *pWriteStart = new QPushButton("Com Start");
    QPushButton *pWriteStop = new QPushButton("Com Stop");
    QPushButton *pUstirovanie = new QPushButton("Ustirovanie");
    QPushButton *pVlevo = new QPushButton("Levo");
    QPushButton *pVpravo = new QPushButton("Pravo");


    QSlider    *pSlider = new QSlider(Qt::Horizontal);
    //test
    QSlider    *pSliderPos = new QSlider(Qt::Horizontal);
    QSlider    *pSliderSpeed  = new QSlider(Qt::Horizontal);
    QSlider    *pSliderCur = new QSlider(Qt::Horizontal);


     QVBoxLayout *pVBoxTest = new QVBoxLayout;

     //test



    QVBoxLayout *pVBox2 = new QVBoxLayout;
    QRadioButton *pRadioButton1 = new QRadioButton("По току");
    QRadioButton *pRadioButton2 = new QRadioButton("По положению");
    QRadioButton *pRadioButton3 = new QRadioButton("По скорости");


    QHBoxLayout *pBox1 = new QHBoxLayout;
    QHBoxLayout *pBox2 = new QHBoxLayout;
    QHBoxLayout *pBox3 = new QHBoxLayout;
    QHBoxLayout *pBox4 = new QHBoxLayout;
    QVBoxLayout *pVBox = new QVBoxLayout;
    QTableWidget *tableRegistr= new QTableWidget;

    void setVier();
    void setTable();

    //Setting modbus

    int SerialBaudRateParameter      = 115200;
    int SerialParityParameter        = 0 ;
    int SerialDataBitsParameter      = 8;
    QString SerialPortNameParameter  = "COM3";
    int SerialStopBitsParameter      = 1;
    int setTimeout                   = 100;
    int setNumberOfRetries           = 0;
    QModbusClient *modbusDevice      = new QModbusRtuSerialMaster;
    int modbusAdress = 1;
    uint dataLens    = 40;//34
    int startAdress  = 0;

    int buf[200];
    QModbusDataUnit dataToRead;//данные для посылки модбасс

    QQueue <quint16> queueValue;
    QQueue <int> queueReg;
    QTimer *timer = new QTimer;//таймер для переодического опроса
    QTimer *timerUst = new QTimer;//таймер для юстирования





    bool isTimeOut=false;


    void transformData();
    void readModbus();
    void readModbusHeard();
    void addToQueueWrite(int registr,quint16 value);
    void writeModbus(int registToRight, quint16 commandToWrite);
    void Ustirovanie();


signals:
    void signalFromBut();

public slots:
    void ChangePosition(int i);

//test
    void ChangePos(int i);
    void ChangePosSpeed(int i);
    void ChangeCur(int i);


};

#endif // WIDGET_H
