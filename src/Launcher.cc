//
// Created by jay on 12/08/2026.
//

#include "Launcher.h"

Launcher* Launcher::s_instance = nullptr;
QUrl Launcher::s_gameDirectory;

Launcher::Launcher(QObject *parent)
  : QObject(parent) {
  s_instance = this;
}

void Launcher::sendError(ErrorMessage& message) {
  if (!s_instance) {
    return;
  }
  if (message.errorFriendly.isEmpty()) {
    message.errorFriendly = "Oops, an error has occurred!";
  }
  emit s_instance->launcherError(message);
}

QUrl Launcher::findGameDirectory() {
  QString rootPath;
#ifdef _WIN32
  rootPath = {QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)};
#elifdef __linux__
  rootPath = {QStandardPaths::writableLocation(QStandardPaths::HomeLocation)};
#else
  throw std::runtime_error("Unknown platform.");
#endif
  QString const FULL_PATH {QDir(rootPath).filePath(".minecraft")};

  QDir const GAME_DIR(FULL_PATH);
  if (!GAME_DIR.exists()) {
    GAME_DIR.mkpath(".");
    qDebug() << "Created game directory.";
  }
  qDebug() << "Found game directory: " << GAME_DIR.absolutePath();

  QUrl const GAME_URL = QUrl::fromLocalFile(FULL_PATH);
  return GAME_URL;
};

QUrl Launcher::getGameDirectory() {
  if (s_gameDirectory.isEmpty() or !s_gameDirectory.isValid()) {
    s_gameDirectory = findGameDirectory();
  }
  return s_gameDirectory;
}