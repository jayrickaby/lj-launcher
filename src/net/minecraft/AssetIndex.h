//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_ASSETINDEX_H_
#define LJ_LAUNCHER_ASSETINDEX_H_
#include <QList>
#include <QString>
#include <QVariantMap>

#include "Launcher.h"
#include "sys/Json.h"

class AssetIndex {
public:
  AssetIndex(QString const& path);

  struct Asset {
    QString path;
    QString hash;
    quint64 size;
  };

  [[nodiscard]] static QString getAssetsPath();
  [[nodiscard]] static QString getAssetsUrl();
  [[nodiscard]] QList<Asset> getObjects() const;

private:
  static QString findAssetsPath();

  QList<Asset> m_objects;

  inline static const QString ASSETS_PATH {findAssetsPath()};
  inline static const QString ASSETS_URL {"https://resources.download.minecraft.net/"};
};

#endif  // LJ_LAUNCHER_ASSETINDEX_H_
