//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_GAME_H_
#define LJ_LAUNCHER_GAME_H_
#include "minecraft/ver/ClientJson.h"


class Game : public QObject {
  Q_OBJECT
  Q_PROPERTY(GameState state MEMBER s_state NOTIFY stateChanged);
signals:
  void stateChanged();

public:
  explicit Game(QObject* parent = nullptr);
  static void launch(const ManifestEntry& version);
  static Game* getInstance();

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
  void refreshState();

private:
  static void prepareAssets(const ManifestEntry& version);
  static void setState(const GameState& state);

  static ClientJson* s_json;
  static Game* s_instance;
  static GameState s_state;
};

#endif  // LJ_LAUNCHER_GAME_H_
