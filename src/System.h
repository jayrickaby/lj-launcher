//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_SYSTEM_H_
#define LJ_LAUNCHER_SYSTEM_H_
#include <qfile.h>

#include <QCryptographicHash>
#include <QStandardPaths>
#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QIODevice>

class System {
public:
  // Files
  [[nodiscard]] static QString read(const QString& path);
  static bool touch(const QString& path, bool existsOk=false);
  [[nodiscard]] static QString which(const QString& path);
  static bool write(const QString& path, const QString& content);
  [[nodiscard]] static QByteArray getSha1Checksum(const QByteArray& data);

  // System
  [[nodiscard]] static QString getArchitecture();
  [[nodiscard]] static QString getOs();
  [[nodiscard]] static QString getOsVersion();

};

#endif  // LJ_LAUNCHER_SYSTEM_H_
