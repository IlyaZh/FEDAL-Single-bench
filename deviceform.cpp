#include "deviceform.h"
#include "ui_deviceform.h"

DeviceForm::DeviceForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceForm)
{
    ui->setupUi(this);

    ui->frequencySpinBox->setSingleStep(1/round(pow(10, FREQUENCY_FRACTIAL_SYMBOLS)));
    ui->durationSpinBox->setSingleStep(1/round(pow(10, DURATION_FRACTIAL_SYMBOLS)));
    ui->currentSpinBox->setSingleStep(1/round(pow(10, CURRENT_FRACTIAL_SYMBOLS)));
    ui->voltageSpinBox->setSingleStep(1/round(pow(10, VOLTAGE_FRACTIAL_SYMBOLS)));
    ui->delaySpinBox->setSingleStep(1/round(pow(10, DELAY_FRACTIAL_SYMBOLS)));

    ui->frequencySpinBox->setDecimals(FREQUENCY_FRACTIAL_SYMBOLS);
    ui->durationSpinBox->setDecimals(DURATION_FRACTIAL_SYMBOLS);
    ui->currentSpinBox->setDecimals(CURRENT_FRACTIAL_SYMBOLS);
    ui->voltageSpinBox->setDecimals(VOLTAGE_FRACTIAL_SYMBOLS);
    ui->delaySpinBox->setDecimals(DELAY_FRACTIAL_SYMBOLS);

    ui->frequencySpinBox->setSuffix(QString(" ") + FREQUENCY_UNIT);
    ui->durationSpinBox->setSuffix(QString(" ") + DURATION_UNIT);
    ui->currentSpinBox->setSuffix(QString(" ") + CURRENT_UNIT);
    ui->voltageSpinBox->setSuffix(QString(" ") + VOLTAGE_UNIT);
    ui->delaySpinBox->setSuffix(QString(" ") + DELAY_UNIT);

    isSlave = true;

    ui->startStopButton->setChecked(false);
    ui->syncButton->setChecked(isSlave); // master = 0, slave = 1
    ui->powerButton->setChecked(false);

    connect(ui->startStopButton, SIGNAL(clicked(bool)), this, SLOT(startStopButtonClicked(bool)));
    connect(ui->syncButton, SIGNAL(clicked(bool)), this, SLOT(syncButtonClicked()));
    connect(ui->powerButton, SIGNAL(clicked(bool)), this, SLOT(powerButtonClicked(bool)));

    // связь крутилок и спин-боксов между собой
//    connect(ui->frequencyDial, SIGNAL(sliderMoved(int)), this, SLOT(frequencyDialSlot(int)));
//    connect(ui->durationDial, SIGNAL(sliderMoved(int)), this, SLOT(durationDialSlot(int)));
//    connect(ui->currentDial, SIGNAL(sliderMoved(int)), this, SLOT(currentDialSlot(int)));
//    connect(ui->voltageDial, SIGNAL(sliderMoved(int)), this, SLOT(voltageDialSlot(int)));
//    connect(ui->delayDial, SIGNAL(sliderMoved(int)), this, SLOT(delayDialSlot(int)));

    connect(ui->frequencyDial, SIGNAL(valueChanged(int)), this, SLOT(frequencyDialSlot(int)));
    connect(ui->durationDial, SIGNAL(valueChanged(int)), this, SLOT(durationDialSlot(int)));
    connect(ui->currentDial, SIGNAL(valueChanged(int)), this, SLOT(currentDialSlot(int)));
    connect(ui->voltageDial, SIGNAL(valueChanged(int)), this, SLOT(voltageDialSlot(int)));
    connect(ui->delayDial, SIGNAL(valueChanged(int)), this, SLOT(delayDialSlot(int)));

    connect(ui->frequencySpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFrequencyUi(double)));
    connect(ui->durationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setDurationUi(double)));
    connect(ui->currentSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setCurrentUi(double)));
    connect(ui->voltageSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setVoltageUi(double)));
    connect(ui->delaySpinBox, SIGNAL(valueChanged(double)), this, SLOT(setDelayUi(double)));

    // отправка нового значения параметра в устройство (обработка "окончания" действия редактирования поля пользователем)
    connect(ui->frequencyDial, SIGNAL(sliderReleased()), this, SLOT(frequencyDialSend()));
    connect(ui->durationDial, SIGNAL(sliderReleased()), this, SLOT(durationDialSend()));
    connect(ui->currentDial, SIGNAL(sliderReleased()), this, SLOT(currentDialSend()));
    connect(ui->voltageDial, SIGNAL(sliderReleased()), this, SLOT(voltageDialSend()));
    connect(ui->delayDial, SIGNAL(sliderReleased()), this, SLOT(delayDialSend()));

    connect(ui->frequencySpinBox, SIGNAL(editingFinished()), this, SLOT(frequencySpinEditFinished()));
    connect(ui->durationSpinBox, SIGNAL(editingFinished()), this, SLOT(durationSpinEditFinished()));
    connect(ui->currentSpinBox, SIGNAL(editingFinished()), this, SLOT(currentSpinEditFinished()));
    connect(ui->voltageSpinBox, SIGNAL(editingFinished()), this, SLOT(voltageSpinEditFinished()));
    connect(ui->delaySpinBox, SIGNAL(editingFinished()), this, SLOT(delaySpinEditFinished()));

    frequency = duration = current = voltage = delay = -1;

    setWidgetEnable(false);
}

