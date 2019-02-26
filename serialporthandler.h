#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

//#include <globals.h>

#include <QObject>
#include <QString>
#include <QSerialPort>
#include <QTimer>
#include <QList>
#include <QQueue>

#include <QDebug>
#include <QDateTime>

class SerialPortHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortHandler(QObject *parent = nullptr);
    ~SerialPortHandler();

    QString getPortName();
    int getBaudRate();
    bool isOpen();
    QString errorString();
    int getTimeout();
    void setTimeout(int value = 50);
    bool isBusy();
    void clearBuffer();
    bool queueIsEmpty();
    quint8 getAddress();
    quint16 getRegister();
    quint8 getCount();
    QList<quint16> &getValues();
    int bytesToSend;

    enum {READ_COMMAND = 0x03, WRITE_COMMAND = 0x06} modBusCommands;
    enum {ADDRESS_SHIFT = 0, FUNCTION_CODE_SHIFT = 1, REG_HIGH_SHIFT = 2, REG_LOW_SHIFT = 3, READ_COUNT_SHIFT = 2, WRITE_HIGH_DATA_SHIFT = 4, WRITE_LOW_DATA_SHIFT = 5, READ_HIGH_DATA_SHIFT = 3, READ_LOW_DATA_SHIFT = 4} modBusShift;

private:
    typedef struct {
        bool isRead;
        quint16 reg;
        quint16 val;
    } reply_t;
    QSerialPort* serialPort;
    QQueue<reply_t> queue;
    QByteArray buffer;
    quint16 crc16(const QByteArray &ptr, int size);
    QTimer* errorTimer;
    int expectedLength;
    quint8 addr;
    quint8 length;
    quint16 firstRegister;
    quint8 funcCode;
    QString errorMsg;
    QString sentData;
    int timeout_period;
    QList<quint16> values;
    void startTransmit(QByteArray);
    bool isTimeout;
    bool busyFlag;
    quint8 address;
    void prepareWrite();
    void prepareRead();

signals:
    void newDataIsReady(bool);
    void stateChanged(bool);
    void errorOccuredSignal();
    void timeoutSignal(quint8 address);

public slots:
    void setPort(QString);
    void setBaud(int);
    void setOpenState(bool);
    void setAddress(quint8);
    void dataToWrite(quint16 startRegister, quint16 value);
    void dataToRead(quint16 startRegister, quint8 count);
    void clearQueue();

private slots:
    void readyRead();
    void errorOccuredSlot(QSerialPort::SerialPortError error);
    void dataIsWritten(qint64);
    void timeOut();
};

#endif // SERIALPORTHANDLER_H
