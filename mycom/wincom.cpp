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

    ui->closeMyComBtn->setEnabled(false);   //开始“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false);      //开始“发送数据”按钮不可用

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

void Wincom::readMyCom()    //读串口函数
{
    /*
    QByteArray temp = myCom->readAll();
    //读取串口缓冲区的所有数据给临时变量temp
    ui->textBrowser->insertPlainText(temp);
    //将串口的数据显示在窗口的文本浏览器中*/

    QByteArray temp = myCom->readAll();
    temp = temp.toHex().toUpper();
    ui->textBrowser->insertPlainText(temp);
    ui->textBrowser->insertPlainText("\r\n");

}

void Wincom::on_openMyComBtn_clicked()
{
    //struct PortSettings myComSetting = {BAUD115200,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
    //定义一个结构体，用来存放串口各个参数
    //myCom = new Win_QextSerialPort("COM4",myComSetting,QextSerialBase::EventDriven);
    //定义串口对象，并传递参数，在构造函数里对其进行初始化

    if(ui->portNameComboBox->count() == 0)
    {
        QMessageBox::warning(this,"warning","没有可用的串口!请连接设备并点击“刷新串口”按钮",QMessageBox::Ok);
        return;
    }
    QString portName = ui->portNameComboBox->currentText();//获取串口名
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    myCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);

    if(!myCom->open(QIODevice::ReadWrite))  //以可读写方式打开串口
    {
        QMessageBox::warning(this,"warning","open serial failed!",QMessageBox::Ok);
        return;
    }
    //根据组合框内容对串口进行设置
    //设置波特率
    if(ui->baudRateComboBox->currentText() == tr("9600"))
        myCom->setBaudRate(BAUD9600);
    else if(ui->baudRateComboBox->currentText() == tr("115200"))
        myCom->setBaudRate(BAUD115200);
    //设置数据位
    QString dataStr = ui->dataBitsComboBox->currentText();
    if(dataStr == tr("8"))
        myCom->setDataBits(DATA_8);
    else if(dataStr == tr("7"))
        myCom->setDataBits(DATA_7);
    //设置奇偶校验
    QString parStr = ui->parityComboBox->currentText();
    qDebug()<<"parStr : "<<parStr<<endl;
    if(parStr == tr("无"))
        myCom->setParity(PAR_NONE);
    else if(parStr == tr("奇"))
        myCom->setParity(PAR_ODD);
    else if(parStr == tr("偶"))
        myCom->setParity(PAR_EVEN);
    //设置停止位
    QString stopStr = ui->stopBitsComboBox->currentText();
    if(stopStr == tr("1"))
        myCom->setStopBits(STOP_1);
    else if(stopStr == tr("2"))
        myCom->setStopBits(STOP_2);
    //设置数据流控制
    myCom->setFlowControl(FLOW_OFF);//设置数据流控制，我们使用无数据流控制的默认设置
    myCom->setTimeout(500);//设置延时

    //信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作
    connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));


    ui->openMyComBtn->setEnabled(false);    //打开串口后“打开串口”按钮不可用
    ui->closeMyComBtn->setEnabled(true);    //打开串口后“关闭串口”按钮可用
    ui->sendMsgBtn->setEnabled(true);       //打开串口后“发送数据”按钮可用

    //设置各个组合框不可用
    ui->baudRateComboBox->setEnabled(false);
    ui->dataBitsComboBox->setEnabled(false);
    ui->parityComboBox->setEnabled(false);
    ui->stopBitsComboBox->setEnabled(false);
    ui->portNameComboBox->setEnabled(false);
}

void Wincom::on_closeMyComBtn_clicked()     //关闭串口槽函数
{
    myCom->close();     //关闭串口，该函数在win_qextserialport.cpp文件中定义
    ui->openMyComBtn->setEnabled(true); //关闭串口后“打开串口”按钮可用
    ui->closeMyComBtn->setEnabled(false); //关闭串口后“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false);  //关闭串口后“发送数据”按钮不可用
    //设置各个组合框可用
    ui->baudRateComboBox->setEnabled(true);
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);
}

void Wincom::on_sendMsgBtn_clicked()        //发送数据槽函数
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
        QMessageBox::warning(this,"warning","输入数据长度错误!",QMessageBox::Ok);
        return;
    }
    QByteArray buf;
    QString data;
    for(int i=0; i<sendStr.length()-1; i+=2)
    {
        data = sendStr.mid(i,2);
        buf.append(data.toInt(0,16));
    }
    //以Latin1编码形式将行编辑框中的数据写入串口
    //myCom->write(ui->sendMsgLineEdit->text().toLatin1());

    //以16进制发送
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
