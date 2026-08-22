//
// Created by jay on 13/08/2026.
//

#include "System.h"

#include "sys/info/SystemInfo.h"
#include "sys/io/FileSystem.h"

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
  return write(path, content.toUtf8());
}

bool System::write(const QString& path, const QByteArray& content) {
  QFile file {path};
  if (!FileSystem::isDirectory(FileSystem::getParentDirectory(path))) {
    FileSystem::makePath(path);
  }

  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Failed to write" << path << "!";
    return false;
  }

  file.write(content);
  file.close();

  qDebug() << "Successfully wrote to" << path << "!";
  return true;
}

QString System::read(const QString& path) {
  return cat(path);
}

QByteArray System::cat(const QString& path) {
  if (!FileSystem::isFile(path)) {
    qDebug() << "The file" << path << "does not exist!";
    return {};
  }

  QFile file {path};
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to read from" << path << "!";
    return {};
  }

  qDebug() << "Successfully read from" << path << "!";

  return file.readAll();
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