DeviceForm::~DeviceForm()
{
    delete ui;
}

// public slots
void DeviceForm::setFrequency(double value) {
    ui->frequencyValueLabel->setText(locale.toString(value, DOUBLE_FORMAT, FREQUENCY_FRACTIAL_SYMBOLS) + QString(" ") + FREQUENCY_UNIT);
    if(ui->frequencyDial->hasFocus() || ui->frequencySpinBox->hasFocus()) return;
    setFrequencyUi(value);
}

void DeviceForm::setDuration(int value) {
    ui->durationValueLabel->setText(locale.toString(value) + QString(" ") + DURATION_UNIT);
    if(ui->durationDial->hasFocus() || ui->durationSpinBox->hasFocus()) return;
    setDurationUi(value);
}

void DeviceForm::setDuration(double value) {
    ui->durationValueLabel->setText(locale.toString(value, DOUBLE_FORMAT, DURATION_FRACTIAL_SYMBOLS) + QString(" ") + DURATION_UNIT);
    if(ui->durationDial->hasFocus() || ui->durationSpinBox->hasFocus()) return;
    setDurationUi(value);
}

void DeviceForm::setCurrent(double value) {
    ui->currentValueLabel->setText(locale.toString(value, DOUBLE_FORMAT, CURRENT_FRACTIAL_SYMBOLS) + QString(" ") + CURRENT_UNIT);
    if(ui->currentDial->hasFocus() || ui->currentSpinBox->hasFocus()) return;
    setCurrentUi(value);
}

void DeviceForm::setVoltage(double value) {
    ui->voltageValueLabel->setText(locale.toString(value, DOUBLE_FORMAT, VOLTAGE_FRACTIAL_SYMBOLS) + QString(" ") + VOLTAGE_UNIT);
    if (ui->voltageDial->hasFocus() || ui->voltageSpinBox->hasFocus()) return;
    setVoltageUi(value);
}

void DeviceForm::setDelay(int value) {
    ui->delayValueLabel->setText(locale.toString(value) + QString(" ") + DELAY_UNIT);
    if(ui->delayDial->hasFocus() || ui->delaySpinBox->hasFocus()) return;
    setDelayUi(value);
}

void DeviceForm::setDelay(double value) {
    ui->delayValueLabel->setText(locale.toString(value, DOUBLE_FORMAT, DELAY_FRACTIAL_SYMBOLS) + QString(" ") + DELAY_UNIT);
    if(ui->delayDial->hasFocus() || ui->delaySpinBox->hasFocus()) return;
    setDelayUi(value);
}

void DeviceForm::setFrequencyUi(double value) {
    frequency = qRound(value*pow(10,FREQUENCY_FRACTIAL_SYMBOLS));
    ui->frequencyDial->setValue(frequency);
    ui->frequencySpinBox->setValue(value);
}

void DeviceForm::setDurationUi(int value) {
    duration = value;
    ui->durationDial->setValue(duration);
    ui->durationSpinBox->setValue(duration);
}

void DeviceForm::setDurationUi(double value) {
    duration = qRound(value*pow(10,DURATION_FRACTIAL_SYMBOLS));
    ui->durationDial->setValue(duration);
    ui->durationSpinBox->setValue(value);
}

void DeviceForm::setCurrentUi(double value) {
    current = qRound(value*pow(10,CURRENT_FRACTIAL_SYMBOLS));
    ui->currentDial->setValue(qRound(value*pow(10, CURRENT_FRACTIAL_SYMBOLS)));
    ui->currentSpinBox->setValue(value);
}

