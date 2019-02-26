#include "serialporthandler.h"
#include "crctable.h"

SerialPortHandler::SerialPortHandler(QObject *parent) : QObject(parent),
    serialPort(nullptr)
{
    serialPort = new QSerialPort();
    serialPort->setStopBits(QSerialPort::TwoStop);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);

    timeout_period = 50;
    values.clear();
    buffer.clear();
    queue.clear();
    busyFlag = false;

    errorTimer = new QTimer();
    errorTimer->setSingleShot(true);
    errorTimer->setInterval(timeout_period);
    isTimeout = false;
    connect(errorTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
    connect(serialPort, SIGNAL(bytesWritten(qint64)), this, SLOT(dataIsWritten(qint64)));
    connect(serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

SerialPortHandler::~SerialPortHandler() {
    serialPort->deleteLater();
}

bool SerialPortHandler::isOpen() {
    return serialPort->isOpen();
}

QString SerialPortHandler::errorString() {
    return errorMsg;
}

QString SerialPortHandler::getPortName() {
    return serialPort->portName();
}

int SerialPortHandler::getBaudRate() {
    return serialPort->baudRate();
}

void SerialPortHandler::setPort(QString port) {
    serialPort->setPortName(port);
}

void SerialPortHandler::setBaud(int baud) {
    serialPort->setBaudRate(baud);
}

void SerialPortHandler::readyRead() {
    errorTimer->stop();
    errorTimer->setInterval(timeout_period);

    buffer.append(serialPort->readAll());

    if(buffer.size() == expectedLength) {
        quint16 inCRC;
        inCRC = (static_cast<quint8>(buffer.at(expectedLength-2))<<8)|(static_cast<quint8>(buffer.at(expectedLength-1)));
        if(inCRC == crc16(buffer, buffer.size()-2)) {
            buffer = buffer.remove(expectedLength-2, 2);

            values.clear();
            funcCode = static_cast<quint8>(buffer.at(FUNCTION_CODE_SHIFT));
            addr = static_cast<quint8>(buffer.at(ADDRESS_SHIFT));

            switch(funcCode) {
            case READ_COMMAND:
                length = static_cast<quint8>(buffer.at(READ_COUNT_SHIFT))/2;
                for(int i =0; i < length; i++) {
                    quint16 res = (static_cast<quint8>(buffer.at(READ_HIGH_DATA_SHIFT+2*i))<<8) | static_cast<quint8>(buffer.at(READ_LOW_DATA_SHIFT+2*i));
                    values.append(res);
                }
                break;
            case WRITE_COMMAND:
                firstRegister = (buffer.at(REG_HIGH_SHIFT)<<8) | (buffer.at(REG_LOW_SHIFT));
                for (int i = 0; i < length; i++)
                    values.append((static_cast<quint8>(buffer.at(WRITE_HIGH_DATA_SHIFT+2*i))<<8) | static_cast<quint8>(buffer.at(WRITE_LOW_DATA_SHIFT+2*i)));
                break;
            default:
                errorMsg = "Неверный функциональный код. Буффер: " + QString(buffer);
                emit errorOccuredSignal();
                return;
            }

            emit newDataIsReady(queueIsEmpty());
//            qDebug() << buffer.toHex();
        } else {
            errorMsg = "Контрольная сумма не сошлась! Буффер: " + QString(buffer);
            emit errorOccuredSignal();
        }
        buffer.clear();
    } else {
        errorTimer->stop();
        errorTimer->setInterval(timeout_period);
        errorTimer->start();
    }
}

QList<quint16>& SerialPortHandler::getValues() {
    return values;
}

void SerialPortHandler::setOpenState(bool state) {
    if(state == serialPort->isOpen()) return;

    if(serialPort->isOpen()) {
        serialPort->close();
        clearQueue();
        emit stateChanged(serialPort->isOpen());
    } else {
        if(serialPort->open(QIODevice::ReadWrite)) {
            emit stateChanged(serialPort->isOpen());
        }
    }
}

void SerialPortHandler::errorOccuredSlot(QSerialPort::SerialPortError error) {
    if(error != QSerialPort::NoError) {
        errorMsg = serialPort->errorString();
		clearQueue();
        emit errorOccuredSignal();
        serialPort->clearError();
    }
}

void SerialPortHandler::dataIsWritten(qint64 sizeWritten) {
    bytesToSend -= sizeWritten;
    errorTimer->stop();
    errorTimer->setInterval(timeout_period);
    errorTimer->start();
}

void SerialPortHandler::setAddress(quint8 addr) {
    address = addr;
}

void SerialPortHandler::startTransmit(QByteArray str) {
    // передача в порт
    busyFlag = true;
    bytesToSend = str.size();
    sentData = str;
    bytesToSend -= serialPort->write(str, bytesToSend);
    errorTimer->stop();
    errorTimer->setInterval(timeout_period);
    errorTimer->start();
}

void SerialPortHandler::prepareWrite() {
    reply_t qData = queue.dequeue();

    buffer.clear();
    QByteArray msg;
    addr = address;
    funcCode = WRITE_COMMAND;
    length = 1;

    msg.append(address);
    msg.append(funcCode);
    msg.append(qData.reg>>8);
    msg.append(qData.reg);
    msg.append(qData.val>>8);
    msg.append(qData.val);
    quint16 crc = crc16(msg, msg.size());
    msg.append(static_cast<quint8> (crc>>8));
    msg.append(static_cast<quint8> (crc));

    // for write command expected length is equal to written length
    expectedLength = msg.size();
    isTimeout = false;
    startTransmit(msg);
}

void SerialPortHandler::dataToWrite(quint16 startRegister, quint16 value) {
    if(!serialPort->isOpen()) return;

    reply_t reqData;
    reqData.isRead = false;
    reqData.reg = startRegister;
    reqData.val = value;

    queue.prepend(reqData);

    if(busyFlag) return;

    prepareWrite();
}

void SerialPortHandler::dataToRead(quint16 startRegister, quint8 count) {
    if(!serialPort->isOpen()) return;

    reply_t reqData;
    reqData.isRead = true;
    reqData.reg = startRegister;
    reqData.val = count;

    queue.append(reqData);

    if(busyFlag) return;

    prepareRead();
}

void SerialPortHandler::prepareRead() {
    reply_t qData = queue.dequeue();

    buffer.clear();
    QByteArray msg;
    addr = address;
    funcCode = READ_COMMAND;
    firstRegister = qData.reg;
    length = qData.val;

    msg.append(address);
    msg.append(funcCode);
    msg.append(static_cast<quint8> (qData.reg>>8));
    msg.append(static_cast<quint8> (qData.reg));
    msg.append(static_cast<quint8> (qData.val>>8));
    msg.append(static_cast<quint8> (qData.val));
    quint16 crc = crc16(msg, msg.size());
    msg.append(static_cast<quint8> (crc>>8));
    msg.append(static_cast<quint8> (crc));

    // for write command expected length is equal to written length
    expectedLength = 2*qData.val+2+3;
    isTimeout = false;
    startTransmit(msg);
}

quint16 SerialPortHandler::crc16(const QByteArray &ptr, int size) {
    uchar   ucCRCHi = 0xFF;
    uchar   ucCRCLo = 0xFF;
    int idx;

        for(int i =0; i < size; i++)
        {
            uchar currByte = ptr.at(i);
            idx = ucCRCLo ^ currByte;
            ucCRCLo = (uchar) ( ucCRCHi ^ aucCRCHi[idx] );
            ucCRCHi = aucCRCLo[idx];
        }

        return quint16 ( ucCRCLo << 8 | ucCRCHi );
}



void SerialPortHandler::timeOut() {
    isTimeout = true;
    clearQueue();
    errorMsg = "Таймаут. Буффер: " + QString(sentData);
    emit timeoutSignal(addr);
}

int SerialPortHandler::getTimeout() {
    return timeout_period;
}

void SerialPortHandler::setTimeout(int value) {
    timeout_period = value;
}

void SerialPortHandler::clearQueue() {
    queue.clear();
    busyFlag = false;
}

bool SerialPortHandler::isBusy() {
    return busyFlag;
}

void SerialPortHandler::clearBuffer() {
    busyFlag = false;

    if(!queueIsEmpty()) {
        if(queue.head().isRead) {
            prepareRead();
        } else {
            prepareWrite();
        }
    }
}

bool SerialPortHandler::queueIsEmpty() {
    return queue.isEmpty();
}

quint8 SerialPortHandler::getAddress() {
    return addr;
}

quint16 SerialPortHandler::getRegister() {
    return firstRegister;
}

quint8 SerialPortHandler::getCount() {
    return length;
}
