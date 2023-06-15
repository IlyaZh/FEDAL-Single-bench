#include "serialporthandler.h"

#include "crctable.h"
#include "models/data_thread.h"

SerialPortHandler::SerialPortHandler(QObject* parent)
    : QObject(parent), dataThread(new models::DataThread) {
  timeout_period = 50;
  values.clear();
  buffer.clear();
  queue.clear();

  connect(dataThread, &models::DataThread::signal_timeout, this,
          &SerialPortHandler::slot_timeOut);
  connect(dataThread, &models::DataThread::signal_errorOccured, this,
          [this]() { clearBuffer(); });
  connect(dataThread, &models::DataThread::signal_receivedData, this,
          &SerialPortHandler::slot_readyRead);
  connect(dataThread, &models::DataThread::signal_stateChanged, this,
          &SerialPortHandler::signal_stateChanged);
}

SerialPortHandler::~SerialPortHandler() { dataThread->deleteLater(); }

bool SerialPortHandler::isOpen() { return dataThread->isOpen(); }

void SerialPortHandler::slot_readyRead(const QByteArray& received_data) {
  buffer.append(received_data);
  qDebug() << "slot_readyRead" << buffer.toHex(' ');
  quint16 expectedLength = 6;
  if (buffer.size() >= expectedLength) {
    switch (static_cast<quint8>(buffer.at(1))) {
      case WRITE_COMMAND:
        expectedLength += 2;
        break;
      case READ_COMMAND:
        expectedLength += static_cast<quint16>(buffer.at(2)) - 1;
        break;
    }
  }
  qDebug() << "buffer.size()=" << buffer.size()
           << " expectedLength=" << expectedLength
           << " code=" << static_cast<quint8>(buffer.at(1));
  if (buffer.size() == expectedLength) {
    //    quint16 inCRC = (static_cast<quint8>(buffer.at(expectedLength - 2)) <<
    //    8) |
    //                    (static_cast<quint8>(buffer.at(expectedLength - 1)));
    if (crc16(buffer, buffer.size()) == 0) {
      buffer = buffer.remove(expectedLength - 2, 2);

      values.clear();
      funcCode = static_cast<quint8>(buffer.at(FUNCTION_CODE_SHIFT));
      //      addr = static_cast<quint8>(buffer.at(ADDRESS_SHIFT));

      switch (funcCode) {
        case READ_COMMAND:
          qDebug() << "read_command";
          length = static_cast<quint8>(buffer.at(READ_COUNT_SHIFT)) / 2;
          for (int i = 0; i < length; i++) {
            quint16 res =
                (static_cast<quint8>(buffer.at(READ_HIGH_DATA_SHIFT + 2 * i))
                 << 8) |
                static_cast<quint8>(buffer.at(READ_LOW_DATA_SHIFT + 2 * i));
            values.append(res);
          }
          break;
        case WRITE_COMMAND:
          qDebug() << "write_command";
          firstRegister =
              (buffer.at(REG_HIGH_SHIFT) << 8) | (buffer.at(REG_LOW_SHIFT));
          for (int i = 0; i < length; i++)
            values.append(
                (static_cast<quint8>(buffer.at(WRITE_HIGH_DATA_SHIFT + 2 * i))
                 << 8) |
                static_cast<quint8>(buffer.at(WRITE_LOW_DATA_SHIFT + 2 * i)));
          break;
        default:
          qDebug() << "ERR_cmd";
          emit signal_errorOccured(
              "Неверный функциональный код. Буффер (hex): " +
              buffer.toHex(' '));
      }
      qDebug() << "signal_newDataIsReady";
      emit signal_newDataIsReady();
    } else {
      emit signal_errorOccured("Контрольная сумма не сошлась! Буффер (hex): " +
                               buffer.toHex(' '));
    }
    buffer.clear();
  }
}

QList<quint16>& SerialPortHandler::getValues() { return values; }

