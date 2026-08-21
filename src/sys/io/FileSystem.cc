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
  return QDir(path).mkpath(".");
}