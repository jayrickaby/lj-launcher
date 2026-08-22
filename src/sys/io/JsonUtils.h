//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_JSONUTILS_H_
#define LJ_LAUNCHER_JSONUTILS_H_
#include <QJsonObject>

#include "System.h"

class JsonUtils {
public:
  static QJsonObject readJson(const QString& path);
};

#endif  // LJ_LAUNCHER_JSONUTILS_H_
