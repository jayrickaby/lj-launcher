//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONMANIFEST_H_
#define LJ_LAUNCHER_VERSIONMANIFEST_H_

#include "sys/io/FileSystem.h"
#include "sys/io/JsonUtils.h"
#include "net/Downloader.h"
#include "net/DownloadItem.h"
#include "net/Versions.h"

struct ManifestEntry {
  DownloadItem item;
  VersionType type;
  QString time;
  QString releaseTime;
  int complianceLevel;
};

struct ManifestLatest {
  QString release;
  QString snapshot;
};


class VersionManifest: public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(ManifestState manifestState MEMBER s_manifestState NOTIFY stateChanged)

signals:
  void stateChanged();

public:
  enum class ManifestState {
    MISSING,      // Not Requested
    DOWNLOADING,  // Requested
    PRESENT       // Successfully/Already Downloaded
  };
  Q_ENUM(ManifestState)

  explicit VersionManifest(QObject *parent = nullptr);
  void onNetworkReply(QNetworkReply* reply) override;

  static void refreshManifest();

  static VersionManifest* getInstance();
  static ManifestLatest getLatestVersions();
  static ManifestState getManifestState();
  static ManifestEntry getVersion(QString versionId);
  static QList<ManifestEntry> getVersions(const QList<VersionType>& types);

  static void requestManifest();
  static void setState(const ManifestState& state);

private:
  static ManifestLatest parseLatestVersions(const QVariantMap& latestData);
  static ManifestEntry parseManifestEntry(const QVariantMap& entryData);

  static ManifestLatest s_latestVersions;
  static QList<ManifestEntry> s_versions;

  static VersionManifest* s_instance;
  static inline ManifestState s_manifestState {ManifestState::MISSING};
  static inline const QString MANIFEST_PATH {FileSystem::joinPaths({Versions::getVersionsPath(), "version_manifest_v2.json"})};
  static inline const QString MANIFEST_URL {"https://launchermeta.mojang.com/mc/game/version_manifest_v2.json"};
};

#endif  // LJ_LAUNCHER_VERSIONMANIFEST_H_
