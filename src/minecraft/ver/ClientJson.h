//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_CLIENTJSON_H_
#define LJ_LAUNCHER_CLIENTJSON_H_
#include <QVariant>

#include "Argument.h"
#include "VersionManifest.h"
#include "minecraft/assets/AssetIndex.h"
#include "minecraft/lib/LibraryIndex.h"
#include "sys/info/SystemInfo.h"

enum class ClientState {
  UNINITIALISED,
  PREPARING,
  DOWNLOADING_JSON,
  DOWNLOADED_JSON,
  DOWNLOADING_OTHERS,
  DOWNLOADED_OTHERS,
  INITIALISED
};
struct ArgumentBearer {
  QStringList unconditionalArguments;
  QList<Argument> conditionalArguments;
};

class ClientJson : public NetworkRequester {
  Q_OBJECT

signals:
  void stateChanged();
  void receivedNetworkReply();

public:
  explicit ClientJson(QObject *parent = nullptr);
  explicit ClientJson(const ManifestEntry& manifestEntry, QObject *parent = nullptr);

  ClientState getState();

  void requestJar();
  void requestJson();

  void refreshJson();

  void onNetworkReply(QNetworkReply* reply) override;

  static Action parseAction(const QString& rawAction);
  static Argument parseArgument(const QVariantMap& rawArgument);
  static OperatingSystem parseOs(const QVariantMap& rawOs);
  static Rule parseRule(const QVariantMap& rawRule);

private slots:
  void refreshState();

private:
  DownloadItem parseClientJar(const QVariantMap& data);
  bool isJarDownloaded();

  void setState(const ClientState& state);

  AssetIndex* m_assetIndex {nullptr};

  bool m_assetsReady {false};
  bool m_clientJarReady {false};
  bool m_clientJsonReady {false};
  bool m_libraryReady {false};

  ClientState m_state {ClientState::UNINITIALISED};

  ArgumentBearer defaultJvmArguments;
  ArgumentBearer jvmArguments;
  ArgumentBearer gameArguments;

  DownloadItem m_clientJson {};
  DownloadItem m_clientJar {};
  LibraryIndex* m_libraryIndex {nullptr};

  QString m_id {""};
  QString m_mainClass {""};
  QString m_releaseTime {""};
  QString m_time {""};

  VersionType m_type;

  const QString CLIENT_JSON_URL;
  const QString CLIENT_PATH;
};

#endif  // LJ_LAUNCHER_CLIENTJSON_H_
