#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QGridLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QSerialPort>

#include "aboutdialog.h"
#include "appsettings.h"
#include "deviceform.h"
#include "globals.h"
#include "serialporthandler.h"
#include "settingsdialog.h"

class SerialPortHandler;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void closeEvent(QCloseEvent* event);

 private:
  Ui::MainWindow* ui;
  bool link;
  DeviceForm* bench;
  AppSettings* settings;
  SettingsDialog* settingsDialog;
  AboutDialog* aboutDialog;
  SerialPortHandler* serialPort;
  bool portIsOpen;
  bool minLimitsIsLoaded;
  bool maxLimitsIsLoaded;
  int currentCommandId;
  void setupWindow();
  void loadComData();
  void setupConnections();
  void readSettings();
  void writeSettings();
  bool maybeSave();

 public slots:
  void settingsChanged();

 private slots:
  void onStateChanged(bool);
  void readReady();
  void setDevParam(quint16, quint16);
  void onTimeout(quint8);
  void requestNextParam();
  void errorMessage(QString);
  void on_connectButton_clicked();
  void on_settingsButton_clicked();
  void buttonStateChanged(bool);
  void prepareWrite(quint16, quint16);
};

#endif  // MAINWINDOW_H