void SerialPortHandler::setOpenState(
    bool state, std::optional<models::PortSettings> settings) {
  qDebug() << "SerialPortHandler::setOpenState state=" << state << " isOpen()"
           << dataThread->isOpen();
  if (state == dataThread->isOpen()) return;

  if (dataThread->isOpen()) {
    dataThread->disable();
    clearQueue();
  } else {
    dataThread->enable(settings.value());
    addr = settings->address;
  }
  emit signal_stateChanged(dataThread->isOpen());
}

void SerialPortHandler::startTransmit(QByteArray&& str) {
  // передача в порт
  dataThread->getQueue()->push(str);
  QString msg(QTime::currentTime().toString("HH:mm:ss.zzz") +
              " -> push to queue: " + str.toHex(' '));
  qDebug() << msg;
}

void SerialPortHandler::prepareWrite(reply_t&& qData) {
  buffer.clear();
  funcCode = WRITE_COMMAND;
  length = 1;
  QByteArray msg;
  msg.append(addr);
  msg.append(funcCode);
  msg.append(qData.reg >> 8);
  msg.append(qData.reg);
  msg.append(qData.val >> 8);
  msg.append(qData.val);
  quint16 crc = crc16(msg, msg.size());
  msg.append(static_cast<quint8>(crc >> 8));
  msg.append(static_cast<quint8>(crc));
  startTransmit(std::move(msg));
}

void SerialPortHandler::dataToWrite(quint16 startRegister, quint16 value) {
  if (!dataThread->isOpen()) return;

  reply_t reqData;
  reqData.isRead = false;
  reqData.reg = startRegister;
  reqData.val = value;

  prepareWrite(std::move(reqData));
}

void SerialPortHandler::dataToRead(quint16 startRegister, quint8 count) {
  if (!dataThread->isOpen()) return;

  reply_t reqData;
  reqData.isRead = true;
  reqData.reg = startRegister;
  reqData.val = count;

  prepareRead(std::move(reqData));
}

void SerialPortHandler::prepareRead(reply_t&& qData) {
  buffer.clear();
  funcCode = READ_COMMAND;
  firstRegister = qData.reg;
  length = qData.val;
  QByteArray msg;
  msg.append(addr);
  msg.append(funcCode);
  msg.append(static_cast<quint8>(qData.reg >> 8));
  msg.append(static_cast<quint8>(qData.reg & 0xff));
  msg.append(static_cast<quint8>(qData.val >> 8));
  msg.append(static_cast<quint8>(qData.val & 0xff));
  quint16 crc = crc16(msg, msg.size());
  msg.append(static_cast<quint8>(crc >> 8));
  msg.append(static_cast<quint8>(crc));
  startTransmit(std::move(msg));
}

quint16 SerialPortHandler::crc16(const QByteArray& ptr, int size) {
  uchar ucCRCHi = 0xFF;
  uchar ucCRCLo = 0xFF;
  int idx;

  for (int i = 0; i < size; i++) {
    uchar currByte = ptr.at(i);
    idx = ucCRCLo ^ currByte;
    ucCRCLo = (uchar)(ucCRCHi ^ aucCRCHi[idx]);
    ucCRCHi = aucCRCLo[idx];
  }

  return quint16(ucCRCLo << 8 | ucCRCHi);
}

void SerialPortHandler::slot_timeOut() {
  clearQueue();
  emit signal_errorOccured("Таймаут. Буффер: " + sentData);
  emit signal_timeout(addr);
}

int SerialPortHandler::getTimeout() { return timeout_period; }

void SerialPortHandler::setTimeout(int value) {
  timeout_period = value;
  dataThread->setTimeout(timeout_period);
}

void SerialPortHandler::clearQueue() { dataThread->getQueue()->clear(); }

void SerialPortHandler::clearBuffer() { buffer.clear(); }

quint8 SerialPortHandler::getAddress() { return addr; }

quint16 SerialPortHandler::getRegister() { return firstRegister; }

quint8 SerialPortHandler::getCount() { return length; }
