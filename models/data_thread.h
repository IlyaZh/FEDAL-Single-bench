#pragma once

#include <QSerialPort>
#include <QSharedPointer>
#include <QString>
#include <QThread>
#include <QtCore>

#include "globals.h"
#include "queue.hpp"
#ifdef USING_SERIAL_MOCK
#include "mocks/serial_mock.hpp"
#endif

namespace models {

struct PortSettings {
  qint32 baud_rate;
  QString port_name;
  quint8 address;
};

class DataThread : public QThread {
  Q_OBJECT
 public:
  DataThread(qint64 rx_package_size = 0, qint64 error_retry = 5,
             QObject *parent = nullptr);
  ~DataThread();
  qint64 countRetries();
  qint64 rxWaitForSize();
  QSharedPointer<Queue> getQueue();
  bool isOpen() const;

 public slots:
  void setTimeout(qint64 timeout_ms);
  void setDelay(qint64 delay_ms);
  void disable();
  void enable(PortSettings &settings);

 signals:
  void signal_stateChanged(bool open);
  void signal_readyToWrite();
  void signal_timeout(const QByteArray &last_package);
  void signal_errorOccured(const QString &error_message);
  void signal_receivedData(const QByteArray &received);

 private:
#ifdef USING_SERIAL_MOCK
  QSharedPointer<mocks::SerialPort_Mock> io;
#else
  QSharedPointer<QSerialPort> io;
#endif
  QSharedPointer<Queue> q;
  std::atomic<int> timeout{1000};
  std::atomic<int> delay{300};
  std::atomic<bool> is_working{false};
  qint64 rx_wait_size{};
  qint64 retry_counts{5};
  PortSettings settings_;

  void run() final override;
};

}  // namespace models
