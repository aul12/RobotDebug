#include "ragdebug.h"

RAGDebug::RAGDebug()
{


}

bool RAGDebug::init(qint32 baud, QString port){
     serial = new QSerialPort(port);
     if(connected)
         return false;
     if(serial->open(QIODevice::ReadWrite))
     {
         serial->setBaudRate(baud);
         serial->setDataBits(QSerialPort::Data8);
         serial->setParity(QSerialPort::NoParity);
         serial->setStopBits(QSerialPort::OneStop);
         serial->setFlowControl(QSerialPort::NoFlowControl);
         connected = true;
         return true;
     }
     else
     {
         serial->close();
         connected = false;
         return false;
     }
}

bool RAGDebug::readData(){
    while(serial->bytesAvailable()){
        QByteArray data = serial->readAll();
        buffer.append(data);
        serial->clear();

        if(buffer.contains('&')){

            unsigned short begin = buffer.indexOf('|');    //Trennzeichen finden
            unsigned char addresse = (unsigned char)buffer.at(begin+1);
            unsigned char lange = (unsigned char)buffer.at(begin+2);
            unsigned long tmp = 0;
            for(int counter=0; counter<lange; counter++){
                tmp = (tmp<<8) | (unsigned char)buffer.at(begin+3+counter);
            }
            int wert = (int) tmp;
            werte[addresse] = wert;
            buffer.clear();
            return true;
        }
    }
    return false;
}

int RAGDebug::sendData(unsigned char addresse, int wert){
    QByteArray data;
    data.push_back('|');
    data.push_back(addresse);
    data.push_back(sizeof(wert));
    for(int counter=sizeof(wert)-1; counter>=0; counter--)
        data.push_back((0xFF<<(counter*8) & wert)>>(counter*8));
    data.push_back('&');

    if(connected){
        serial->write(data);
        return data.length();
    }
    return -1;
}

int RAGDebug::getData(int addresse){
    if(werte[addresse]==NULL)
        return 0;
    return werte[addresse];
}


