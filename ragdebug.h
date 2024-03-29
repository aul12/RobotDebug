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
    int sendData(unsigned char addresse, int value);
    int getData(int addresse);
    QString getString(int addresse);
    int getData(int addresse, int faktor);
    QString getString(int addresse, int faktor);
    bool readData();
    QSerialPort * serial;

private:
    int _sendData(QByteArray data);
    QByteArray buffer;
    int werte[256];
    bool connected;



};

#endif // RAGDEBUG_H