void DeviceForm::setVoltageUi(double value) {
    voltage = qRound(value*pow(10,VOLTAGE_FRACTIAL_SYMBOLS));
    ui->voltageDial->setValue(voltage);
    ui->voltageSpinBox->setValue(value);
}

void DeviceForm::setDelayUi(int value) {
    delay = value;
    ui->delayDial->setValue(delay);
    ui->delaySpinBox->setValue(delay);
}

void DeviceForm::setDelayUi(double value) {
    delay = qRound(value*pow(10, DELAY_FRACTIAL_SYMBOLS));
    ui->delayDial->setValue(delay);
    ui->delaySpinBox->setValue(value);
}

void DeviceForm::setMinFrequency(double value) {
    ui->frequencyDial->setMinimum(qRound(value*pow(10, FREQUENCY_FRACTIAL_SYMBOLS)));
    ui->frequencySpinBox->setMinimum(value);
}

void DeviceForm::setMinDuration(int value) {
    ui->durationDial->setMinimum(value);
    ui->durationSpinBox->setMinimum(value);
}

void DeviceForm::setMinCurrent(double value) {
    ui->currentDial->setMinimum(qRound(value*pow(10, CURRENT_FRACTIAL_SYMBOLS)));
    ui->currentSpinBox->setMinimum(value);
}

void DeviceForm::setMinVoltage(double value) {
    ui->voltageDial->setMinimum(qRound(value*pow(10, VOLTAGE_FRACTIAL_SYMBOLS)));
    ui->voltageSpinBox->setMinimum(value);
}

void DeviceForm::setMinDelay(int value) {
    ui->delayDial->setMinimum(value);
    ui->delaySpinBox->setMinimum(value);
}

void DeviceForm::setMaxFrequency(double value) {
    ui->frequencyDial->setMaximum(qRound(value*pow(10, FREQUENCY_FRACTIAL_SYMBOLS)));
    ui->frequencySpinBox->setMaximum(value);
}

void DeviceForm::setMaxDuration(int value) {
    ui->durationDial->setMaximum(value);
    ui->durationSpinBox->setMaximum(value);
}

void DeviceForm::setMaxCurrent(double value) {
    ui->currentDial->setMaximum(qRound(value*pow(10, CURRENT_FRACTIAL_SYMBOLS)));
    ui->currentSpinBox->setMaximum(value);
}

void DeviceForm::setMaxVoltage(double value) {
    ui->voltageDial->setMaximum(qRound(value*pow(10, VOLTAGE_FRACTIAL_SYMBOLS)));
    ui->voltageSpinBox->setMaximum(value);
}

void DeviceForm::setMaxDelay(int value) {
    ui->delayDial->setMaximum(value);
    ui->delaySpinBox->setMaximum(value);
}

void DeviceForm::setDeviceState(int value) {
    if(value & POWER_ON_READ_MASK) {
        setPower(true);
    } else {
        setPower(false);
    }

    if(value & START_STOP_READ_MASK) {
        setStartStop(true);
    } else {
        setStartStop(false);
    }

    if(value & INT_SYNC_READ_MASK) {
        setIntExtSync(true);
    } else {
        setIntExtSync(false);
    }
}

void DeviceForm::setDeviceBlocks(int value) {
    if(value & EXT_BLOCK) {
        setExtBlock(true);
    } else {
        setExtBlock(false);
    }

    if(value & TEMP_BLOCK) {
        setTempBlock(true);
    } else {
        setTempBlock(false);
    }
}

void DeviceForm::setStartStop(bool flag) {
    ui->startStopButton->setChecked(flag);
    if(flag) {
        ui->startStopButton->setText("Генерация");
    } else {
        ui->startStopButton->setText("БЕЗ генерации");
    }
}

void DeviceForm::setIntExtSync(bool flag) {
    isSlave = flag;
    ui->syncButton->setChecked(isSlave);
    if(isSlave) {
        ui->syncButton->setText("Внешняя синхронизация");
    } else {
        ui->syncButton->setText("Внутренняя синхронизация");
    }
}

void DeviceForm::setPower(bool flag) {
    ui->powerButton->setChecked(flag);
    if(flag) {
        ui->powerButton->setText("Включен");
    } else {
        ui->powerButton->setText("Выключен");
    }
}

void DeviceForm::setLink(bool flag) {
    if(flag) {
        ui->linkLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid rgb(0, 255, 0); background-color: rgb(0, 255, 0); }");
    } else {
        ui->linkLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid red; background-color: rgb(229, 34, 0); }");
    }
}

