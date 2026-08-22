//
// Created by jay on 22/08/2026.
//

#include "Game.h"

Game* Game::s_instance {nullptr};
Game::GameState Game::s_state {GameState::UNINITIALISED};
ClientJson* Game::s_json {nullptr};

Game::Game(QObject* parent)
: QObject(parent) {
  if (!s_instance) {
    s_instance = this;
  }
};

void Game::launch(const ManifestEntry& version) {
  if (s_state != GameState::UNINITIALISED) {
    qDebug() << "There is already a game instance running!";
    return;
  }
  setState(GameState::PREPARING);
  prepareAssets(version);
}

void Game::prepareAssets(const ManifestEntry& version) {
  s_json = new ClientJson(version);

  connect(s_json, &ClientJson::stateChanged,
    getInstance(), &Game::refreshState);
}

void Game::refreshState() {
  if (!s_json) {
    return;
  }

  switch (s_json->getState()) {
    case ClientState::DOWNLOADING_JSON:
    case ClientState::DOWNLOADING_OTHERS: {
      setState(GameState::DOWNLOADING);
      break;
    }

    case ClientState::INITIALISED: {
      setState(GameState::DOWNLOADED);
      break;
    }
    default:;
  }
}

void Game::setState(const GameState& state) {
  if (s_state != state) {
    s_state = state;
    emit getInstance()->stateChanged();
  }
}

Game* Game::getInstance() {
  if (!s_instance) {
    s_instance = new Game();
  }
  return s_instance;
}