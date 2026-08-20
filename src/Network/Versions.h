//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONS_H_
#define LJ_LAUNCHER_VERSIONS_H_

#include <QDirIterator>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include "NetworkRequester.h"
#include "../Launcher.h"
#include "../System.h"


class Versions : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(ManifestState manifestState READ manifestState NOTIFY stateChanged)
  Q_PROPERTY(QVariantList versionsList READ versionsList NOTIFY versionsListChanged);
  QML_ELEMENT
  QML_SINGLETON

signals:
  void stateChanged();
  void versionsListChanged();

public:
  explicit Versions(QObject *parent = nullptr);

  enum class ManifestState {
    MISSING,      // Not Requested
    DOWNLOADING,  // Requested
    PRESENT       // Successfully Downloaded
  };
  Q_ENUM(ManifestState)

  ManifestState manifestState();
  QVariantList versionsList();

  void onNetworkReply(QNetworkReply* reply) override;

  static QString getLatestVersion(bool snapshot=false);
  static QList<QVariantMap> getAvailableVersions(bool snapshot=false, bool historical=false);
  static QList<QVariantMap> getDownloadedVersions();
  static QVariantMap getDownloadedVersion(const QString& versionId);
  static ManifestState getState();
  static bool isDownloaded(const QString& versionId);

  static Versions* getInstance();

private:
  static QUrl findVersionsPath();
  static QUrl findJsonPath();
  static QVariantMap getManifest();
  static void requestManifest();
  static void setState(const ManifestState& state);

  static inline const QUrl VERSIONS_PATH {findVersionsPath()};
  static inline const QUrl JSON_PATH {findJsonPath()};
  static inline const QUrl MANIFEST_URL {"https://launchermeta.mojang.com/mc/game/version_manifest_v2.json"};

  static ManifestState s_state;
  static Versions* s_instance;
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
