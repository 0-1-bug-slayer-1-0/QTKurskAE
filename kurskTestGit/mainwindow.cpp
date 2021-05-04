#include "mainwindow.h"
#include <QDebug>
#include <QVector>


Widget::Widget(QWidget *parent):QWidget(parent)
{

    connect (pConnect,     &QPushButton::clicked,this, &Widget::StartReading);
    connect (pDisConnect,  &QPushButton::clicked,this, &Widget::StopReading);
    connect (pWriteStart,  &QPushButton::clicked,this, &Widget::StartMotor);
    connect (pWriteStop,   &QPushButton::clicked,this, &Widget::StopMotor);
    connect (pUstirovanie, &QPushButton::clicked,this, &Widget::Ustirovanie);
    connect (pVlevo,       &QPushButton::clicked,this, &Widget::Levo);
    connect (pVpravo,      &QPushButton::clicked,this, &Widget::Pravo);

    connect (timer,       &QTimer::timeout,     this, &Widget::readModbus);
    connect (timerUst,    &QTimer::timeout,     this, &Widget::Ustirovanie);

    connect (pSlider, SIGNAL(valueChanged(int)),this, SLOT(ChangePosition(int)));

    //test
    connect (pSliderPos, SIGNAL(valueChanged(int)),this, SLOT(ChangePos(int)));
    connect (pSliderSpeed, SIGNAL(valueChanged(int)),this, SLOT(ChangePosSpeed(int)));
    connect (pSliderCur, SIGNAL(valueChanged(int)),this, SLOT(ChangeCur(int)));

    pVBoxTest->addWidget(pSliderPos);
    pVBoxTest->addWidget(pSliderSpeed);
    pVBoxTest->addWidget(pSliderCur);
    /*pSliderPos-> setRange(-16383 ,16383 );
     pSliderSpeed-> setRange(-16383*2 ,16383*2 );
      pSliderCur-> setRange(-16383*2 ,16383*2 );
      pSliderPos-> setPageStep(1638);
        pSliderSpeed-> setPageStep(200);
          pSliderCur-> setPageStep(1638);
*/
    this->setVier();
}

void Widget::ChangePos(int i)
{
     addToQueueWrite(1,i);

}

void Widget::ChangePosSpeed(int i)
{
     addToQueueWrite(3,i);

}

void Widget::ChangeCur(int i)
{
     addToQueueWrite(4,i);

}

void Widget::setVier()
{
    tableRegistr-> setColumnCount(2);
    tableRegistr-> setRowCount(15);

    pSlider-> setRange(-16383 ,16383 );
    pSlider-> setPageStep(1638);


    pLeftPos -> setText("-100%");
    pRightPos-> setText("100%");

    pBox1-> addWidget(pConnect);
    pBox1-> addWidget(pDisConnect);
    pBox1-> addWidget(pConnState);


    pBox2-> addWidget(pWriteStart);
    pBox2-> addWidget(pWriteStop);
    pBox2-> addWidget(pUstirovanie);
    pBox2-> addWidget(pTextRes);


    pBox3-> addWidget(pLeftPos);
    pBox3-> addWidget(pSlider);
    pBox3-> addWidget(pRightPos);

    pBox4-> addWidget(pVlevo);
    pBox4-> addWidget(pVpravo);

    pVBox2->addWidget(pRadioButton1);
    pVBox2->addWidget(pRadioButton2);
    pVBox2->addWidget(pRadioButton3);
    pRadioButton2->setChecked(true);
    pBox4->addItem(pVBox2);



    pVBox-> addItem(pBox1);
    pVBox-> addItem(pBox2);
    pVBox-> addItem(pBox3);
    pVBox-> addItem(pBox4);
   // pVBox-> addItem( pVBoxTest);

    pVBox -> addWidget(tableRegistr);

    this -> setLayout(pVBox);
    this -> setGeometry(700,50,700,600);

    setTable();

}

