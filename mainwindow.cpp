#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    time = new QTimer(this);
    time->stop();
    time->setInterval(ui->timeSpinBox->value());
    connect(time,&QTimer::timeout,this,[=](){if(serialPort->isOpen())on_sendButton_clicked();});
    initPort();
    showColon = true;

    txCount = 0;
    rxCount = 0;

    DisplayTimeStatus = false;
    DisplayFlag = true;

    ui->lcdNumber->setDigitCount(8);//设置能显示的位数
    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);//设置显示外观
    ui->lcdNumber->setStyleSheet("border: 1px solid green; color: green; background: silver;");//设置样式

    timer = new QTimer(this);//新建一个定时器对象
    connect(timer, &QTimer::timeout, this, &MainWindow::showTime);


    //设置默认值
    //ui->BaudBox->setCurrentIndex(7);
    ui->PortBox->setCurrentIndex(1);
    ui->StopBox->setCurrentIndex(0);
    ui->DataBox->setCurrentIndex(3);
    ui->ParityBox->setCurrentIndex(0);
    ui->sendButton->setEnabled(false);
    ui->sHexRadio->setEnabled(false);
    ui->sTextRadio->setChecked(true);
    ui->sTextRadio->setEnabled(false);
    ui->rTextRadio->setChecked(true);
    //connect(ui->reSendCheck, &QCheckBox::stateChanged, this, &MainWindow::timeTosend);
    //connect(time, &QTimer::timeout, this, &MainWindow::on_sendButton_clicked);
    connect(ui->rHexRadio, &QRadioButton::toggled, this, &MainWindow::Mdisplay);

}
void MainWindow::initPort()
{
    //获取可用串口
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos)
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(info.portName());
            qDebug()<<info.portName();
            serial.close();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
