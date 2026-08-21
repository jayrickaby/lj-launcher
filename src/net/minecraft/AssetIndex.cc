//
// Created by jay on 21/08/2026.
//

#include "AssetIndex.h"

AssetIndex::AssetIndex(QString const& path) {
  const QVariantMap DATA {Json::readJson(path).toVariantMap()};
  const QVariantMap OBJECTS (DATA.value("objects").toMap());

  for (const auto& filePath : OBJECTS.keys()) {
    const QVariantMap OBJECT {OBJECTS.value(filePath).toMap()};
    const QString HASH {OBJECT.value("hash").toString()};
    const quint64 SIZE {OBJECT.value("size").toULongLong()};

    // Skip non-existent just in case
    if (filePath.isEmpty() and HASH.isEmpty() and SIZE == 0) {
      continue;
    }

    m_objects.append(Asset(filePath, HASH, SIZE));
  }
}

QString AssetIndex::getAssetsPath() { return ASSETS_PATH; }
QString AssetIndex::getAssetsUrl() { return ASSETS_URL; }

QList<AssetIndex::Asset> AssetIndex::getObjects() const { return m_objects; }

QString AssetIndex::findAssetsPath() {
  return Launcher::getGameDirectory().toLocalFile() + "/assets/";
}