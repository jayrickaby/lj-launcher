//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_GAME_H_
#define LJ_LAUNCHER_GAME_H_

#include <QProcess>

#include "minecraft/ver/ClientJson.h"


class Game : public QObject {
  Q_OBJECT
  Q_PROPERTY(GameState state MEMBER s_state NOTIFY stateChanged);
signals:
  void stateChanged();
  void assetsDownloaded();

public:
  explicit Game(QObject* parent = nullptr);
  static void launch(const QSharedPointer<ProfileEntry>& profile);
  static Game* getInstance();

  static void addFeature(const Feature& feature);
  static QList<Feature> getFeatures() {return s_features;};
  static void clearFeatures();

  enum class GameState {
    UNINITIALISED,  // simple instance
    PREPARING,      // waiting until reaching a point of download
    DOWNLOADING,    // client json + others downloading
    DOWNLOADED,     // above downloaded and present
    LAUNCHING,      // game itself launching
    LAUNCHED        // boom
  };
  Q_ENUM(GameState);

private slots:
  void refreshDownloadState();

private:
  static void prepareAssets(const ManifestEntry& version);
  static QStringList prepareArguments();
  static void prepareExecutable();
  static void setState(const GameState& state);

  static ClientJson* s_json;
  static QSharedPointer<ProfileEntry> s_profile;
  static Game* s_instance;
  static GameState s_state;
  static QList<Feature> s_features;
};

#endif  // LJ_LAUNCHER_GAME_H_