void Widget::setTable()
{
    tableRegistr -> setItem(0,2, new QTableWidgetItem("Счетчик"));
    tableRegistr -> setItem(1,2, new QTableWidgetItem("Ограничение угла против часовой"));
    tableRegistr -> setItem(2,2, new QTableWidgetItem("Ограничение угла по часовой"));
    tableRegistr -> setItem(3,2, new QTableWidgetItem("Превышение ошибки слежения"));
    tableRegistr -> setItem(4,2, new QTableWidgetItem("Смещение ДПР"));
    tableRegistr -> setItem(5,2, new QTableWidgetItem("Золотник РК  положение «0»"));
    tableRegistr -> setItem(6,2, new QTableWidgetItem("Золотник РК  положение «мах»"));
    tableRegistr -> setItem(7,2, new QTableWidgetItem("Слово состояние"));
    tableRegistr -> setItem(8,2, new QTableWidgetItem("Ошибка регулирования по положению в дискретах датчика"));
    tableRegistr -> setItem(9,2, new QTableWidgetItem("Текущий угол в формате датчика положения выходного вала"));
    tableRegistr -> setItem(10,2, new QTableWidgetItem("Текущая скорость"));
    tableRegistr -> setItem(11,2, new QTableWidgetItem("Ток двигателя"));
    tableRegistr -> setItem(12,2, new QTableWidgetItem("Напряжение питания, В"));
    tableRegistr -> setItem(13,2, new QTableWidgetItem("Напряжение двигателя"));
    tableRegistr -> setColumnWidth(0,400);
}

void Widget::transformData()
{
    qDebug()<< buf[0];
     readData[0]=buf[5];
     readData[1]=(buf[7] << 16) | buf[6];
     readData[2]=(buf[9] << 16) | buf[8];
     readData[3]=buf[10];
     readData[4]=buf[11];
     readData[5]=buf[30];
     readData[6]=buf[31];
     readData[7]=buf[32];
     readData[8]=buf[33];
     readData[9]=(buf[34] << 8) | buf[35];
     readData[10]=buf[36];
     readData[11]=buf[37];
     readData[12]=buf[38]*1008/32767;
     readData[13]=buf[39];

    for(int i=0; i<15;i++)
        tableRegistr->setItem(i,3, new QTableWidgetItem(QString::number(readData[i])));

}

void Widget::StartReading()
{
    modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,SerialBaudRateParameter);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,SerialDataBitsParameter);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,  SerialParityParameter);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,SerialPortNameParameter);
    modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,SerialStopBitsParameter);
    modbusDevice->setTimeout(setTimeout);
    modbusDevice->setNumberOfRetries(setNumberOfRetries);
    modbusDevice->connectDevice();


    if (modbusDevice->state()!= QModbusDevice::ConnectedState)
         pConnState -> setText("No Connection");
    else
    {
        pConnState -> setText("Connect");
        timer      -> start(150);
    }

}

void Widget::StopReading()
{
    modbusDevice-> disconnectDevice();
    pConnState  -> setText("State: Disconnect");
    timer       -> stop();

}

void Widget::StartMotor()
{

    addToQueueWrite(2,0);
    addToQueueWrite(3,0);

    pSlider->setSliderPosition(0);

    if(pRadioButton1->isChecked())
         addToQueueWrite(0,0x1082);
     if(pRadioButton2->isChecked())
         addToQueueWrite(0,0x1088);
     if(pRadioButton3->isChecked())
         addToQueueWrite(0,0x1084);

}

void Widget::StopMotor()
{
    addToQueueWrite(0,0);
    addToQueueWrite(4,0);

}

void Widget::ChangePosition(int i)
{
   /* if(pRadioButton2->isChecked())
   addToQueueWrite(1,i);
   if(pRadioButton3->isChecked())
   addToQueueWrite(3,i*2);
   if(pRadioButton1->isChecked())
    addToQueueWrite(4,i*2);
    */

  addToQueueWrite(1,i);

  // addToQueueWrite(2,i*2);
  addToQueueWrite(3,i*2);

   addToQueueWrite(4,i*2);

}

