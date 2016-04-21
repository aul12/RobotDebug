#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    #ifdef _WIN32
        ui->lineEditPort->setText("COM1");
    #endif

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
    if(ui->groupBoxConsole->isVisible()){
        if(ui->groupBoxConsole->y() /182.0 < faktor)
            faktor = ui->groupBoxConsole->y() /182.0;
    }
    else if(ui->groupBoxUSB->isVisible()){
        if(ui->groupBoxUSB->y() /182.0 < faktor)
            faktor = ui->groupBoxUSB->y() /182.0;
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
    ui->labelADCA0->setText("ADC0: " + debug.getString(ADCA0, 16));
    ui->labelADCA1->setText("ADC1: " + debug.getString(ADCA1, 16));
    ui->labelADCA2->setText("ADC2: " + debug.getString(ADCA2, 16));
    ui->labelADCA3->setText("ADC3: " + debug.getString(ADCA3, 16));
    ui->labelADCA4->setText("ADC4: " + debug.getString(ADCA4, 16));
    ui->labelADCA5->setText("ADC5: " + debug.getString(ADCA5, 16));
    ui->labelADCA6->setText("ADC6: " + debug.getString(ADCA6, 16));
    ui->labelADCA7->setText("ADC7: " + debug.getString(ADCA7, 16));

    ui->labelI2C_CMPS->setText("BNO Grad: "+debug.getString(BNO055_GRAD, 2));
    ui->labelI2C_MPU->setText("BNO Rotation: "+debug.getString(BNO055_ROT, 256));
    ui->labelI2C_BNO->setText("");
    ui->labelI2C_US0->setText("US0: "+debug.getString(US0));
    ui->labelI2C_US1->setText("US1: "+debug.getString(US1));
    ui->labelI2C_US2->setText("US2: "+debug.getString(US2));

    ui->labelPixieStatus->setText("Status: "+debug.getString(PIXIE_ERROR));
    ui->labelPixieX1->setText("X: "+debug.getString(PIXIEX1));
    ui->labelPixieY1->setText("Y: "+debug.getString(PIXIEY1));
    ui->labelPixieH1->setText("Height: "+debug.getString(PIXIEH1));
    ui->labelPixieW1->setText("Width: "+debug.getString(PIXIEW1));

    ui->labelBerechnetBallwinkel->setText("Ballwinkel: "+debug.getString(BALLWINKEL,2));
    ui->labelBerechnetPosX->setText("Position X: "+debug.getString(POSX));
    ui->labelBerechnetPosY->setText("Position Y: "+debug.getString(POSY));
    ui->labelBerechnetSollPhi->setText("Soll-Phi: "+debug.getString(SOLLPHI, 2));
    ui->labelBerechnetDeltaPhi->setText("Delta-Phi: "+debug.getString(DELTAPHI, 2));
    ui->labelBerechnetProgramm->setText("Spielprogramm: "+debug.getString(SPIELPROGRAMM));
    ui->labelBerechnetKameraWinkel->setText("Kamerawinkel: "+debug.getString(KAMERA_WINKEL));
    ui->labelBerechnetBallDistanz->setText("Balldistanz: "+debug.getString(BALLDISTANZ, 24));
    ui->labelBerechnetBallda->setText("Ballda: " + debug.getString(BALLDA));

    ui->labelDigitalSchalterMotor->setText("Schalter Motor: "+debug.getString(MOTORSCHALTER));
    ui->labelDigitalSchalterDisplay->setText("Schalter Display: "+debug.getString(DISPLAYSCHALTER));
    ui->labelDigitalSchalterLinie->setText("Schalter Linie: "+debug.getString(LINIENSCHALTER));
    ui->labelDigitalSchalterProgrammLinks->setText("Schalter Programm Links: "+debug.getString(PROGRAMMLINKS));
    ui->labelDigitalSchalterProgrammRechts->setText("Schalter Programm Rechts: "+debug.getString(PROGRAMMRECHTS));
    ui->labelDigitalSchalterEEPROM->setText("Taster EEPROM: "+debug.getString(EEPROMTASTER));
    ui->labelDigitalLichtschranke->setText("Lichtschranke: "+debug.getString(LICHTSCHRANKE));
    ui->labelDigitalSchalterAktiv->setText("Schalter Aktiv/Passiv: " + debug.getString(AKTIVSCHALTER));

    ui->labelMotor0->setText("Motor 0: "+debug.getString(MOTO0, 8));
    ui->labelMotor1->setText("Motor 1: "+debug.getString(MOTO1, 8));
    ui->labelMotor2->setText("Motor 2: "+debug.getString(MOTO2, 8));
    ui->labelMotor3->setText("Motor 3: "+debug.getString(MOTO3, 8));
    ui->labelMotorDribbler->setText("Dribbler: "+debug.getString(DRIBBLER));
    ui->labelMotorSchuss->setText("Schuss: "+debug.getString(SCHUSS));
    ui->labelMotorOutStatus->setText("Out-Status: "+debug.getString(OUTSTATUS));
    ui->labelMotorOutWinkel->setText("Out-Winkel: "+debug.getString(OUTWINKEL, 2));

}

void MainWindow::readData(){
    if(debug.readData()){
        y_pos = debug.getData(POSX);
        x_pos = debug.getData(POSY);
        roboter_drehung = debug.getData(DELTAPHI)*2;
        ball_winkel = debug.getData(BALLWINKEL)*2;
        ball_abstand = (256 - debug.getData(BALLDISTANZ));
    }
}


void MainWindow::on_actionZeige_Konsole_toggled(bool arg1)
{
    ui->groupBoxConsole->setVisible(arg1);
}

void MainWindow::on_actionZeige_USB_Optionen_toggled(bool arg1)
{
    ui->groupBoxUSB->setVisible(arg1);
}

void MainWindow::on_actionZeige_Daten_toggled(bool arg1)
{
    ui->tabWidget->setVisible(arg1);
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
        ui->textBrowser->append("Fehler beim Verbinden");
    QObject::connect(debug.serial, SIGNAL(readyRead()), this, SLOT(readData()) );
}

void MainWindow::on_pushButtonSerialSend_clicked()
{
    if(ui->lineEditSerialSendAddress->text() == "")
         ui->textBrowser->append("Bitte eine gültige Adresse eingeben!");
    else{
        int addr = ui->lineEditSerialSendAddress->text().toInt();
        if(ui->lineEditSerialSendData->text() == ""){

            ui->textBrowser->append(debug.getString(addr));
        }
        else{
            int data = ui->lineEditSerialSendData->text().toInt();
            int len = debug.sendData(addr, (uint8_t)data);
            if(len == -1)
                ui->textBrowser->append("Fehler beim senden, ist das Programm verbunden?");
            else
                ui->textBrowser->append("Gesendet: "+QString::number(len)+" bytes");
        }

    }

}
