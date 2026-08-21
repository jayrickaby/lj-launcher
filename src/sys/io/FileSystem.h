//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_FILESYSTEM_H_
#define LJ_LAUNCHER_FILESYSTEM_H_
#include <QDir>
#include <QString>
#include <QStringList>

class FileSystem {
public:
  [[nodiscard]] static bool isDirectory(const QString& path);
  [[nodiscard]] static bool isFile(const QString& path);

  [[nodiscard]] static quint64 getFileSize(const QString& path);

  [[nodiscard]] static QString joinPath(const QString& root, const QString& path);
  [[nodiscard]] static QString joinPath(const QString& root, const QStringList& paths);
  static bool makePath(const QString& path);
};

#endif  // LJ_LAUNCHER_FILESYSTEM_H_
