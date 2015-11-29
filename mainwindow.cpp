#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>

#include "ragdebug.h"
#include "idPresets.hpp"

RAGDebug debug;

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define ABSTAND_RAND 20
#define ROBOTER_RADIUS 10



int x_pos=100;
int y_pos=100;
int ball_winkel=30;
int ball_abstand=30;
int roboter_drehung=-70;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionZeige_Konsole->setChecked(true);
    ui->actionZeige_USB_Optionen->setChecked(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateUI()));
    timer->start(40);
}

MainWindow::~MainWindow()
{
    delete ui;
    debug.serial->close();
}


void MainWindow::updateUI(){
    this->updateLabels();
    this->update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    float faktor = ui->tabWidget->x()/293.0;
    if(ui->frameConsole->isVisible()){
        if(ui->frameConsole->y() /182.0 < faktor)
            faktor = ui->frameConsole->y() /182.0;
    }
    else if(ui->frameUSB->isVisible()){
        if(ui->frameUSB->y() /182.0 < faktor)
            faktor = ui->frameUSB->y() /182.0;
    }

    QPainter painter(this);
    painter.fillRect(ABSTAND_RAND,ABSTAND_RAND,243*faktor,182*faktor, Qt::darkGreen);                           //Feld
    QPen linie(Qt::white, 2*faktor, Qt::SolidLine);                                                             //Linienstift
    painter.setPen(linie);
    painter.drawRect(ABSTAND_RAND+(30*faktor), ABSTAND_RAND+(30*faktor), 183*faktor, 122*faktor);               //Linie
    painter.fillRect((22.6*faktor)+ABSTAND_RAND, (61*faktor)+ABSTAND_RAND, 7.4*faktor, 60*faktor, Qt::yellow);  //Linkes Tor (Gelb)
    painter.fillRect((183+30)*faktor+ABSTAND_RAND,(61*faktor)+ABSTAND_RAND, 7.4*faktor, 60*faktor, Qt::blue);   //Rechtes Tor (Blau)

    painter.setBrush(Qt::black);
    painter.setPen(Qt::black);
    painter.drawEllipse((x_pos-ROBOTER_RADIUS)*faktor, (y_pos-ROBOTER_RADIUS)*faktor, ROBOTER_RADIUS*faktor*2, ROBOTER_RADIUS*faktor*2);    //Roboter zeichnen
    painter.setPen(Qt::white);
    painter.drawLine(x_pos*faktor,y_pos*faktor,sin((-roboter_drehung+90)*DEG_TO_RAD)*ROBOTER_RADIUS*faktor+x_pos*faktor,cos((-roboter_drehung+90)*DEG_TO_RAD)*ROBOTER_RADIUS*faktor+y_pos*faktor);  //Richtungslinie

    painter.setBrush(Qt::darkRed);
    painter.setPen(Qt::black);
    painter.drawEllipse((x_pos+sin((-ball_winkel+90)*DEG_TO_RAD)*ball_abstand-3.7)*faktor,(y_pos+cos((-ball_winkel+90)*DEG_TO_RAD)*ball_abstand-3.7)*faktor, 7.4*faktor,7.4*faktor);    //Ball
}

