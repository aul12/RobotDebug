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
    int sendData(unsigned char addresse, int64_t value);
    int sendData(unsigned char addresse, int32_t value);
    int sendData(unsigned char addresse, int16_t value);
    int sendData(unsigned char addresse, int8_t value);
    int sendData(unsigned char addresse, uint64_t value);
    int sendData(unsigned char addresse, uint32_t value);
    int sendData(unsigned char addresse, uint16_t value);
    int sendData(unsigned char addresse, uint8_t value);
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
