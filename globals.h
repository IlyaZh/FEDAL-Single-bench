#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QLocale>
#include <QDebug>

//#define DEBUG_MODE
//#define SHOW_UART_LOG

#ifdef DEBUG_MODE
    #include <QDebug>
#endif

const QString LOG_FILE = "protocol.log";

const QString ORG_NAME = "FEDAL";
const QString APP_NAME = "SingleWorkBench";
const QString APP_TITLE = "FEDAL инструменты управления";
const quint32 MAJOR_VERSION = 1;
const quint32 MINOR_VERSION = 0;
const QString VERSION = QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
const QString WINDOW_TITLE = APP_TITLE + " (Версия " + QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION) + ")";

const QList<quint32> BAUDRATES = {9600, 14400, 19200, 28800, 38400, 57600, 115200};
const int COM_TIMEOUT = 100; // ms

//extern QLocale wlocale;

const char DOUBLE_FORMAT = 'f';

const int CURRENT_FRACTIAL_SYMBOLS = 1;
const int DURATION_FRACTIAL_SYMBOLS = 0;
const int FREQUENCY_FRACTIAL_SYMBOLS = 1;
const int VOLTAGE_FRACTIAL_SYMBOLS = 1;
const int DELAY_FRACTIAL_SYMBOLS = 0;
const QString CURRENT_UNIT = "A";
const QString DURATION_UNIT = "мкс";
const QString FREQUENCY_UNIT = "Гц";
const QString VOLTAGE_UNIT = "В";
const QString DELAY_UNIT = "мкс";

const int STATUSBAR_MESSAGE_TIMEOUT = 5000; //ms

const QVector<quint16> readRegisters = {0x0011, 0x0021, 0x0001};
const QVector<quint16> countRegisters = {5, 5, 9};
enum {MIN_REGS_SHIFT = 0, MAX_REGS_SHIFT = 1, VALUES_REGS_SHIFT = 2} arrayRegShift_t;

const int MAX_LOG_FILE_SIZE = 1024*1024*20; // MB

#endif // GLOBALS_H
