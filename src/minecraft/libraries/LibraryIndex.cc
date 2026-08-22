//
// Created by jay on 21/08/2026.
//

#include "LibraryIndex.h"


LibraryIndex::LibraryIndex(const QVariantList& data) {
  if (data.isEmpty()) {
    return;
  }

  for (const auto& rawVariant : data) {
    Library library;

    const QVariantMap RAW_LIBRARY {rawVariant.toMap()};
    library.name = RAW_LIBRARY.value("name").toString();

    library.artifact = parseArtifact(
      RAW_LIBRARY
      .value("downloads").toMap()
      .value("artifact").toMap()
    );

    const QVariantList RAW_RULES (RAW_LIBRARY.value("rules").toList());
    for (const auto& rawRule : RAW_RULES) {
      const QVariantMap RULE {rawRule.toMap()};
      library.rules.append(parseRule(RULE));
    }

    m_libraries.append(library);
  }
}

QList<Library> LibraryIndex::getLibraries() const {
  return m_libraries;
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