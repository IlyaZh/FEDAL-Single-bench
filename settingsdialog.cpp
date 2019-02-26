#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    settings = new AppSettings(this);

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui->addressBox, SIGNAL(valueChanged(int)), this, SLOT(addressChanged(int)));

    updateParameters();
}



SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::refreshPorts() {
    ui->portBox->clear();
    foreach (QSerialPortInfo item, QSerialPortInfo::availablePorts()) {
        ui->portBox->addItem(item.portName());
    }
}

// private slots
void SettingsDialog::saveSettings() {
    settings->setAddress(ui->addressBox->value());
    settings->setBaudRate(ui->baudBox->currentData().toInt());
    settings->setPortName(ui->portBox->currentText());
    settings->setPortAutoconnect(ui->autoConnectCheckBox->isChecked());

    qInfo() << "Settings was saved: " << QString::number(settings->getAddress()) << settings->getPortName() << QString::number(settings->getBaudRate()) + "bps" << "Autoconnect: " + QString(settings->getPortAutoconnect());
}

void SettingsDialog::addressChanged(int value) {
    QString str = QString("Адрес (hex):") + QString("%1").arg(value, 0, 16).toUpper() + QString("h");
    ui->hexAddressLabel->setText(str);
}

void SettingsDialog::on_refreshButton_clicked()
{
    refreshPorts();
}

void SettingsDialog::updateParameters() {
    ui->autoConnectCheckBox->setChecked(settings->getPortAutoconnect());
    refreshPorts();

    ui->portBox->setCurrentText(settings->getPortName());

    foreach (quint32 item, BAUDRATES) {
        ui->baudBox->addItem(QString::number(item), QVariant(item));
    }
    ui->addressBox->setValue(settings->getAddress());
    addressChanged(settings->getAddress());
}
