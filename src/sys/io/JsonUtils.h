//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_JSONUTILS_H_
#define LJ_LAUNCHER_JSONUTILS_H_
#include <QJsonObject>

#include "FileSystem.h"

class JsonUtils {
public:
  [[nodiscard]] static QJsonObject readJson(const QString& path);
  static bool writeJson(const QString& path, const QVariantMap& contents);
  static bool writeJson(const QString& path, const QJsonObject& contents);
};

#endif  // LJ_LAUNCHER_JSONUTILS_H_
