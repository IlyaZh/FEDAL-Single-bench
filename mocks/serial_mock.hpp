#pragma once

#include <QDebug>
#include <QSerialPort>
#include <QtCore>

#include "crctable.h"

namespace mocks {

class SerialPort_Mock : public QSerialPort {
  Q_OBJECT
 public:
  SerialPort_Mock(QObject* parent = nullptr) : QSerialPort(parent) {}

  bool open(OpenMode mode) override {
    Q_UNUSED(mode);
    setOpenMode(ReadWrite);
    qDebug() << "Open port" << static_cast<int>(openMode());

    return true;
  };
  qint64 write(const QByteArray& data) {
    qDebug() << "Write port (HEX): " << data.toHex(' ');
    written_ = data;
    return data.size();
  }
  bool waitForBytesWritten(int msecs = 30000) override {
    Q_UNUSED(msecs);
    return true;
  }
  bool waitForReadyRead(int msecs = 30000) override {
    Q_UNUSED(msecs);
    return true;
  }
  QByteArray readAll() {
    QByteArray answer;
    answer.push_back(written_.at(0));
    const auto code = written_.at(1);
    answer.push_back(code);
    switch (code) {
      case 0x03:
        answer.push_back(make_answer_03());
        break;
      case 0x06:
        answer.push_back(make_answer_06());
        break;
      default:
        qDebug() << "Wrong modbus code " << code;
        return {};
    }
    const auto crc = crc16(answer, answer.size());
    answer.push_back(static_cast<quint8>((crc >> 8) & 0xff));
    answer.push_back(static_cast<quint8>(crc & 0xff));
    qDebug() << "Read port (HEX): " << answer.toHex(' ');
    return answer;
  }

 private:
  QByteArray written_;
  QHash<quint16, quint16> values_;

  quint16 crc16(const QByteArray& ptr, int size) {
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
  QByteArray make_answer_03() {
    QByteArray answer;
    const auto count = 2 * ((static_cast<quint16>(written_.at(4)) << 8) +
                            static_cast<quint16>(written_.at(5)));
    answer.push_back(count);
    const quint16 begin_addr = ((static_cast<quint16>(written_.at(2)) << 8) +
                                static_cast<quint16>(written_.at(3)));
    for (int i = 0; i < count / 2; ++i) {
      const auto addr = begin_addr + i;
      const auto value = values_.value(addr, 0);
      answer.push_back(static_cast<quint8>((value >> 8) & 0xff));
      answer.push_back(static_cast<quint8>(value & 0xff));
    }
    return answer;
  }
  QByteArray make_answer_06() {
    QByteArray answer;
    const quint16 addr = ((static_cast<quint16>(written_.at(2)) << 8) +
                          static_cast<quint16>(written_.at(3)));
    const quint16 new_value = ((static_cast<quint16>(written_.at(4)) << 8) +
                               static_cast<quint16>(written_.at(5)));
    answer.push_back(static_cast<quint8>((addr >> 8) & 0xff));
    answer.push_back(static_cast<quint8>((addr)&0xff));
    answer.push_back(static_cast<quint8>((new_value >> 8) & 0xff));
    answer.push_back(static_cast<quint8>((new_value)&0xff));
    values_[addr] = new_value;
    return answer;
  }
};

}  // namespace mocks
