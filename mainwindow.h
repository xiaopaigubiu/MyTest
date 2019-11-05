#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QLabel>
#include <QTimer>
#include <windows.h>
#include <QString>
#include <dbt.h>
#include <QDebug>
#include <devguid.h>
#include <QDateTime>
#include <QFileDialog>
#include <QLCDNumber>
//#include<SetupAPI.h>
//#include<InitGuid.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void initPort();
    ~MainWindow();

    int rxCount,txCount;

private slots:
    void ReadData();//读取数据

    void show_portStateLabel();//显示状态栏

    void on_openButton_clicked();

    void on_r_clearButton_clicked();

    void on_s_clearButton_clicked();

    void on_sendButton_clicked();

    void StringToHex(QString str, QByteArray & senddata);//用于发送时进制转换

    char ConvertHexChart(char ch);//十六进制转换

    void readToHex();//将读取的数据以十六进制显示

    void Mdisplay();

    void on_resetCntButton_clicked();

    void on_autoSendCB_clicked(bool checked);//自动发送

    void on_showTimeCB_stateChanged(int arg1);

    void on_saveButton_clicked();

    void showTime();//显示当前时间

private:
    Ui::MainWindow *ui;

    QSerialPort *serialPort;

    QTimer *time;//用于定时发送

    bool DisplayTimeStatus;

    bool SaveAs();//另存为

    bool showColon;//用于显示冒号

    bool DisplayFlag;//停止显示

    QTimer *timer;
};

#endif // MAINWINDOW_H
