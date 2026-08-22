//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_GAME_H_
#define LJ_LAUNCHER_GAME_H_
#include "minecraft/ver/ClientJson.h"

class Game {

public:
  static void createInstance(const ManifestEntry& version);
  static Game* getInstance();

private:
  Game(const ManifestEntry& version);

  ClientJson* m_json;
  static Game* s_instance;
};

#endif  // LJ_LAUNCHER_GAME_H_
