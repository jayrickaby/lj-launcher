//
// Created by jay on 21/08/2026.
//

#include "JsonUtils.h"

QJsonObject JsonUtils::readJson(const QString& path) {
  return QJsonDocument::fromJson(FileSystem::read(path).toUtf8()).object();
}

bool JsonUtils::writeJson(const QString& path,
                          const QJsonObject& contents) {
  QByteArray data = QJsonDocument(contents).toJson();
  return FileSystem::write(path, data);
}

bool JsonUtils::writeJson(const QString& path,
                          const QVariantMap& contents) {
  return writeJson(path, QJsonObject::fromVariantMap(contents));
}