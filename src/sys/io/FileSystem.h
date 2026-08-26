//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_FILESYSTEM_H_
#define LJ_LAUNCHER_FILESYSTEM_H_
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QStandardPaths>
#include <QString>
#include <QStringList>

class FileSystem {
public:
  [[nodiscard]] static QByteArray cat(const QString& path);

  [[nodiscard]] static bool isDirectory(const QString& path);
  [[nodiscard]] static bool isFile(const QString& path);

  [[nodiscard]] static QString getParentDirectory(const QString& path);
  [[nodiscard]] static quint64 getFileSize(const QString& path);
  [[nodiscard]] static quint64 getFolderSize(const QString& path);
  [[nodiscard]] static QByteArray getSha1Checksum(const QByteArray& data);

  [[nodiscard]] static QString joinPaths(const QStringList& paths);

  static bool makePath(const QString& path);

  [[nodiscard]] static QString read(const QString& path);

  static bool touch(const QString& path, bool existsOk=false);

  [[nodiscard]] static QString which(const QString& path);
  static bool write(const QString& path, const QString& content);
  static bool write(const QString& path, const QByteArray& content);

};

#endif  // LJ_LAUNCHER_FILESYSTEM_H_
