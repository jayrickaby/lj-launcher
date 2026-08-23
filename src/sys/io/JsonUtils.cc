//
// Created by jay on 21/08/2026.
//

#include "JsonUtils.h"

QJsonObject JsonUtils::readJson(const QString& path) {
  return QJsonDocument::fromJson(FileSystem::read(path).toUtf8()).object();
}