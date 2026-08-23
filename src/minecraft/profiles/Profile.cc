//
// Created by jay on 23/08/2026.
//

#include "Profile.h"

#include <QJsonObject>

#include "Launcher.h"

Profile::Profile(QObject* parent)
  : QObject(parent)
{}

Profile::Profile(const QJsonObject& data, QObject *parent)
  : Profile(parent){
  copy(data);
}

Profile::Profile(const QVariantMap& data, QObject *parent)
  : Profile(parent){
  copy(data);
}

void Profile::copy(const QJsonObject& data) {
  copy(data.toVariantMap());
}

void Profile::copy(const QVariantMap& data) {
  if (data.contains("name")) {
    name = data.value("name").toString();
  }

  if (data.contains("type")) {
    // can't mix and match latest type with random version
    QString dataType {data.value("type").toString()};
    if (dataType == "latest-release") {
      type = ProfileType::LATEST_RELEASE;
      lastVersionId = std::move(dataType);
    }
    else if (dataType == "latest-snapshot") {
      type = ProfileType::LATEST_SNAPSHOT;
      lastVersionId = std::move(dataType);
    }
    else {
      type = ProfileType::CUSTOM;
      lastVersionId = data.value("lastVersionId").toString();
    }
  }

  if (data.contains("created")) {
    created = data.value("created").toString();
  }

  if (data.contains("lastUsed")) {
    lastUsed = data.value("lastUsed").toString();
  }

  if (data.contains("icon")) {
    icon = data.value("icon").toString();
  }

  if (data.contains("gameDir")) {
    gameDir = data.value("gameDir").toString();
  }

  if (data.contains("javaDir")) {
    javaDir = data.value("javaDir").toString();
  }

  if (data.contains("javaArgs")) {
    javaArgs = data.value("javaArgs").toString();
  }

  if (data.contains("resolution")) {
    QVariantMap dataResolution {data.value("resolution").toMap()};
    if (dataResolution.contains("width") and dataResolution.contains("height")) {
      resolution = Resolution {
        .width = dataResolution.value("width").toUInt(),
        .height = dataResolution.value("height").toUInt()
      };
    }
  }
}

QVariantMap Profile::toMap() {
  QVariantMap result;
  result["name"] = name;

  // can't mix and match latest type with random version
  switch (type) {
    case ProfileType::LATEST_SNAPSHOT:
    case ProfileType::LATEST_RELEASE: {
      result["type"] = lastVersionId;
      result["lastVersionId"] = lastVersionId;
    }
    case ProfileType::CUSTOM: {
      result["type"] = "custom";
      result["lastVersionId"] = lastVersionId;
    }
  }

  result["created"] = created;
  result["lastUsed"] = lastUsed;
  result["icon"] = icon;

  if (gameDir.has_value()) {
    result["gameDir"] = gameDir.value();
  }

  if (javaDir.has_value()) {
    result["javaDir"] = javaDir.value();
  }

  if (javaArgs.has_value()) {
    result["javaArgs"] = javaArgs.value();
  }

  if (resolution.has_value()) {
    result["resolution"] = QVariantMap({
        {"width", resolution.value().width},
        {"height", resolution.value().height},
      }
    );
  }
  return result;
}

QJsonObject Profile::toJson() {
  return QJsonObject::fromVariantMap(toMap());
}

QVariant Profile::getGameDir() {
  if (gameDir.has_value()) {
    return gameDir.value();
  }
  return {};
}

QVariant Profile::getJavaDir() {
  if (javaDir.has_value()) {
    return javaDir.value();
  }
  return {};
}

QVariant Profile::getJavaArgs() {
  if (javaArgs.has_value()) {
    return javaArgs.value();
  }
  return {};
}

QVariant Profile::getResolution() {
  if (resolution.has_value()) {
    return QVariant::fromValue(resolution.value());
  }
  return {};
}