//
// Created by jay on 21/08/2026.
//

#include "LibraryIndex.h"

#include "sys/info/SystemInfo.h"

LibraryIndex::LibraryIndex(const QVariantList& data, QObject *parent)
: NetworkRequester(parent) {
  if (data.isEmpty()) {
    return;
  }

  setState(LibraryIndexState::LOADING);

  m_libraries.clear();
  m_libraries.reserve(data.size());

  for (const auto& rawVariant : data) {
    Library library;

    const QVariantMap RAW_LIBRARY {rawVariant.toMap()};

    library.artifact = parseArtifact(
      RAW_LIBRARY
      .value("downloads").toMap()
      .value("artifact").toMap()
    );

    library.artifact.name = RAW_LIBRARY.value("name").toString();

    const QVariantList RAW_RULES (RAW_LIBRARY.value("rules").toList());
    for (const auto& rawRule : RAW_RULES) {
      const QVariantMap RULE {rawRule.toMap()};
      library.rules.append(parseRule(RULE));
    }

    m_libraries.enqueue(library);
  }
  setState(LibraryIndexState::LOADED);
}

void LibraryIndex::requestLibraries() {
  qDebug() << "Requesting libraries";
  setState(LibraryIndexState::DOWNLOADING);

  const OperatingSystem USER_OS {SystemInfo::getOperatingSystem()};

  while (!m_libraries.isEmpty()) {
    const Library LIBRARY {m_libraries.dequeue()};

    if (!LIBRARY.isUserSuitable(USER_OS)) {
      qDebug() << "Skipping" << LIBRARY.artifact.name << "due to imposed rules";
      continue;
    }

    if (LIBRARY.artifact.isDownloaded()) {
      qDebug() << "Skipping" << LIBRARY.artifact.name << "due to it already being downloaded!";
      continue;
    }

    expectedLibraryReplies++;
    Downloader::addDownload(this, LIBRARY.artifact);
  }

  if (expectedLibraryReplies == 0) {
    setState(LibraryIndexState::DOWNLOADED);
    setState(LibraryIndexState::INITIALISED);
  }
}

void LibraryIndex::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();
  if (reply->error()) {
    throw std::runtime_error("Failed to download library");
  }

  expectedLibraryReplies--;

  if (expectedLibraryReplies == 0) {
    setState(LibraryIndexState::DOWNLOADED);
    setState(LibraryIndexState::INITIALISED);
  }
}

DownloadItem LibraryIndex::parseArtifact(const QVariantMap& rawArtifact) {
  if (rawArtifact.isEmpty()) {
    return {};
  }

  const QString PATH {rawArtifact.value("path").toString()};

  return DownloadItem{
    .hash = rawArtifact.value("sha1").toString(),
    .id = "",
    .name = "",
    // Artifact is relative to library path, so make absolute
    .path = FileSystem::joinPaths({getLibraryPath(), PATH}),
    .size = rawArtifact.value("size").toULongLong(),
    .totalSize = 0,
    .url = rawArtifact.value("url").toString()
  };
}

Rule LibraryIndex::parseRule(const QVariantMap& rawRule) {
  if (rawRule.isEmpty()) {
    return {};
  }

  Rule rule;

  const QString RAW_ACTION {rawRule.value("action").toString()};
  rule.action = parseAction(RAW_ACTION);

  const QVariantMap RAW_OS {rawRule.value("os").toMap()};
  rule.os = parseOs(RAW_OS);

  return rule;
}

Action LibraryIndex::parseAction(const QString& rawAction) {
  if (rawAction == "allow") {
    return Action::ALLOW;
  }
  if (rawAction == "disallow") {
    return Action::DISALLOW;
  }
  return Action::NONE;
}

OperatingSystem LibraryIndex::parseOs(const QVariantMap& rawOs) {
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

  return operatingSystem;
}

QString LibraryIndex::getLibraryPath() {
  return LIBRARY_PATH;
}

QString LibraryIndex::findLibraryPath() {
  return FileSystem::joinPaths(
    { Launcher::getGameDirectory().toLocalFile(), "libraries" }
  );
}

LibraryIndexState LibraryIndex::getState() const {
  return m_state;
}

void LibraryIndex::setState(const LibraryIndexState& state) {
  if (m_state != state) {
    m_state = state;
    emit stateChanged();
  }
}