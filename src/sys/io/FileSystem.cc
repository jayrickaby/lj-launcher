//
// Created by jay on 21/08/2026.
//

#include "FileSystem.h"

QString FileSystem::joinPath(const QString& root, const QString& path) {
  return joinPath(root, QStringList{path});
}

QString FileSystem::joinPath(const QString& root, const QStringList& paths) {
  QString result {root};
  for (const QString& path : paths) {
    result += "/" + path;
  }
  return QDir::cleanPath(result);
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