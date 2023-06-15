#ifndef DEVICEFORM_H
#define DEVICEFORM_H

#include <QLocale>
#include <QPushButton>
#include <QTime>
#include <QWidget>

#include "globals.h"

namespace Ui {
class DeviceForm;
}

class DeviceForm : public QWidget {
  Q_OBJECT

 public:
  explicit DeviceForm(QWidget *parent = 0);
  ~DeviceForm();
  enum {
    FREQUENCY_SHIFT = 0x0001,
    MIN_FREQUENCY_SHIFT = 0x0011,
    MAX_FREQUENCY_SHIFT = 0x0021,
    DURATION_SHIFT = 0x0002,
    MIN_DURATION_SHIFT = 0x0012,
    MAX_DURATION_SHIFT = 0x0022,
    CURRENT_SHIFT = 0x0003,
    MIN_CURRENT_SHIFT = 0x0013,
    MAX_CURRENT_SHIFT = 0x0023,
    VOLTAGE_SHIFT = 0x0004,
    MIN_VOLTAGE_SHIFT = 0x0014,
    MAX_VOLTAGE_SHIFT = 0x0024,
    DELAY_SHIFT = 0x0005,
    MIN_DELAY_SHIFT = 0x0015,
    MAX_DELAY_SHIFT = 0x0025,
    STATUS_READ_SHIFT = 0x0007,
    BLOCKS_SHIFT = 0x0008,
    STATUS_WRITE_SHIFT = 0x0009
  } commandsShift;
  enum {
    POWER_ON_MASK = 0x02,
    POWER_OFF_MASK = 0x04,
    START_MASK = 1 << 3,
    STOP_MASK = 1 << 4,
    MASTER_MASK = 1 << 5,
    SLAVE_MASK = 1 << 6,
    EXT_SYNC_MASK = 0x40,
    INT_SYNC_MASK = 0x20
  } statusWriteMask;
  enum {
    POWER_ON_READ_MASK = 1,
    START_STOP_READ_MASK = 1 << 1,
    MASTER_SLAVE_READ_MASK = 1 << 2,
    INT_SYNC_READ_MASK = 1 << 2
  } statusReadMask;
  enum { EXT_BLOCK = 1 << 1, TEMP_BLOCK = 1 << 4 } blocksReadMask;

 private:
  Ui::DeviceForm *ui;
  QLocale locale;
  int frequency, duration, current, voltage, delay;
  bool isSlave;

 public slots:
  void setFrequency(double);
  void setDuration(int);
  void setDuration(double);
  void setCurrent(double);
  void setVoltage(double);
  void setDelay(int);
  void setDelay(double);
  void setMinFrequency(double);
  void setMinDuration(int);
  void setMinCurrent(double);
  void setMinVoltage(double);
  void setMinDelay(int);
  void setMaxFrequency(double);
  void setMaxDuration(int);
  void setMaxCurrent(double);
  void setMaxVoltage(double);
  void setMaxDelay(int);
  void setDeviceState(int);
  void setDeviceBlocks(int);
  void setStartStop(bool);
  void setIntExtSync(bool);
  void setPower(bool);
  void setLink(bool);
  void setExtBlock(bool);
  void setTempBlock(bool);
  void setWidgetEnable(bool);

 private slots:
  void startStopButtonClicked(bool);
  void syncButtonClicked();
  void powerButtonClicked(bool);
  void frequencyDialSlot(int);
  void durationDialSlot(int);
  void currentDialSlot(int);
  void voltageDialSlot(int);
  void delayDialSlot(int);
  void frequencyDialSend();
  void durationDialSend();
  void currentDialSend();
  void voltageDialSend();
  void delayDialSend();
  void frequencySpinEditFinished();
  void durationSpinEditFinished();
  void currentSpinEditFinished();
  void voltageSpinEditFinished();
  void delaySpinEditFinished();
  void setFrequencyUi(double);
  void setDurationUi(int);
  void setDurationUi(double);
  void setCurrentUi(double);
  void setVoltageUi(double);
  void setDelayUi(int);
  void setDelayUi(double);

 signals:
  void updateParameter(quint16, quint16);
};

#endif  // DEVICEFORM_H