//读取数据
void MainWindow::ReadData()
{
    if(ui->showNumCB->isChecked())
    {
        //获取系统当前时间
        QDateTime dateTime = QDateTime::currentDateTime();
        //显示的内容
        QString text = dateTime.toString("yyyy-MM-dd HH:mm:ss");
        ui->textBrowser->insertPlainText(QString("[%0]%1").arg(ui->recvCntLineEdit->text()).arg(text));
    }
    static int reNum = 0;
    QByteArray buf;
    buf = serialPort->readAll();
    if (!buf.isEmpty())
    {
        reNum += buf.size();
        QString myStrTemp = QString::fromLocal8Bit(buf); //支持中文显示
        QString str = ui->textBrowser->toPlainText();
        str += myStrTemp;
        //是否显示数据
        ui->textBrowser->clear();
        if(DisplayFlag == true)
        {
            ui->textBrowser->append(str);
        }
        ui->recvCntLineEdit->setText(tr("%1").arg(reNum));
        rxCount = reNum;show_portStateLabel();
    }
    //自动换行
    if(ui->autoLineCB->isChecked())
    {
        ui->textBrowser->insertPlainText("\n");
    }
    buf.clear();
}
//状态栏显示
void MainWindow::show_portStateLabel()
{
    QString info=QString("RX: %0     TX: %1 Bytes").arg(rxCount).arg(txCount);
    ui->portStateLabel->setText(info);
}
//设置串口参数
void MainWindow::on_openButton_clicked()
{
    if (ui->openButton->text() == tr("打开串口"))
    {
        serialPort = new QSerialPort;
        serialPort->setPortName(ui->PortBox->currentText());
        if(serialPort->open(QIODevice::ReadWrite))
        {
            //设置波特率
            //qDebug()<<ui->BaudBox->currentIndex();
            QString str_baudrae =  ui->BaudBox->currentText();

            serialPort->setBaudRate(str_baudrae.toInt());

            //设置停止位
            switch (ui->StopBox->currentIndex())
            {
            case 1:
                serialPort->setStopBits(QSerialPort::OneStop);
                break;
            case 2:
                serialPort->setStopBits(QSerialPort::TwoStop);
                break;
            default:
                break;
            }
            //设置数据位
            switch (ui->DataBox->currentIndex())
            {
            case 1:
                serialPort->setDataBits(QSerialPort::Data8);
                break;
            case 2:
                serialPort->setDataBits(QSerialPort::Data7);
                break;
            case 3:
                serialPort->setDataBits(QSerialPort::Data6);
                break;
            case 4:
                serialPort->setDataBits(QSerialPort::Data5);
                break;
            default:
                break;
            }
            //设置校验方式
            switch (ui->ParityBox->currentIndex())
            {
            case 0:
                serialPort->setParity(QSerialPort::NoParity);
                break;
            default:
                break;
            }
            ui->openButton->setText(tr("关闭串口"));
            ui->PortBox->setEnabled(false);
            ui->BaudBox->setEnabled(false);
            ui->StopBox->setEnabled(false);
            ui->DataBox->setEnabled(false);
            ui->ParityBox->setEnabled(false);
            ui->sendButton->setEnabled(true);
            ui->sTextRadio->setEnabled(true);
            ui->sHexRadio->setEnabled(true);
//            connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
            ui->comLabel->setText(QString("串口号：%0   波特率：%1").arg(ui->PortBox->currentText()).arg(ui->BaudBox->currentText()));

            //timer->start(1000);

        }
        else
        {
            QMessageBox::critical(this, tr("Error"), serialPort->errorString());
        }
    }
    else
    {
        serialPort->clear();
        serialPort->close();
        serialPort->deleteLater();
        ui->sendButton->setEnabled(false);
        ui->openButton->setText(tr("打开串口"));
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->DataBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->sHexRadio->setEnabled(false);
        ui->sTextRadio->setEnabled(false);
        ui->comLabel->setText(tr("串口已关闭！"));
    }
}
//清除接收
void MainWindow::on_r_clearButton_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_sendButton_clicked()
{
    QString str = ui->lineEdit->text();
    if(!str.isEmpty())
    {
        auto isHexSend = ui->sHexRadio->isChecked();
        int len = str.length();
        txCount+=len;show_portStateLabel();
        ui->sendCntLineEdit->setText(tr("%1").arg(txCount));
        if(len%2 == 1)
        {
            str = str.insert(len-1,'0');
        }
        QByteArray senddata;
        if(isHexSend)
        {
            StringToHex(str,senddata);
            if(serialPort->write(senddata)<0)
            {
                QMessageBox::critical(this, tr("Error"), serialPort->errorString());
            }
        }
        else
        {
            if(serialPort->write(ui->lineEdit->text().toLocal8Bit())<0)
            {
                QMessageBox::critical(this, tr("Error"), serialPort->errorString());
            }
        }
        //接收区显示发送区的内容
        auto isredisplay = ui->reDisplay->isChecked();
        if(isredisplay)
        {
            ui->textBrowser->append(str);
        }
    }
}
void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChart(hstr);
        lowhexdata = ConvertHexChart(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

char MainWindow::ConvertHexChart(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    // 0x30 对应 ‘0’
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    //        else return (-1);
    else return ch-ch;
    //不在0-f范围内的会发送成0
}
void MainWindow::readToHex()
{
    QByteArray temp = serialPort->readAll();
    QDataStream out(&temp,QIODevice::ReadWrite);
    //将字节数组读入
    while(!out.atEnd())
    {
        qint8 outChar = 0;
        out>>outChar; //每字节填充一次，直到结束
        //十六进制的转换
        QString str = QString("%1").arg(outChar&0xFF,2,16,QLatin1Char('0'));
        ui->textBrowser->insertPlainText(str.toUpper());//大写
        ui->textBrowser->insertPlainText(" ");//每发送两个字符后添加一个空格
        ui->textBrowser->moveCursor(QTextCursor::End);
    }
}

void MainWindow::Mdisplay()
{    if(ui->rHexRadio->isChecked())
    {
        disconnect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readToHex);
    }
    else
    {
        connect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);
        disconnect(serialPort, &QSerialPort::readyRead, this, &MainWindow::readToHex);
    }
}
//清除发送
void MainWindow::on_s_clearButton_clicked()
{
    ui->lineEdit->clear();
}
//清空计数
void MainWindow::on_resetCntButton_clicked()
{
    ui->sendCntLineEdit->clear();
    ui->recvCntLineEdit->clear();
}
//自动发送
void MainWindow::on_autoSendCB_clicked(bool checked)
{
    if(checked)
    {
        time->setInterval(ui->timeSpinBox->value());
        time->start();
        ui->timeSpinBox->setReadOnly(true);
    }
    else {
        time->stop();
        ui->timeSpinBox->setReadOnly(false);
    }
}
//显示时间
void MainWindow::on_showTimeCB_stateChanged(int arg1)
{
    if(ui->showTimeCB->isChecked())
    {
        DisplayTimeStatus = true;//显示时间
    }
    else {
        DisplayTimeStatus = false;
    }
}
//保存数据
void MainWindow::on_saveButton_clicked()
{
        SaveAs();
}
bool MainWindow::SaveAs()
{
    QString curFile;
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),curFile,tr("TXT文档(*.txt);;所有文件(*.*)"));
    if(fileName.isEmpty())
    {
        return false;
    }
    if(!(fileName.endsWith(".txt",Qt::CaseInsensitive)))
    {
        fileName += ".txt";
    }
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::critical(this,tr("critical"),tr("无法写入数据！"),QMessageBox::Yes);
        return false;
    }
    else
    {
        QTextStream out(&file);
        out<<ui->textBrowser->toPlainText();
        return true;
    }
}
//显示当前时间
void MainWindow::showTime()
{
    //获取系统当前时间
    QTime time = QTime::currentTime();
    //显示的内容
    QString text = time.toString("hh:mm:ss");
    if(showColon)
    {
        text[2] = ':';
        text[5] = ':';
        showColon = false;
    }
    else {
        text[2] = ' ';
        text[5] = ' ';
        showColon = true;
    }
    ui->lcdNumber->display(text);

    ReadData();
}
