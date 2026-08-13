//
// Created by jay on 13/08/2026.
//

#include "System.h"

#include <qfile.h>

bool System::touch(const QString& path, bool existsOk) {
  QFileInfo const FILE_INFO {path};
  if (FILE_INFO.exists()) {
    QString const MSG {"The file %1 already exists!"};
    qDebug() << MSG.arg(path);
    return existsOk;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    QString const MSG {"Failed to create %1!"};
    qDebug() << MSG.arg(path);
    return false;
  }

  // Close early as only creating the file
  file.close();
  QString const MSG {"Successfully created %1!"};
  qDebug() << MSG.arg(path);
  return true;
}

bool System::write(const QString& path, const QString& content) {
  QFileInfo const FILE_INFO {path};
  if (!FILE_INFO.exists()) {
    QString const MSG {"The file %1 does not exist!"};
    qDebug() << MSG.arg(path);
    return false;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    QString const MSG {"Failed to write to %1!"};
    qDebug() << MSG.arg(path);
    return false;
  }

  QTextStream stream {&file};
  stream << content;

  QString const MSG {"Successfully wrote to %1!"};
  qDebug() << MSG.arg(path);
  return true;
}