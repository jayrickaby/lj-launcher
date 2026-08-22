//
// Created by jay on 21/08/2026.
//

#include "AssetIndex.h"

AssetIndex::AssetIndex(const QVariantMap& data) {
  const QString INDEX_ID {data.value("id").toString()};
  const QString FILE_NAME {INDEX_ID + ".json"};

  m_downloadItem = DownloadItem {
    .hash = data.value("sha1").toString(),
    .id = INDEX_ID,
    .name = "",
    .path = FileSystem::joinPaths({getAssetsPath(), "indexes", FILE_NAME}),
    .size = data.value("size").toULongLong(),
    .totalSize = data.value("totalSize").toULongLong(),
    .url = data.value("url").toString()
  };
}

void AssetIndex::refreshIndex() {
  const QVariantMap DATA {JsonUtils::readJson(m_downloadItem.path).toVariantMap()};
  const QVariantMap OBJECTS (DATA.value("objects").toMap());

  m_objects.clear();
  for (const auto& filePath : OBJECTS.keys()) {
    const QVariantMap OBJECT {OBJECTS.value(filePath).toMap()};
    const QString HASH {OBJECT.value("hash").toString()};
    const quint64 SIZE {OBJECT.value("size").toULongLong()};

    // Skip non-existent just in case
    if (filePath.isEmpty() or HASH.isEmpty() or SIZE == 0) {
      continue;
    }

    // File contains whole hash, parent folder contains first two digits of same hash
    // e
    const QString BASE {
      FileSystem::joinPaths({HASH.left(2), HASH})
    };

    const QString URL {
      FileSystem::joinPaths({ASSETS_URL, BASE})
    };
    const QString PATH {
      FileSystem::joinPaths({ASSETS_PATH, "objects", BASE})
    };

    m_objects.append(
      DownloadItem {
      .hash = HASH,
      .id = "",
      .name = filePath,
      .path = PATH,
      .size = SIZE,
      .totalSize = 0,
      .url = URL
      }
    );
  }
}

QString AssetIndex::getAssetsPath() { return ASSETS_PATH; }
QString AssetIndex::getAssetsUrl() { return ASSETS_URL; }

QList<DownloadItem> AssetIndex::getObjects() const { return m_objects; }