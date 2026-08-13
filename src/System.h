//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_SYSTEM_H_
#define LJ_LAUNCHER_SYSTEM_H_
#include <QUrl>
#include <QDebug>
#include <QFileInfo>
#include <QIODevice>

class System {
public:
  [[nodiscard]] static bool touch(const QString& path, bool existsOk=false);
  [[nodiscard]] static bool write(const QString& path, const QString& content);
  [[nodiscard]] static QString read(const QString& path);
};

#endif  // LJ_LAUNCHER_SYSTEM_H_
