//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_SYSTEM_H_
#define LJ_LAUNCHER_SYSTEM_H_
#include <qfile.h>

#include <QCryptographicHash>
#include <QDebug>
#include <QFileInfo>
#include <QIODevice>
#include <QStandardPaths>
#include <QUrl>

#include "sys/info/OperatingSystem.h"

class System {
public:
  // Files
  [[nodiscard]] static QString read(const QString& path);
  [[nodiscard]] static QByteArray cat(const QString& path);

  static bool touch(const QString& path, bool existsOk=false);
  [[nodiscard]] static QString which(const QString& path);

  static bool write(const QString& path, const QString& content);
  static bool write(const QString& path, const QByteArray& content);

  [[nodiscard]] static QByteArray getSha1Checksum(const QByteArray& data);
};

#endif  // LJ_LAUNCHER_SYSTEM_H_