void DeviceForm::setExtBlock(bool flag) {
    if(flag) {
        ui->intExtBlockLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid rgb(0, 255, 0); background-color: rgb(0, 255, 0); }");
    } else {
        ui->intExtBlockLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid red; background-color: rgb(229, 34, 0); }");
    }
}

void DeviceForm::setTempBlock(bool flag) {
    if(flag) {
        ui->tempBlockLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid rgb(0, 255, 0); background-color: rgb(0, 255, 0); }");
    } else {
        ui->tempBlockLabel->setStyleSheet("QLabel { width: 20px; height: 20px; border-radius: 10px; border: 1px solid red; background-color: rgb(229, 34, 0); }");
    }
}

void DeviceForm::setWidgetEnable(bool flag) {
    ui->frequencyDial->setEnabled(flag);
    ui->durationDial->setEnabled(flag);
    ui->currentDial->setEnabled(flag);
    ui->voltageDial->setEnabled(flag);
    ui->delayDial->setEnabled(flag);
    ui->frequencySpinBox->setEnabled(flag);
    ui->durationSpinBox->setEnabled(flag);
    ui->currentSpinBox->setEnabled(flag);
    ui->voltageSpinBox->setEnabled(flag);
    ui->delaySpinBox->setEnabled(flag);
    ui->startStopButton->setEnabled(flag);
    ui->syncButton->setEnabled(flag);
    ui->powerButton->setEnabled(flag);
}

// private slots

void DeviceForm::startStopButtonClicked(bool flag) {
    if(flag) {
        emit updateParameter(STATUS_WRITE_SHIFT, START_MASK);
    } else {
        emit updateParameter(STATUS_WRITE_SHIFT, STOP_MASK);
    }
}

void DeviceForm::syncButtonClicked() {
    if(isSlave) {
        emit updateParameter(STATUS_WRITE_SHIFT, INT_SYNC_MASK);
    } else {
        emit updateParameter(STATUS_WRITE_SHIFT, EXT_SYNC_MASK);
    }
}

void DeviceForm::powerButtonClicked(bool flag) {
    if(flag) {
        emit updateParameter(STATUS_WRITE_SHIFT, POWER_ON_MASK);
    } else {
        emit updateParameter(STATUS_WRITE_SHIFT, POWER_OFF_MASK);
    }
}

void DeviceForm::frequencyDialSlot(int value) {
    setFrequencyUi(value / pow(10, FREQUENCY_FRACTIAL_SYMBOLS));
}

void DeviceForm::durationDialSlot(int value) {
    setDurationUi(value);
}

void DeviceForm::currentDialSlot(int value) {
    setCurrentUi(value / pow(10, CURRENT_FRACTIAL_SYMBOLS));
}

void DeviceForm::voltageDialSlot(int value) {
    setVoltageUi(value / pow(10, VOLTAGE_FRACTIAL_SYMBOLS));
}

void DeviceForm::delayDialSlot(int value) {
    setDelayUi(value);
}

void DeviceForm::frequencyDialSend() {
    if(ui->frequencyDial->isSliderDown()) return;
    emit updateParameter(FREQUENCY_SHIFT, ui->frequencyDial->value());
}

void DeviceForm::durationDialSend() {
    if(ui->durationDial->isSliderDown()) return;
    emit updateParameter(DURATION_SHIFT, ui->durationDial->value());
}

void DeviceForm::currentDialSend() {
    if(ui->currentDial->isSliderDown()) return;
    emit updateParameter(CURRENT_SHIFT, ui->currentDial->value());
}

void DeviceForm::voltageDialSend() {
    if(ui->voltageDial->isSliderDown()) return;
    emit updateParameter(VOLTAGE_SHIFT, ui->voltageDial->value());
}

void DeviceForm::delayDialSend() {
    if(ui->delayDial->isSliderDown()) return;
    emit updateParameter(DELAY_SHIFT, ui->delayDial->value());
}

void DeviceForm::frequencySpinEditFinished() {
    emit updateParameter(FREQUENCY_SHIFT, ui->frequencyDial->value());
}

void DeviceForm::durationSpinEditFinished() {
    emit updateParameter(DURATION_SHIFT, ui->durationDial->value());
}

void DeviceForm::currentSpinEditFinished() {
    emit updateParameter(CURRENT_SHIFT, ui->currentDial->value());
}

void DeviceForm::voltageSpinEditFinished() {
    emit updateParameter(VOLTAGE_SHIFT, ui->voltageDial->value());
}

void DeviceForm::delaySpinEditFinished() {
    emit updateParameter(DELAY_SHIFT, ui->delayDial->value());
}
