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

enum class AssetIndexState {
  UNINITIALISED,
  DOWNLOADING_INDEX,
  DOWNLOADED_INDEX,
  DOWNLOADING_ASSETS,
  DOWNLOADED_ASSETS,
  INITIALISED
};

class AssetIndex : public NetworkRequester{
  Q_OBJECT

signals:
  void stateChanged();

public:
  AssetIndex(const QVariantMap& data, QObject *parent = nullptr);

  [[nodiscard]] static QString getAssetsPath();
  [[nodiscard]] static QString getAssetsUrl();
  [[nodiscard]] AssetIndexState getState() const;

  void requestAssets();
  void requestIndex();

  void onNetworkReply(QNetworkReply* reply) override;

  void refreshIndex();

private:
  QQueue<DownloadItem> m_objects;

  DownloadItem m_downloadItem {};
  AssetIndexState m_state{AssetIndexState::UNINITIALISED};

  quint64 expectedAssetReplies {0};

  void setState(const AssetIndexState& state);

  inline static const QString ASSETS_PATH {FileSystem::joinPaths({Launcher::getGameDirectory().toLocalFile(), "assets"})};
  inline static const QString ASSETS_URL {"https://resources.download.minecraft.net/"};
};

#endif  // LJ_LAUNCHER_ASSETINDEX_H_
