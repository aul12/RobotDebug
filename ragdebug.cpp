#include "ragdebug.h"

RAGDebug::RAGDebug()
{
   connected = false;

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


int RAGDebug::sendData(unsigned char addresse, int value)
{
    QByteArray data;
    data.push_back('|');
    data.push_back(addresse);
    data.push_back(value);
    data.push_back('&');
    return _sendData(data);
}

bool RAGDebug::readData(){
    while(serial->bytesAvailable()){
        QByteArray data = serial->readAll();
        buffer.append(data);
        serial->clear();
        if(buffer.contains('&') && buffer.contains('|') && buffer.length() > buffer.indexOf('|')+3){
            unsigned short begin = buffer.indexOf('|');    //Trennzeichen finden
            unsigned char addresse = (unsigned char)buffer.at(begin+1);
            werte[addresse] = buffer.at(begin+2)<0?buffer.at(begin+2)+256:buffer.at(begin+2);
            buffer.clear();
            return true;
        }
    }
    return false;
}

int RAGDebug::_sendData(QByteArray data)
{
    if(connected){
       serial->write(data);
        return data.length();
    }
    return -1;
}

int RAGDebug::getData(int addresse, int faktor){
    return (werte[addresse] * faktor);
}

int RAGDebug::getData(int addresse){
    return werte[addresse];
}

QString RAGDebug::getString(int addresse)
{
    return QString::number(this->getData(addresse));
}

QString RAGDebug::getString(int addresse, int faktor)
{
    return QString::number(this->getData(addresse, faktor));
}


