//
// Created by jay on 22/08/2026.
//

#include "ClientJson.h"

#include "minecraft/exec/JavaVirtualMachine.h"

ClientJson::ClientJson(QObject* parent)
  : NetworkRequester(parent)
{}

ClientJson::ClientJson(const ManifestEntry& manifestEntry, QObject* parent)
: NetworkRequester(parent),
  CLIENT_PATH(FileSystem::getParentDirectory(manifestEntry.item.path)),
  m_clientJson(manifestEntry.item),
  m_id(manifestEntry.item.id),
  m_time(manifestEntry.time),
  m_releaseTime(manifestEntry.releaseTime),
  m_type(manifestEntry.type){

  connect(this, &ClientJson::receivedNetworkReply, &ClientJson::refreshState);
  requestJson();
}

void ClientJson::refreshJson() {
  setState(ClientState::PREPARING);

  QString nativesPath {FileSystem::joinPaths({CLIENT_PATH, "natives"})};
  FileSystem::makePath(nativesPath);
  JavaVirtualMachine::setVariable("natives_directory", nativesPath);

  const QVariantMap MAP {JsonUtils::readJson(m_clientJson.path).toVariantMap()};

  // Installed CompleteVersion{id='x'...}
  Launcher::addLog(
    QString("Installed CompleteVersion%1")
    .arg(FileSystem::cat(m_clientJson.path))
  );

  QVariantMap arguments {MAP.value("arguments").toMap()};
  for (const auto& rawDefaultJvmArg : arguments.value("default-user-jvm").toList()) {
    if (rawDefaultJvmArg.metaType().id() == QMetaType::QString) {
      defaultJvmArguments.unconditionalArguments.append(rawDefaultJvmArg.toString());
    }
    else {
      defaultJvmArguments.conditionalArguments.append(parseArgument(rawDefaultJvmArg.toMap()));
    }
  }

  for (const auto& rawGameArg : arguments.value("game").toList()) {
    if (rawGameArg.metaType().id() == QMetaType::QString) {
      gameArguments.unconditionalArguments.append(rawGameArg.toString());
    }
    else {
      gameArguments.conditionalArguments.append(parseArgument(rawGameArg.toMap()));
    }
  }

  for (const auto& rawJvmArg : arguments.value("jvm").toList()) {
    if (rawJvmArg.metaType().id() == QMetaType::QString) {
      jvmArguments.unconditionalArguments.append(rawJvmArg.toString());
    }
    else {
      jvmArguments.conditionalArguments.append(parseArgument(rawJvmArg.toMap()));
    }
  }

  m_assetIndex = new AssetIndex(MAP.value("assetIndex").toMap());
  m_libraryIndex = new LibraryIndex(MAP.value("libraries").toList());

  connect(m_assetIndex, &AssetIndex::stateChanged, this, &ClientJson::refreshState);
  connect(m_libraryIndex, &LibraryIndex::stateChanged, this, &ClientJson::refreshState);

  m_mainClass = MAP.value("mainClass").toString();

  m_clientJar = (
    parseClientJar(
    MAP
      .value("downloads").toMap()
      .value("client").toMap()
    )
  );
  JavaVirtualMachine::appendVariable("classpath", m_clientJar.path, ":");
}

void ClientJson::requestJson() {
  qDebug() << "Requesting client.json file...";
  Launcher::addLog("Queueing library & version downloads");

  setState(ClientState::DOWNLOADING_JSON);
  Downloader::addDownload(this, m_clientJson);
}

void ClientJson::requestJar() {
  if (m_clientJar.isDownloaded()) {
    qDebug() << "Skipped client.jar as it was already downloaded!";
    return;
  }

  qDebug() << "Requesting client.jar file...";

  // Don't add this as requester or else it will be an infinite loop of refresh -> request
  setState(ClientState::DOWNLOADING_OTHERS);
  Downloader::addDownload(m_clientJar);
}

void ClientJson::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();

  emit receivedNetworkReply();

  refreshJson();
  requestJar();
  m_libraryIndex->requestLibraries();
  m_assetIndex->requestIndex();
}

DownloadItem ClientJson::parseClientJar(const QVariantMap& data) {
  const QString JAR_NAME {m_id + ".jar"};

  return DownloadItem{
    .hash = data.value("sha1").toString(),
    .id = "",
    .name = "",
    .path = FileSystem::joinPaths({CLIENT_PATH, JAR_NAME}),
    .size = data.value("size").toULongLong(),
    .totalSize = 0,
    .url = data.value("url").toString()
  };
}

ClientState ClientJson::getState() {
  return m_state;
}

void ClientJson::setState(const ClientState& state) {
  if (m_state != state) {
    m_state = state;
    emit stateChanged();
  }
}

