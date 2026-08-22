//
// Created by jay on 21/08/2026.
//

#include "FileSystem.h"

#include <QDirIterator>

QString FileSystem::joinPaths(const QStringList& paths) {
  QString result {};
  for (const QString& path : paths) {
    result += "/" + path;
  }
  return QDir::cleanPath(result);
}

QString FileSystem::getParentDirectory(const QString& path) {
  return QFileInfo(
    QDir::cleanPath(path)
  ).absolutePath();
}

bool FileSystem::makePath(const QString& path) {
  QFileInfo info(path);
  if (info.isFile() or !info.suffix().isEmpty()) {
    return QDir().mkpath(info.absolutePath());
  }

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