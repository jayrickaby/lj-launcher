//
// Created by jay on 22/08/2026.
//

#include "Game.h"

#include <qcoreapplication.h>

#include "JavaVirtualMachine.h"

Game* Game::s_instance {nullptr};
Game::GameState Game::s_state {GameState::UNINITIALISED};
ClientJson* Game::s_json {nullptr};
QSharedPointer<ProfileEntry> Game::s_profile {nullptr};
QList<Feature> Game::s_features {};

Game::Game(QObject* parent)
: QObject(parent) {
  if (!s_instance) {
    s_instance = this;
  }

  connect(this, &Game::assetsDownloaded, &Game::prepareExecutable);
};

void Game::launch(const QSharedPointer<ProfileEntry>& profile) {
  if (s_state != GameState::UNINITIALISED) {
    qDebug() << "There is already a game instance running!";
    return;
  }

  if (!profile) {
    qDebug() << "Game Profile is invalid!";
  }

  s_profile = profile;

  clearFeatures();

  auto resolutionData {profile->getResolution()};

  if (resolutionData.isValid() and !resolutionData.isNull()) {
    addFeature(Feature::HAS_CUSTOM_RESOLUTION);

    auto resolution {resolutionData.value<Resolution>()};

    JavaVirtualMachine::setVariable("resolution_width", QString::number(resolution.getWidth()));
    JavaVirtualMachine::setVariable("resolution_height", QString::number(resolution.getHeight()));
  }

  setState(GameState::PREPARING);

  QString profileVer{};
  switch (profile->getType()) {
    case ProfileEntry::ProfileType::LATEST_RELEASE:
      profileVer = VersionManifest::getLatestVersions().release;
      break;
    case ProfileEntry::ProfileType::LATEST_SNAPSHOT:
      profileVer = VersionManifest::getLatestVersions().snapshot;
      break;
    case ProfileEntry::ProfileType::CUSTOM:
      profileVer = profile->getLastVersionId();
  }
  // TODO: Generate and store to clientId_v2.txt and grab from there instead
  JavaVirtualMachine::setVariable("clientid", "3d99c47a1bfe4b50b651fb8429c3aad6");

  prepareAssets(VersionManifest::getVersion(s_profile->getLastVersionId()));
}

void Game::prepareAssets(const ManifestEntry& version) {
  JavaVirtualMachine::setVariable("version_name", version.item.id);
  JavaVirtualMachine::setVariable("version_type", Versions::convertFromVersionType(version.type));

  s_json = new ClientJson(version);

  connect(s_json, &ClientJson::stateChanged,
    getInstance(), &Game::refreshDownloadState);
}

void Game::prepareExecutable() {
  qDebug() << "Preparing executable...";

  QString gameDir = s_profile->getGameDir().isNull()
                  ? Launcher::getGameDirectory().toLocalFile()
                  : s_profile->getGameDir().toString();

  JavaVirtualMachine::setVariable("game_directory", gameDir);

  QString javaDir = s_profile->getJavaDir().isNull()
                  ? Launcher::getJavaExecutable().toLocalFile()
                  : s_profile->getJavaDir().toString();


  QStringList args {prepareArguments()};

  auto* process = new QProcess();

  process->setWorkingDirectory(gameDir);
  process->setProcessChannelMode(QProcess::MergedChannels);

  connect(process, &QProcess::readyReadStandardOutput,
    getInstance(), [process] {
      qDebug() << process->readAllStandardOutput();
    });

  setState(GameState::LAUNCHING);

  qDebug() << QStringList({javaDir, args.join(" ")}).join(" ");

  if (process->startDetached(javaDir, args)) {
    setState(GameState::LAUNCHED);
    QCoreApplication::quit();
  }
  setState(GameState::UNINITIALISED);
}

QStringList Game::prepareArguments() {
  QStringList arguments;

  if (s_profile->getJavaArgs().isNull()) {
    for (const auto& arg : s_json->getValidDefaultJvmArguments()) {
      arguments.append(arg);
    }
  }
  else {
    arguments.append(s_profile->getJavaArgs().toStringList());
  }

  arguments.append(s_json->getValidJvmArguments());
  arguments.append(s_json->getMinecraftClass());
  arguments.append(s_json->getValidGameArguments());

  return JavaVirtualMachine::resolveArguments(arguments);
}

void Game::refreshDownloadState() {
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
  if (s_state == state) {
    return;
  }

  s_state = state;
  emit getInstance()->stateChanged();

  if (state == GameState::DOWNLOADED) {
    emit getInstance()->assetsDownloaded();
  }
}

Game* Game::getInstance() {
  if (!s_instance) {
    s_instance = new Game();
  }
  return s_instance;
}

void Game::addFeature(const Feature& feature) {
  if (!s_features.contains(feature)) {
    s_features.append(feature);
  }
}

void Game::clearFeatures() {
  s_features.clear();
}