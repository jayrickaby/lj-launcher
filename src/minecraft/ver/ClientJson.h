//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_CLIENTJSON_H_
#define LJ_LAUNCHER_CLIENTJSON_H_
#include <QVariant>

#include "VersionManifest.h"
#include "minecraft/assets/AssetIndex.h"
#include "minecraft/lib/LibraryIndex.h"
#include "sys/info/SystemInfo.h"

class ClientJson : public NetworkRequester {
  Q_OBJECT
public:
  explicit ClientJson(QObject *parent = nullptr);
  explicit ClientJson(const ManifestEntry& manifestEntry, QObject *parent = nullptr);

  void requestJar();
  void requestJson();

  void refreshJson();

  void onNetworkReply(QNetworkReply* reply) override;

private:
  DownloadItem parseClientJar(const QVariantMap& data);
  bool isJarDownloaded();

  AssetIndex* m_assetIndex;
  DownloadItem m_clientJson;
  DownloadItem m_clientJar;
  LibraryIndex* m_libraryIndex;

  QString m_id;
  QString m_mainClass;
  QString m_releaseTime;
  QString m_time;
  VersionType m_type;

  const QString CLIENT_JSON_URL;
  const QString CLIENT_PATH;
};

#endif  // LJ_LAUNCHER_CLIENTJSON_H_
