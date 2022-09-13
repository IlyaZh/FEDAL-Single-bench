#pragma once

#include <QByteArray>
#include <QMutex>
#include <QQueue>
#include <QSemaphore>
#include <QThread>
#include <QtCore>
#include <optional>

namespace models {

class Queue : public QObject {
  Q_OBJECT
 private:
  const int kMaxSize;
  QMutex mtx;
  QSemaphore sem;
  QQueue<QByteArray> queue;

 signals:
  void EmptyOccured();

 public:
  Queue(int max_size = 300, QObject* parent = nullptr)
      : QObject(parent), kMaxSize(max_size) {}

  ~Queue() { clear(); }

  QByteArray pop(int timeout = -1) {
    sem.tryAcquire(1, timeout);
    QMutexLocker lk(&mtx);
    if (queue.size() == 1) {
      emit EmptyOccured();
    }
    return queue.dequeue();
  }

  bool empty() {
    QMutexLocker lk(&mtx);
    return (sem.available() != 0);
  }

 public slots:

  void push(const QByteArray& array) {
    QMutexLocker lk(&mtx);
    queue.enqueue(array);
    sem.release();
  }

  void push(QByteArray&& array) {
    QMutexLocker lk(&mtx);
    queue.enqueue(array);
    sem.release();
  }

  void push_at_head(const QByteArray& array) {
    QMutexLocker lk(&mtx);
    queue.prepend(array);
    sem.release();
  }

  void push_at_head(QByteArray&& array) {
    QMutexLocker lk(&mtx);
    queue.prepend(array);
    sem.release();
  }

  void clear() {
    QMutexLocker lk(&mtx);
    sem.acquire(sem.available());
    queue.clear();
  }
};

}  // namespace models
