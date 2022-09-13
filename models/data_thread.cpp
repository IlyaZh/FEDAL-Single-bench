#include "data_thread.h"

#include "queue.hpp"

#ifdef USING_SERIAL_MOCK
#include "mocks/serial_mock.hpp"
#else
#include <QSerialPort>
#endif

namespace models {

DataThread::DataThread(qint64 rx_package_size, qint64 error_retry,
                       QObject* parent)
    : QThread(parent),
      rx_wait_size(rx_package_size),
      retry_counts(error_retry) {}

DataThread::~DataThread() { q.clear(); }

qint64 DataThread::countRetries() { return retry_counts; }

qint64 DataThread::rxWaitForSize() { return rx_wait_size; }

QSharedPointer<Queue> DataThread::getQueue() { return q; }

bool DataThread::isOpen() const { return is_working.load(); }

void DataThread::setTimeout(qint64 timeout_ms) { timeout.store(timeout_ms); }

void DataThread::setDelay(qint64 delay_ms) { delay.store(delay_ms); }

void DataThread::disable() {
  is_working.store(false);
  q->push("");
  q->push("");
  qDebug() << "disable thread";
}

void DataThread::enable(PortSettings& settings) {
  qDebug() << "DataThread enable isRunning()" << isRunning();
  if (!isRunning()) {
    is_working.store(true);
    settings_ = settings;
    QThread::start(LowPriority);
  }
}

void DataThread::run() {
  q = QSharedPointer<Queue>::create();
#ifdef USING_SERIAL_MOCK
  io = QSharedPointer<mocks::SerialPort_Mock>::create();
#else
  io = QSharedPointer<QSerialPort>::create();
#endif
  io->setBaudRate(settings_.baud_rate);
  io->setPortName(settings_.port_name);
  io->setStopBits(QSerialPort::TwoStop);
  io->setDataBits(QSerialPort::Data8);
  io->setParity(QSerialPort::NoParity);
  io->open(QIODevice::ReadWrite);
  if (!io->isOpen()) {
    emit signal_errorOccured("IO device hasn't opened");
    return;
  }
  emit signal_stateChanged(io->isOpen());

  //  if (q->empty()) {
  //    emit signal_readyToWrite();
  //  }
  //  connect(q.data(), &Queue::EmptyOccured, this,
  //          &DataThread::signal_readyToWrite);
  qDebug() << "Serial port has started";
  QByteArray rx_buffer;
  while (is_working.load()) {
    const auto t = timeout.load();
    if (!io->isOpen()) {
      emit signal_errorOccured(
          QString("IO device hasn't opened. Waiting for %1 ms timeout.")
              .arg(t));
      QThread::msleep(t);
    }

    auto package = q->pop();
    if (package.isEmpty()) continue;
    io->write(package);
    qDebug() << QTime::currentTime().toString("HH:mm:ss.zzz")
             << "Tx: " << package.toHex(' ');
    if (!io->waitForBytesWritten(t)) {
      emit signal_timeout(package);
      continue;
    }
    if (!io->waitForReadyRead(t)) {
      emit signal_timeout(package);
      continue;
    }
    auto received_package = io->readAll();
    qDebug() << "Rx: " << received_package.toHex(' ');
    rx_buffer.append(received_package);
    if (received_package.size() >= rx_wait_size) {
      qDebug() << "emit received data";
      emit signal_receivedData(rx_buffer);
      rx_buffer.clear();
    }
    QThread::msleep(delay.load());
  }
  if (q) {
    q.data()->disconnect();
    q->clear();
  }

  emit signal_stateChanged(false);
  qDebug() << "Serial port thread has stopped";
  if (io) {
    io->close();
  }
  io = nullptr;
}

}  // namespace models
