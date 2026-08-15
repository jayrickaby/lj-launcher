//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONS_H_
#define LJ_LAUNCHER_VERSIONS_H_

#include <QNetworkRequest>
#include <QString>
#include <QUrl>

#include "NetworkRequester.h"
#include "../Launcher.h"
#include "../System.h"


class Versions : public NetworkRequester {
  Q_OBJECT

public:
  enum class ManifestState {
    MISSING,      // Not Requested
    DOWNLOADING,  // Requested
    PRESENT       // Successfully Downloaded
  };


  explicit Versions(QObject *parent = nullptr);
  void onNetworkReply(QNetworkReply* reply) override;

  static QString getLatest(bool snapshot=false);
  static ManifestState getState();

  static Versions* getInstance();


signals:
  void stateChanged();

private:
  static QUrl findJsonPath();
  static void requestManifest();
  static void setState(const ManifestState& state);

  static inline const QUrl JSON_PATH {findJsonPath()};
  static inline const QUrl MANIFEST_URL {"https://launchermeta.mojang.com/mc/game/version_manifest_v2.json"};

  static ManifestState s_state;
  static Versions* s_instance;
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
