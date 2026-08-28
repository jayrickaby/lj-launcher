//
// Created by jay on 21/08/2026.
//

#include "FileSystem.h"


QString FileSystem::joinPaths(const QStringList& paths) {
  QString result {};
  for (const QString& path : paths) {
    if (result.isEmpty()) {
      result = path;
    } else {
      result = QDir(result).filePath(path);
    }
  }
  return QDir::cleanPath(result);
}

QString FileSystem::getParentDirectory(const QString& path) {
  return QFileInfo(
    QDir::cleanPath(path)
  ).absolutePath();
}

bool FileSystem::makePath(const QString& path) {
  return QDir().mkpath(path);
}

bool FileSystem::isDirectory(const QString& path) {
  return QDir(path).exists();
}

bool FileSystem::isFile(const QString& path) {
  const QFileInfo FILE {path};
  return FILE.exists() and FILE.isFile();
}

quint64 FileSystem::getFileSize(const QString& path) {
  if (!isFile(path)) {
    return {};
  }

  const QFileInfo FILE {path};

  return FILE.size();
}

quint64 FileSystem::getFolderSize(const QString& path) {
  quint64 totalSize {0};

  QDirIterator iterator(path, QDir::Files | QDir::Hidden | QDir::NoSymLinks , QDirIterator::Subdirectories);

  while (iterator.hasNext()) {
    iterator.next();
    totalSize += iterator.fileInfo().size();
  }

  return totalSize;
}

bool FileSystem::touch(const QString& path, bool existsOk) {
  if (isFile(path)) {
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

bool FileSystem::write(const QString& path, const QString& content) {
  return write(path, content.toUtf8());
}

bool FileSystem::write(const QString& path, const QByteArray& content) {
  QFile file {path};
  if (!isDirectory(getParentDirectory(path))) {
    makePath(path);
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

QString FileSystem::read(const QString& path) {
  return cat(path);
}

QByteArray FileSystem::cat(const QString& path) {
  if (!isFile(path)) {
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

QString FileSystem::which(const QString& path) {
  QString foundPath {QStandardPaths::findExecutable(path)};

  if (foundPath.isEmpty()) {;
    qDebug() << "The executable" << path << "could not be found!";
    return {};
  }
  qDebug() << "Found" << path << "at:" << foundPath;

  return foundPath;
}

QByteArray FileSystem::getSha1Checksum(const QByteArray& data) {
  return QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
}