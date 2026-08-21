//
// Created by jay on 13/08/2026.
//

#include "System.h"

bool System::touch(const QString& path, bool existsOk) {
  QFileInfo const FILE_INFO {path};
  if (FILE_INFO.exists()) {
    qDebug() << "The file" << path << "already exists!";
    return existsOk;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Failed to create" << path << "!";
    return false;
  }

  // Close early as only creating the file
  file.close();
  qDebug() << "Successfully created" << path << "!";
  return true;
}

bool System::write(const QString& path, const QString& content) {
  QFileInfo const FILE_INFO {path}; 
  if (!FILE_INFO.exists()) {
    qDebug() << "The file" << path << "does not exist! Attempted to write:" << content;
    return false;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Failed to write" << path << "!";
    return false;
  }

  QTextStream stream {&file};
  stream << content;
  file.close();

  qDebug() << "Successfully wrote to" << path << "!";
  return true;
}

QString System::read(const QString& path) {
  QFileInfo const FILE_INFO {path};
  if (!FILE_INFO.exists()) {
    qDebug() << "The file" << path << "does not exist!";
    return {};
  }

  QFile file {path};
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to read from" << path << "!";
    return {};
  }

  QTextStream stream {&file};
  QString contents {stream.readAll()};
  file.close();

  qDebug() << "Successfully read from" << path << "!";
  return contents;
}

QString System::which(const QString& path) {
  QString foundPath {QStandardPaths::findExecutable(path)};

  if (foundPath.isEmpty()) {;
    qDebug() << "The executable" << path << "could not be found!";
    return {};
  }
  qDebug() << "Found" << path << "at:" << foundPath;

  return foundPath;
}

QByteArray System::getSha1Checksum(const QByteArray& data) {
  return QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
}