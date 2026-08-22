//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_CLIENTJSON_H_
#define LJ_LAUNCHER_CLIENTJSON_H_
#include <QVariant>

#include "minecraft/assets/AssetIndex.h"

class ClientJson {
public:
  ClientJson(const QString& path);

  void requestJar();
  void requestLibraries();
  void requestAssets();

private:
  DownloadItem parseClientJar(const QVariantMap& data);

  AssetIndex m_assetIndex;
  DownloadItem m_clientJar;
  LibraryIndex m_libraryIndex;

  QString m_id;
  QString m_mainClass;
  QString m_releaseTime;
  QString m_time;
  VersionType m_type;

  const QString CLIENT_PATH;
};

#endif  // LJ_LAUNCHER_CLIENTJSON_H_