void ClientJson::refreshState() {
  if (m_assetIndex) {
    m_assetsReady = m_assetIndex->getState() == AssetIndexState::INITIALISED;

    if (m_assetIndex->getState() == AssetIndexState::DOWNLOADING_INDEX
    or m_assetIndex->getState() == AssetIndexState::DOWNLOADING_ASSETS) {
      setState(ClientState::DOWNLOADING_OTHERS);
    }
  }

  if (m_libraryIndex) {
    m_libraryReady = m_libraryIndex->getState() == LibraryIndexState::INITIALISED;
    if (m_libraryIndex->getState() == LibraryIndexState::DOWNLOADING) {
      setState(ClientState::DOWNLOADING_OTHERS);
    }
  }

  m_clientJarReady = m_clientJar.isDownloaded();
  m_clientJsonReady = m_clientJson.isDownloaded();

  if (m_assetsReady and m_libraryReady and m_clientJarReady and m_clientJsonReady) {
    setState(ClientState::DOWNLOADED_OTHERS);
    setState(ClientState::INITIALISED);
  }
}

  Argument ClientJson::parseArgument(const QVariantMap& rawArgument) {
    Argument argument;

    for (const auto& rawRule : rawArgument["rules"].toList()) {
      if (!rawRule.isValid() or rawRule.isNull() or rawRule.toMap().isEmpty()) {
        continue;
      }

      argument.rules.append(parseRule(rawRule.toMap()));
    }

    const auto rawValue {rawArgument.value("value")};

    // Ensures strings aren't split
    if (rawValue.metaType().id() == QMetaType::QString) {
      auto value = rawValue.toString();
      if (!value.isEmpty()) {
        argument.values.append(value);
      }
    }
    else if (rawValue.canConvert<QVariantList>()) {
      for (const auto& rawValue : rawValue.toList()) {
        if (rawValue.isValid() and !rawValue.isNull() and !rawValue.toString().isEmpty()) {
          argument.values.append(rawValue.toString());
        }
      }
    }

    return argument;
  }

Feature ClientJson::parseFeature(const QVariantMap& rawFeature) {
  if (rawFeature.value("is_demo_user").toBool()) {
    return Feature::IS_DEMO_USER;
  }
  if (rawFeature.value("has_custom_resolution").toBool()) {
    return Feature::HAS_CUSTOM_RESOLUTION;
  }
  if (rawFeature.value("has_quick_plays_support").toBool()) {
    return Feature::HAS_QUICK_PLAYS_SUPPORT;
  }
  if (rawFeature.value("is_quick_play_singleplayer").toBool()) {
    return Feature::IS_QUICK_PLAY_SINGLEPLAYER;
  }
  if (rawFeature.value("is_quick_play_multiplayer").toBool()) {
    return Feature::IS_QUICK_PLAY_MULTIPLAYER;
  }
  if (rawFeature.value("is_quick_play_realms").toBool()) {
    return Feature::IS_QUICK_PLAY_REALMS;
  }
  return Feature::NONE;
}

Rule ClientJson::parseRule(const QVariantMap& rawRule) {
  if (rawRule.isEmpty()) {
    return {};
  }

  Rule rule;

  const QString RAW_ACTION {rawRule.value("action").toString()};
  rule.action = parseAction(RAW_ACTION);

  const QVariantMap RAW_OS {rawRule.value("os").toMap()};
  rule.os = parseOs(RAW_OS);

  const QVariantMap RAW_FEATURE {rawRule.value("features").toMap()};
  rule.feature = parseFeature(RAW_FEATURE);

  return rule;
}

Action ClientJson::parseAction(const QString& rawAction) {
  if (rawAction == "allow") {
    return Action::ALLOW;
  }
  if (rawAction == "disallow") {
    return Action::DISALLOW;
  }
  return Action::NONE;
}

OperatingSystem ClientJson::parseOs(const QVariantMap& rawOs) {
  OperatingSystem operatingSystem;

  const QString RAW_OS_NAME {rawOs.value("name").toString()};

  if (RAW_OS_NAME == "windows") {
    operatingSystem.name = SystemName::WINDOWS;
  }
  else if (RAW_OS_NAME == "linux") {
    operatingSystem.name = SystemName::LINUX;
  }
  else if (RAW_OS_NAME == "osx") {
    operatingSystem.name = SystemName::OSX;
  }
  else {
    operatingSystem.name = SystemName::NONE;
  }

  const QString RAW_OS_ARCHITECTURE {rawOs.value("arch").toString()};

  if (RAW_OS_ARCHITECTURE == "x86") {
    operatingSystem.arch = SystemArchitecture::X86;
  }
  else {
    operatingSystem.arch = SystemArchitecture::NONE;
  }

  const auto RAW_VERSION_RANGE {rawOs.value("versionRange").toMap()};
  operatingSystem.versionRange.min = RAW_VERSION_RANGE.value("min").toString();
  operatingSystem.versionRange.max = RAW_VERSION_RANGE.value("max").toString();


  return operatingSystem;
}

QStringList ClientJson::getValidDefaultJvmArguments() {
  return getValidArguments(defaultJvmArguments);
}

QStringList ClientJson::getValidJvmArguments() {
  return getValidArguments(jvmArguments);
}

QStringList ClientJson::getValidGameArguments() {
  return getValidArguments(gameArguments);
}

QStringList ClientJson::getValidArguments(const ArgumentBearer& bearer) {

  QStringList validArgs {bearer.unconditionalArguments};

  auto user {SystemInfo::getOperatingSystem()};

  for (const auto& arg : bearer.conditionalArguments) {
    if (arg.isUserSuitable(user)) {
      validArgs.append(arg.values);
    }
  }

  return validArgs;
}

QString ClientJson::getMinecraftClass() {
  return m_mainClass;
}