//
// Created by jay on 22/08/2026.
//

#include "Game.h"

Game* Game::s_instance {nullptr};

Game::Game(const ManifestEntry& version)
: m_json(new ClientJson(version)) {
  if (!s_instance) {
    s_instance = this;
  }
};

void Game::createInstance(const ManifestEntry& version) {
  if (s_instance) {
    qDebug() << "There is already a game instance!";
    return;
  }

  s_instance = new Game(version);
}

Game* Game::getInstance() {
  return s_instance;
}