void MainWindow::updateLabels(){
    ui->labelADCA0->setText("ADC0: " + QString::number(debug.getData(ADCA0)));
    ui->labelADCA1->setText("ADC1: " + QString::number(debug.getData(ADCA1)));
    ui->labelADCA2->setText("ADC2: " + QString::number(debug.getData(ADCA2)));
    ui->labelADCA3->setText("ADC3 " + QString::number(debug.getData(ADCA3)));
    ui->labelADCA4->setText("ADC4: " + QString::number(debug.getData(ADCA4)));
    ui->labelADCA5->setText("ADC5: " + QString::number(debug.getData(ADCA5)));
    ui->labelADCA6->setText("ADC6: " + QString::number(debug.getData(ADCA6)));
    ui->labelADCA7->setText("ADC7: " + QString::number(debug.getData(ADCA7)));

    ui->labelADCB0->setText("ADC0: " + QString::number(debug.getData(ADCB0)));
    ui->labelADCB1->setText("ADC1: " + QString::number(debug.getData(ADCB1)));
    ui->labelADCB2->setText("ADC2: " + QString::number(debug.getData(ADCB2)));
    ui->labelADCB3->setText("ADC3 " + QString::number(debug.getData(ADCB3)));
    ui->labelADCB4->setText("ADC4: " + QString::number(debug.getData(ADCB4)));
    ui->labelADCB5->setText("ADC5: " + QString::number(debug.getData(ADCB5)));
    ui->labelADCB6->setText("ADC6: " + QString::number(debug.getData(ADCB6)));
    ui->labelADCB7->setText("ADC7: " + QString::number(debug.getData(ADCB7)));

    ui->labelI2C_CMPS->setText("CMPS: "+QString::number(debug.getData(CMPS)));
    ui->labelI2C_MPU->setText("MPU: "+QString::number(debug.getData(MPU)));
    ui->labelI2C_BNO->setText("BNO: "+QString::number(debug.getData(BNO055)));
    ui->labelI2C_US0->setText("US0: "+QString::number(debug.getData(US0)));
    ui->labelI2C_US1->setText("US1: "+QString::number(debug.getData(US1)));
    ui->labelI2C_US2->setText("US2: "+QString::number(debug.getData(US2)));
    ui->labelI2C_US3->setText("US3: "+QString::number(debug.getData(US3)));

    ui->labelPixieStatus->setText("Status: "+QString::number(debug.getData(PIXIE_ERROR)));
    ui->labelPixieX1->setText("X: "+QString::number(debug.getData(PIXIEX1)));
    ui->labelPixieY1->setText("Y: "+QString::number(debug.getData(PIXIEY1)));
    ui->labelPixieH1->setText("Height: "+QString::number(debug.getData(PIXIEH1)));
    ui->labelPixieW1->setText("Width: "+QString::number(debug.getData(PIXIEW1)));

    ui->labelBerechnetBallwinkel->setText("Ballwinkel: "+QString::number(debug.getData(BALLWINKEL)));
    ui->labelBerechnetFahrtwinkel->setText("Fahrtrichtung: "+QString::number(debug.getData(FAHRTRICHTUNG)));
    ui->labelBerechnetPosX->setText("Position X: "+QString::number(debug.getData(POSX)));
    ui->labelBerechnetPosY->setText("Position Y: "+QString::number(debug.getData(POSY)));
    ui->labelBerechnetSollPhi->setText("Soll-Phi: "+QString::number(debug.getData(SOLLPHI)));
    ui->labelBerechnetDeltaPhi->setText("Delta-Phi: "+QString::number(debug.getData(DELTAPHI)));
    ui->labelBerechnetProgramm->setText("Spielprogramm: "+QString::number(debug.getData(SPIELPROGRAMM)));
    ui->labelBerechnetKameraWinkel->setText("Kamerawinkel: "+QString::number(debug.getData(KAMERA_WINKEL)));

    ui->labelDigitalSchalterMotor->setText("Schalter Motor: "+QString::number(debug.getData(MOTORSCHALTER)));
    ui->labelDigitalSchalterDisplay->setText("Schalter Display: "+QString::number(debug.getData(DISPLAYSCHALTER)));
    ui->labelDigitalSchalterLinie->setText("Schalter Linie: "+QString::number(debug.getData(LINIENSCHALTER)));
    ui->labelDigitalSchalterProgrammLinks->setText("Schalter Programm Links: "+QString::number(debug.getData(PROGRAMMLINKS)));
    ui->labelDigitalSchalterProgrammRechts->setText("Schalter Programm Rechts: "+QString::number(debug.getData(PROGRAMMRECHTS)));
    ui->labelDigitalSchalterEEPROM->setText("Taster EEPROM: "+QString::number(debug.getData(EEPROMTASTER)));
    ui->labelDigitalLichtschranke->setText("Lichtschranke: "+QString::number(debug.getData(LICHTSCHRANKE)));

    ui->labelMotor0->setText("Motor 0: "+QString::number(debug.getData(MOTO0)));
    ui->labelMotor1->setText("Motor 1: "+QString::number(debug.getData(MOTO1)));
    ui->labelMotor2->setText("Motor 2: "+QString::number(debug.getData(MOTO2)));
    ui->labelMotor3->setText("Motor 3: "+QString::number(debug.getData(MOTO3)));
    ui->labelMotorDribbler->setText("Dribbler: "+QString::number(debug.getData(DRIBBLER)));
    ui->labelMotorSchuss->setText("Schuss: "+QString::number(debug.getData(SCHUSS)));
    ui->labelMotorOutStatus->setText("Out-Status: "+QString::number(debug.getData(OUTSTATUS)));
    ui->labelMotorOutWinkel->setText("Out-Winkel: "+QString::number(debug.getData(OUTWINKEL)));

}

void MainWindow::readData(){
    if(debug.readData()){
        y_pos = debug.getData(POSX);
        x_pos = debug.getData(POSY);
        roboter_drehung = debug.getData(DELTAPHI);
    }
}


void MainWindow::on_actionZeige_Konsole_toggled(bool arg1)
{
    ui->frameConsole->setVisible(arg1);
}

void MainWindow::on_actionZeige_USB_Optionen_toggled(bool arg1)
{
    ui->frameUSB->setVisible(arg1);
}



void MainWindow::on_actionRefresh_triggered()
{
    this->updateLabels();
    this->update();
}

void MainWindow::on_pushConnect_clicked()
{
    bool result = debug.init(ui->lineEditBaud->text().toInt(), ui->lineEditPort->text());
    if(result)
        ui->textBrowser->append("Verbunden");
    else
        ui->textBrowser->append("Fehler beim verbinden");
    QObject::connect(debug.serial, SIGNAL(readyRead()), this, SLOT(readData()) );
}

void MainWindow::on_pushButtonSerialSend_clicked()
{
    int addr = ui->lineEditSerialSendAddress->text().toInt();
    int data = ui->lineEditSerialSendData->text().toInt();
    int len = debug.sendData(addr, data);
    if(len == -1)
        ui->textBrowser->append("Fehler beim senden, ist das Programm verbunden?");
    else
        ui->textBrowser->append("Gesendet: "+QString::number(len)+" bytes");
}
