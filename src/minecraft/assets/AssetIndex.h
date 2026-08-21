//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_ASSETINDEX_H_
#define LJ_LAUNCHER_ASSETINDEX_H_
#include <QList>
#include <QString>
#include <QVariantMap>

#include "IndexedAsset.h"
#include "Launcher.h"
#include "sys/io/JsonUtils.h"

class AssetIndex {
public:
  AssetIndex(QString const& path);

  [[nodiscard]] static QString getAssetsPath();
  [[nodiscard]] static QString getAssetsUrl();
  [[nodiscard]] QList<IndexedAsset> getObjects() const;

private:
  static QString findAssetsPath();

  QList<IndexedAsset> m_objects;

  inline static const QString ASSETS_PATH {findAssetsPath()};
  inline static const QString ASSETS_URL {"https://resources.download.minecraft.net/"};
};

#endif  // LJ_LAUNCHER_ASSETINDEX_H_
