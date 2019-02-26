#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "globals.h"
#include "appsettings.h"
#include <QDialog>
#include <QSerialPortInfo>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();
    void updateParameters();

private:
    Ui::SettingsDialog *ui;
    AppSettings* settings;
    void refreshPorts();

private slots:
    void saveSettings();
    void addressChanged(int);
    void on_refreshButton_clicked();
};

#endif // SETTINGSDIALOG_H
