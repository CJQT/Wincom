#ifndef WINCOM_H
#define WINCOM_H

#include <QMainWindow>
#include "win_qextserialport.h"

namespace Ui {
class Wincom;
}

class Wincom : public QMainWindow
{
    Q_OBJECT

public:
    explicit Wincom(QWidget *parent = 0);
    ~Wincom();

public slots:
    void readMyCom();
private slots:
    void on_openMyComBtn_clicked();

    void on_closeMyComBtn_clicked();

    void on_sendMsgBtn_clicked();

    void on_updateComPortBtn_clicked();

private:
    void init();
    QStringList getEnableComPort();

private:
    Ui::Wincom *ui;

    Win_QextSerialPort *myCom;
};

#endif // WINCOM_H
