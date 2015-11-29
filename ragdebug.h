#ifndef RAGDEBUG_H
#define RAGDEBUG_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort>

#include <QObject>
#include <QDebug>

class RAGDebug : public QObject
{
public:
    RAGDebug();
    bool init(qint32 baud, QString port);
    int sendData(unsigned char addresse, int data);
    int getData(int addresse);
    QString getString(int addresse);
    bool readData();
    QSerialPort * serial;

private:

    QByteArray buffer;
    int werte[256];
    bool connected = false;



};

#endif // RAGDEBUG_H
