#include "wincom.h"
#include "ui_wincom.h"
#include <QString>
#include <QIODevice>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <string>
#include <cctype>
#include <QChar>
#include <QSerialPort>
#include <QSerialPortInfo>

Wincom::Wincom(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Wincom)
{
    ui->setupUi(this);

    ui->closeMyComBtn->setEnabled(false);   //��ʼ���رմ��ڡ���ť������
    ui->sendMsgBtn->setEnabled(false);      //��ʼ���������ݡ���ť������

    ui->sendMsgLenLineEdit->setText("0");
    ui->sendMsgLenLineEdit->setReadOnly(true);

    //QString str = "5f5e";
    //qDebug()<<"hex : "<<str.toInt(0,16);

    //QStringList comPortList = getEnableComPort();
    //qDebug()<<"available com : "<<comPortList<<endl;
}

Wincom::~Wincom()
{
    delete ui;
}

void Wincom::readMyCom()    //�����ں���
{
    /*
    QByteArray temp = myCom->readAll();
    //��ȡ���ڻ��������������ݸ���ʱ����temp
    ui->textBrowser->insertPlainText(temp);
    //�����ڵ�������ʾ�ڴ��ڵ��ı��������*/

    QByteArray temp = myCom->readAll();
    temp = temp.toHex().toUpper();
    ui->textBrowser->insertPlainText(temp);
    ui->textBrowser->insertPlainText("\r\n");

}

void Wincom::on_openMyComBtn_clicked()
{
    //struct PortSettings myComSetting = {BAUD115200,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
    //����һ���ṹ�壬������Ŵ��ڸ�������
    //myCom = new Win_QextSerialPort("COM4",myComSetting,QextSerialBase::EventDriven);
    //���崮�ڶ��󣬲����ݲ������ڹ��캯���������г�ʼ��

    if(ui->portNameComboBox->count() == 0)
    {
        QMessageBox::warning(this,"warning","û�п��õĴ���!�������豸�������ˢ�´��ڡ���ť",QMessageBox::Ok);
        return;
    }
    QString portName = ui->portNameComboBox->currentText();//��ȡ������
    //���崮�ڶ��󣬲����ݲ������ڹ��캯���������г�ʼ��
    myCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);

    if(!myCom->open(QIODevice::ReadWrite))  //�Կɶ�д��ʽ�򿪴���
    {
        QMessageBox::warning(this,"warning","open serial failed!",QMessageBox::Ok);
        return;
    }
    //������Ͽ����ݶԴ��ڽ�������
    //���ò�����
    if(ui->baudRateComboBox->currentText() == tr("9600"))
        myCom->setBaudRate(BAUD9600);
    else if(ui->baudRateComboBox->currentText() == tr("115200"))
        myCom->setBaudRate(BAUD115200);
    //��������λ
    QString dataStr = ui->dataBitsComboBox->currentText();
    if(dataStr == tr("8"))
        myCom->setDataBits(DATA_8);
    else if(dataStr == tr("7"))
        myCom->setDataBits(DATA_7);
    //������żУ��
    QString parStr = ui->parityComboBox->currentText();
    qDebug()<<"parStr : "<<parStr<<endl;
    if(parStr == tr("��"))
        myCom->setParity(PAR_NONE);
    else if(parStr == tr("��"))
        myCom->setParity(PAR_ODD);
    else if(parStr == tr("ż"))
        myCom->setParity(PAR_EVEN);
    //����ֹͣλ
    QString stopStr = ui->stopBitsComboBox->currentText();
    if(stopStr == tr("1"))
        myCom->setStopBits(STOP_1);
    else if(stopStr == tr("2"))
        myCom->setStopBits(STOP_2);
    //��������������
    myCom->setFlowControl(FLOW_OFF);//�������������ƣ�����ʹ�������������Ƶ�Ĭ������
    myCom->setTimeout(500);//������ʱ

    //�źźͲۺ��������������ڻ�����������ʱ�����ж����ڲ���
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));


    ui->openMyComBtn->setEnabled(false);    //�򿪴��ں󡰴򿪴��ڡ���ť������
    ui->closeMyComBtn->setEnabled(true);    //�򿪴��ں󡰹رմ��ڡ���ť����
    ui->sendMsgBtn->setEnabled(true);       //�򿪴��ں󡰷������ݡ���ť����

    //���ø�����Ͽ򲻿���
    ui->baudRateComboBox->setEnabled(false);
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);
}

void Wincom::on_closeMyComBtn_clicked()     //�رմ��ڲۺ���
{
    myCom->close();     //�رմ��ڣ��ú�����win_qextserialport.cpp�ļ��ж���
    ui->openMyComBtn->setEnabled(true); //�رմ��ں󡰴򿪴��ڡ���ť����
    ui->closeMyComBtn->setEnabled(false); //�رմ��ں󡰹رմ��ڡ���ť������
    ui->sendMsgBtn->setEnabled(false);  //�رմ��ں󡰷������ݡ���ť������
    //���ø�����Ͽ����
    ui->baudRateComboBox->setEnabled(true);
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
}

void Wincom::on_sendMsgBtn_clicked()        //�������ݲۺ���
{

    QString sendStr = ui->sendMsgLineEdit->text();
    for(int i=0; i<sendStr.length(); ++i)
    {
        if(QString(sendStr[i]) == " ")
            sendStr.remove(i,1);
    }
    qDebug()<<"sendStr : "<<sendStr.toLatin1();
    if(sendStr.length()%2 != 0)
    {
        QMessageBox::warning(this,"warning","�������ݳ��ȴ���!",QMessageBox::Ok);
        return;
    }
    QByteArray buf;
    QString data;
    for(int i=0; i<sendStr.length()-1; i+=2)
    {
        data = sendStr.mid(i,2);
        buf.append(data.toInt(0,16));
    }
    //��Latin1������ʽ���б༭���е�����д�봮��
    //myCom->write(ui->sendMsgLineEdit->text().toLatin1());

    //��16���Ʒ���
    myCom->write(buf);


    /*QByteArray buf;
    QString str;
    char data;
    QStringList list;
    str = ui->sendMsgLineEdit->text();
    list = str.split(" ");
    for(int i = 0; i < list.count(); i++){
        if(list.at(i) == " ")
            continue;
        if(list.at(i).isEmpty())
            continue;
        data = (char)list.at(i).toInt(0, 16);
        buf.append(data);
    }
    myCom->write(buf);*/
}

void Wincom::init()
{
    QStringList comPortList = getEnableComPort();
    ui->portNameComboBox->clear();
    ui->portNameComboBox->addItems(comPortList);
    //if(!comPortList.isEmpty())
    //{
    //    ui->portNameComboBox->setCurrentIndex(0);
    //}
}

QStringList Wincom::getEnableComPort()
{
    QStringList comPortList;

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            comPortList.append(serial.portName());
            serial.close();
        }
    }

    return comPortList;
}

void Wincom::on_updateComPortBtn_clicked()
{
    init();
}
