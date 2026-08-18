//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONS_H_
#define LJ_LAUNCHER_VERSIONS_H_

#include <QDirIterator>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#include "NetworkRequester.h"
#include "../Launcher.h"
#include "../System.h"


class Versions : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(ManifestState manifestState READ manifestState NOTIFY stateChanged)
  QML_ELEMENT
  QML_SINGLETON

public:
  enum class ManifestState {
    MISSING,      // Not Requested
    DOWNLOADING,  // Requested
    PRESENT       // Successfully Downloaded
  };
  Q_ENUM(ManifestState)

  explicit Versions(QObject *parent = nullptr);

  ManifestState manifestState();

  void onNetworkReply(QNetworkReply* reply) override;

  static QString getLatestVersion(bool snapshot=false);
  static QJsonObject getAvailableVersions(bool snapshot=false, bool historical=false);
  static QStringList getDownloadedVersions();
  static ManifestState getState();
  static bool isDownloaded(const QString& version);

  static Versions* getInstance();

signals:
  void stateChanged();

private:
  static QUrl findVersionsPath();
  static QUrl findJsonPath();
  static QJsonObject getManifest();
  static void requestManifest();
  static void setState(const ManifestState& state);

  static inline const QUrl VERSIONS_PATH {findVersionsPath()};
  static inline const QUrl JSON_PATH {findJsonPath()};
  static inline const QUrl MANIFEST_URL {"https://launchermeta.mojang.com/mc/game/version_manifest_v2.json"};

  static ManifestState s_state;
  static Versions* s_instance;
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
