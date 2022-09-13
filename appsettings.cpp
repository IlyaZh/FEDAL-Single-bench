#include "appsettings.h"

AppSettings::AppSettings(QObject *parent) : QObject(parent) {
  settings = new QSettings(ORG_NAME, APP_NAME);
}

// public interface

int AppSettings::getBaudRate() {
  return settings->value("comPort/baudRate", BAUDRATES[0]).toInt();
}

quint8 AppSettings::getAddress() {
  return settings->value("comPort/address", 1).toUInt();
}

bool AppSettings::getPortAutoconnect() {
  return settings->value("comPort/autoconnect", false).toBool();
}

QString AppSettings::getPortName() {
  return settings->value("comPort/portName", "COM1").toString();
}

QPoint AppSettings::getWindowPosition() {
  return settings->value("window/pos", QPoint(-1, -1)).toPoint();
}
QSize AppSettings::getWindowSize() {
  return settings->value("window/size", QSize(-1, -1)).toSize();
}

void AppSettings::setBaudRate(int baud) {
  settings->setValue("comPort/baudRate", baud);
}

void AppSettings::setAddress(quint8 addr) {
  settings->setValue("comPort/address", addr);
}

void AppSettings::setPortAutoconnect(bool flag) {
  settings->setValue("comPort/autoconnect", flag);
}

void AppSettings::setPortName(QString port) {
  settings->setValue("comPort/portName", port);
}

void AppSettings::setWindowSize(QSize size) {
  settings->setValue("window/size", size);
}

void AppSettings::setWindowPosision(QPoint point) {
  settings->setValue("window/pos", point);
}