void Widget::readModbus()
{
    if  (!queueReg.isEmpty()){
         writeModbus(queueReg.dequeue(),queueValue.dequeue());
         return;
}

   qDebug()<<("startReading");
   dataToRead.setStartAddress(startAdress);
   dataToRead.setValueCount(dataLens);
   dataToRead.setRegisterType(QModbusDataUnit::HoldingRegisters);

   if(modbusDevice->state() == QModbusDevice::ConnectedState)
   {
       if (auto *reply = modbusDevice->sendReadRequest(dataToRead, modbusAdress))
       {

           if(!reply->isFinished())
          {


               connect(reply, &QModbusReply::finished, this, &Widget::readModbusHeard);
          } else {

              delete reply;
          }
       } else {
           qDebug()<<("reply is null");
       }
   } else {
       qDebug()<<("Slave не подключен");
       pConnState->setText("State: No Slave");
   }

}

void Widget::readModbusHeard()
{qDebug()<<("startReadingSlushat");

    auto reply = qobject_cast<QModbusReply *>(sender());

    if(!reply) return;

    if(reply->error() == QModbusDevice::NoError)
    {
        pConnState->setText("State: Reading good");
        qDebug()<<("end Reading");


        const QModbusDataUnit unit = reply->result();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i)
            buf[i]=unit.value(i);


       transformData();
       reply->deleteLater();
    }
    else
    {
        pConnState->setText("State: Error Reading");
        qDebug()<<("wrong otvet");
        qDebug()<<reply->error();
    }

    reply->deleteLater();

}

void Widget::writeModbus(int registToRight, quint16 commandToWrite)
{

    QModbusDataUnit dataToWrite;
    QVector<quint16> mCommandToWrite(1,commandToWrite);
    dataToWrite.setRegisterType(QModbusDataUnit::HoldingRegisters);
    dataToWrite.setStartAddress(registToRight);
    dataToWrite.setValueCount(1);
    dataToWrite.setValues(mCommandToWrite);

     qDebug()<<"StartWriting Regist:"<< registToRight;
   if(modbusDevice->state() == QModbusDevice::ConnectedState)   {

        if (auto *reply = modbusDevice->sendWriteRequest(dataToWrite, modbusAdress)) {
            if(!reply->isFinished()) {

                connect(reply, &QModbusReply::finished, this, [this, reply](){
                    if (reply->error() == QModbusDevice::ProtocolError) {
                        qDebug()<<"Write response error: %1 (Modbus exception: 0x%2)";

                         qDebug()<<"StoptWriting Regist:1" ;

                    } else if (reply->error() != QModbusDevice::NoError) {
                        qDebug()<<"Write response error: %1 (code: 0x%2)";
                        qDebug()<<"StoptWriting Regist:2" ;


                    }
                     reply->deleteLater();

                        qDebug()<<"StoptWriting Regist:3" ;

                });
             } else {
                qDebug()<<("write bad");
                reply->deleteLater();


             }

            // dataToRead.setStartAddress(1);
           // dataToRead.setValueCount(1);
           // dataToRead.setRegisterType(QModbusDataUnit::HoldingRegisters);

          // modbusDevice->sendReadRequest(dataToRead, modbusAdress);

         } else {
           qDebug()<<("Slave не подключенwrite bad 2 ");

         }

     }

}

void Widget::Ustirovanie()
{
    if (isTimeOut)
    {
        isTimeOut=false;
        addToQueueWrite(0,0x400);
        timerUst->stop();
        return;
    }

    addToQueueWrite(4,16767);
    addToQueueWrite(0,0x81);


isTimeOut = true;
timerUst->start(10000);



}

void Widget::addToQueueWrite(int registr,quint16 value)
{
    queueValue.enqueue(value);
    queueReg.enqueue(registr);

}

void Widget::Levo()
{
    addToQueueWrite(0,0x0100);
}

void Widget::Pravo()
{
    addToQueueWrite(0,0x0200);
}
