#ifndef SERIALPORTHANDLER_H
#define SERIALPORTHANDLER_H

#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QTimer>
#include <optional>

#include "models/data_thread.h"

struct PortSettings;

class SerialPortHandler : public QObject {
  Q_OBJECT
 public:
  explicit SerialPortHandler(QObject* parent = nullptr);
  ~SerialPortHandler();

  bool isOpen();
  int getTimeout();
  void setTimeout(int value = 50);
  void clearBuffer();
  quint8 getAddress();
  quint16 getRegister();
  quint8 getCount();
  QList<quint16>& getValues();
  int bytesToSend;

  enum {
    MIN_REGS_SHIFT = 0,
    MAX_REGS_SHIFT = 1,
    VALUES_REGS_SHIFT = 2
  } arrayRegShift_t;
  enum { READ_COMMAND = 0x03, WRITE_COMMAND = 0x06 } modBusCommands;
  enum {
    ADDRESS_SHIFT = 0,
    FUNCTION_CODE_SHIFT = 1,
    REG_HIGH_SHIFT = 2,
    REG_LOW_SHIFT = 3,
    READ_COUNT_SHIFT = 2,
    WRITE_HIGH_DATA_SHIFT = 4,
    WRITE_LOW_DATA_SHIFT = 5,
    READ_HIGH_DATA_SHIFT = 3,
    READ_LOW_DATA_SHIFT = 4
  } modBusShift;

 private:
  typedef struct {
    bool isRead;
    quint16 reg;
    quint16 val;
  } reply_t;
  models::DataThread* dataThread;
  QQueue<reply_t> queue;
  QByteArray buffer;
  quint16 crc16(const QByteArray& ptr, int size);
  quint8 addr;
  quint8 length;
  quint16 firstRegister;
  quint8 funcCode;
  QString sentData;
  int timeout_period;
  QList<quint16> values;
  void startTransmit(QByteArray&&);
  void prepareWrite(reply_t&& request);
  void prepareRead(reply_t&& request);

 signals:
  void signal_newDataIsReady();
  void signal_stateChanged(bool);
  void signal_errorOccured(const QString& error);
  void signal_timeout(quint8 address);

 public slots:
  void setOpenState(bool newState,
                    std::optional<models::PortSettings> settings);
  void dataToWrite(quint16 startRegister, quint16 value);
  void dataToRead(quint16 startRegister, quint8 count);
  void clearQueue();

 private slots:
  void slot_readyRead(const QByteArray& received_package);
  void slot_timeOut();
};

#endif  // SERIALPORTHANDLER_H
