#include <QApplication>
#include <QLibraryInfo>
#include <QTextStream>
#include <QTranslator>

#include "globals.h"
#include "mainwindow.h"

void messageToFile(QtMsgType type, const QMessageLogContext &context,
                   const QString &msg);

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

#ifndef STOP_LOG_TO_FILE
  qInstallMessageHandler(messageToFile);
#endif

  // set translator for default widget's text (for example: QMessageBox's
  // buttons)
  QTranslator qtTranslator;
  qtTranslator.load("qt_ru",
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  a.installTranslator(&qtTranslator);

  MainWindow w;
  w.show();

  return a.exec();
}

void messageToFile(QtMsgType type, const QMessageLogContext &context,
                   const QString &msg) {
  QFile file(LOG_FILE);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
    return;
  }

  if (file.size() > MAX_LOG_FILE_SIZE) {
    file.resize(0);
  }

  QTextStream out(&file);

  switch (type) {
    case QtInfoMsg:
      out << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss:zzz")
          << " Info: " << msg << ",     " << context.file << "     "
          << context.function << endl;
      break;
    case QtDebugMsg:
      out << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss:zzz")
          << " Debug: " << msg << ",     " << context.file << "     "
          << context.function << endl;
      break;
    case QtWarningMsg:
      out << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss:zzz")
          << " Warning: " << msg << ",      " << context.file << endl;
      break;
    case QtCriticalMsg:
      out << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss:zzz")
          << " Critical: " << msg << ",  " << context.file << endl;
      break;
    case QtFatalMsg:
      out << QDateTime::currentDateTime().toString("dd.MM.yy hh:mm:ss:zzz")
          << " Fatal: " << msg << ",     " << context.file << endl;
      break;
  }
  file.close();
}
