//
// Created by jay on 12/08/2026.
//

#include "Launcher.h"

#include "minecraft/exec/Game.h"
#include "minecraft/exec/JavaVirtualMachine.h"
#include "minecraft/ver/ClientJson.h"
#include "minecraft/ver/VersionManifest.h"

Launcher* Launcher::s_instance {nullptr};
QUrl Launcher::s_gameDirectory;
QUrl Launcher::s_javaExecutable {QUrl::fromLocalFile(FileSystem::which("java"))}; // stores properly
QString Launcher::s_username;

Launcher::Launcher(QObject *parent)
  : QObject(parent) {
  connect(this, &Launcher::usernameChanged,
      this, &Launcher::userMessageChanged);

  connect(Authentication::getInstance(), &Authentication::authStateChanged,
    this, &Launcher::userMessageChanged);

  connect(Profiles::getInstance(), &Profiles::currentProfileIdChanged,
    this, &Launcher::userMessageChanged);

  connect(Profiles::getInstance(), &Profiles::profilesChanged,
    this, &Launcher::userMessageChanged);

  connect(VersionManifest::getInstance(), &VersionManifest::stateChanged,
  this, &Launcher::userMessageChanged);

  if (!s_instance) {
    s_instance = this;
  }
}

QUrl Launcher::getJavaExecutable() {
  if (s_javaExecutable.isEmpty() or !s_javaExecutable.isValid()) {
    throw std::runtime_error("Java path could not be found. Are you sure Java is installed?");
  }

  return s_javaExecutable;
}

QString Launcher::userMessage() {
  qDebug() << "Updating user message...";
  bool authenticated {Authentication::getAuthState() == Authentication::AuthState::AUTHENTICATED};
  bool manifestDownloaded {VersionManifest::getManifestState() == VersionManifest::ManifestState::PRESENT};

  QString version {
    ProfileManager::getProfile(
      Profiles::getCurrentProfileId()
    )->getLastVersionId()
  };
  if (version == "latest-release") {
    version = VersionManifest::getLatestVersions().release;
  }
  else if (version == "latest-snapshot") {
    version = VersionManifest::getLatestVersions().snapshot;
  }

  QString username {getUsername()};

  QString message {QString("Welcome, <b>%1</b>").arg(username)};

  if (!authenticated) {
    message.append("! Please log in.");
  }

  message.append("<br>");

  if (manifestDownloaded and authenticated) {
    const QString DOWNLOAD_MESSAGE {Versions::isDownloaded(version) ? "" : "download & "};

    const QString READY_MESSAGE {"Ready to %1play Minecraft %2"};

    message.append(READY_MESSAGE.arg(DOWNLOAD_MESSAGE, version));
  }
  else {
    message.append("Loading versions...");
  }

  return message;
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
  OperatingSystem user {SystemInfo::getOperatingSystem()};

  QString rootPath;
  if (user.name == SystemName::WINDOWS) {
    rootPath = {QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)};
  }
  else if (user.name == SystemName::LINUX) {
    rootPath = {QStandardPaths::writableLocation(QStandardPaths::HomeLocation)};
  }
  else {
    throw std::runtime_error("Unknown platform.");
  }
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

void Launcher::setUsername(const QString& username) {
  if (s_username == username) { return; }
  qDebug() << "Setting username to:" << username;
  s_username = username;
  emit s_instance->usernameChanged();
}

QString Launcher::getUsername() {
  if (s_username.isEmpty() or s_username.isNull()) {
    return "guest";
  }

  return s_username;
}

QString Launcher::getTime(bool def) {
  if (def) { return "1970-01-01T00:00:00.000Z"; }

  QDateTime const DATE_TIME = QDateTime::currentDateTimeUtc();
  return DATE_TIME.toString(Qt::ISODateWithMs);
}

Launcher* Launcher::getInstance() {
  if (!s_instance) {
    s_instance = new Launcher();
  }
  return s_instance;
}

void Launcher::play() {
  const auto profile {
    ProfileManager::getProfile(
      Profiles::getCurrentProfileId()
    )
  };

  Game::launch(profile);
}