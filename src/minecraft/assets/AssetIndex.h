//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_ASSETINDEX_H_
#define LJ_LAUNCHER_ASSETINDEX_H_
#include <QList>
#include <QString>
#include <QVariantMap>

#include "Launcher.h"
#include "sys/io/FileSystem.h"
#include "sys/io/JsonUtils.h"

class AssetIndex {
public:
  AssetIndex(const QVariantMap& data);

  [[nodiscard]] static QString getAssetsPath();
  [[nodiscard]] static QString getAssetsUrl();
  [[nodiscard]] QList<DownloadItem> getObjects() const;

  void refreshIndex();

private:
  QList<DownloadItem> m_objects;

  DownloadItem m_downloadItem {};

  inline static const QString ASSETS_PATH {FileSystem::joinPaths({Launcher::getGameDirectory().toLocalFile(), "assets"})};
  inline static const QString ASSETS_URL {"https://resources.download.minecraft.net/"};
};

#endif  // LJ_LAUNCHER_ASSETINDEX_H_
