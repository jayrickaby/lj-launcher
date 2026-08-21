//
// Created by jay on 21/08/2026.
//

#include "Json.h"


QJsonObject Json::readJson(const QString& path) {
  return QJsonDocument::fromJson(System::read(path).toUtf8()).object();
}