//
// Created by jay on 21/08/2026.
//

#include "VersionManifest.h"

#include <iostream>

ManifestLatest VersionManifest::s_latestVersions {};
QList<ManifestEntry> VersionManifest::s_versions {};
VersionManifest* VersionManifest::s_instance {nullptr};

VersionManifest::VersionManifest(QObject* parent)
  : NetworkRequester(parent) {
}

void VersionManifest::requestManifest() {
  qDebug() << "Requesting versions manifest";
  QNetworkRequest const REQUEST {MANIFEST_URL};

  setState(ManifestState::DOWNLOADING);
  Network::get(getInstance(), REQUEST);
}

VersionManifest* VersionManifest::getInstance() {
  if (!s_instance) {
    s_instance = new VersionManifest();
  }
  if (s_manifestState == ManifestState::MISSING) {
    requestManifest();
  }
  return s_instance;
}

void VersionManifest::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();

  if (reply->error()) {
    qDebug() << "Error: " << reply->errorString();
    throw std::runtime_error("Couldn't obtain versions manifest!");
    return;
  }

  if (!System::touch(MANIFEST_PATH, true)) {
    throw std::runtime_error("Couldn't create manifest file!");
  }

  if (!System::write(MANIFEST_PATH, reply->readAll())) {
    throw std::runtime_error("Couldn't write versions to manifest file!");
  }

  refreshManifest();
  setState(ManifestState::PRESENT);
}

ManifestEntry VersionManifest::getVersion(const QString& versionId) {
  qDebug() << "Getting manifest version:" << versionId;
  for (const auto& version: s_versions ) {
    if (version.id == versionId) {
      return version;
    }
  }
  qDebug() << "Version:" << versionId << "does not exist!";
  return {};
}

VersionManifest::ManifestState VersionManifest::getManifestState() {
  qDebug() << "Retrieved manifest state:" << s_manifestState;
  return s_manifestState;
}

void VersionManifest::setState(const ManifestState& state) {
  if (s_manifestState != state) {
    qDebug() << "Setting manifest state to:" << state;
    s_manifestState = state;
    emit getInstance()->stateChanged();
  }
}

ManifestLatest VersionManifest::getLatestVersions() {
  qDebug() << "Retrieving latest manifest versions...";
  return s_latestVersions;
}

QList<ManifestEntry> VersionManifest::getVersions(const QList<VersionType>& types) {
  qDebug() << "Retrieving manifest versions...";
  QList<ManifestEntry> versions;
  for (const auto& version: s_versions ) {
    if (types.contains(version.type)) {
      versions.append(version);
    }
  }
  return versions;
}

void VersionManifest::refreshManifest() {
  qDebug() << "Refreshing versions manifest...";
  const QVariantMap MANIFEST_DATA {JsonUtils::readJson(MANIFEST_PATH).toVariantMap()};

  s_latestVersions = parseLatestVersions(MANIFEST_DATA.value("latest").toMap());

  s_versions.clear();
  for (const auto& version : MANIFEST_DATA.value("versions").toList()) {
    try {
      s_versions.append(parseManifestEntry(version.toMap()));
    }
    catch (const std::exception& e) {
      qCritical() << e.what();
      continue;
    }
  }
}

ManifestLatest VersionManifest::parseLatestVersions(const QVariantMap& latestData) {
  qDebug() << "Parsing raw manifest data of latest versions...";
  return ManifestLatest {
    .release = latestData.value("release").toString(),
    .snapshot = latestData.value("snapshot").toString()
  };
}

ManifestEntry VersionManifest::parseManifestEntry(const QVariantMap& entryData) {
  qDebug() << "Parsing raw manifest data of a version entry...";
  const QString RAW_TYPE {entryData.value("type").toString()};
  VersionType type;

  if (RAW_TYPE == "release") {
    type = VersionType::RELEASE;
  }
  else if (RAW_TYPE == "snapshot") {
    type = VersionType::SNAPSHOT;
  }
  else if (RAW_TYPE == "old_alpha") {
    type = VersionType::OLD_ALPHA;
  }
  else if (RAW_TYPE == "old_beta") {
    type = VersionType::OLD_BETA;
  }
  else {
    qCritical() << "Unknown version type:" << RAW_TYPE;
    throw std::runtime_error("Unknown version type in manifest!");
  }

  return ManifestEntry {
    .id = entryData.value("id").toString(),
    .type = type,
    .url = entryData.value("url").toString(),
    .time = entryData.value("time").toString(),
    .releaseTime = entryData.value("releaseTime").toString(),
    .sha1 = entryData.value("sha1").toString(),
    .complianceLevel = entryData.value("complianceLevel").toInt(),
  };
}