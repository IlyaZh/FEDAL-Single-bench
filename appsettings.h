#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QPoint>
#include <QSettings>
#include <QSize>

#include "globals.h"

class AppSettings : public QObject {
  Q_OBJECT
 public:
  explicit AppSettings(QObject *parent = 0);
  int getBaudRate();
  quint8 getAddress();
  bool getPortAutoconnect();
  QString getPortName();
  QPoint getWindowPosition();
  QSize getWindowSize();
  void setBaudRate(int);
  void setAddress(quint8);
  void setPortAutoconnect(bool);
  void setPortName(QString);
  void setWindowSize(QSize);
  void setWindowPosision(QPoint);

 private:
  QSettings *settings;
};

#endif  // APPSETTINGS_H
