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
  qDebug() << "Requesting ver manifest";

  setState(ManifestState::DOWNLOADING);

  Downloader::addDownload(
    getInstance(),
    DownloadItem {
    .hash = "",
    .id = "",
    .name = "",
    .path = MANIFEST_PATH,
    .size = 0,
    .totalSize = 0,
    .url = MANIFEST_URL
    }
  );
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
    throw std::runtime_error("Couldn't obtain ver manifest!");
    return;
  }

  refreshManifest();
  setState(ManifestState::PRESENT);
}

ManifestEntry VersionManifest::getVersion(const QString& versionId) {
  qDebug() << "Getting manifest version:" << versionId;
  for (const auto& version: s_versions ) {
    if (version.item.id == versionId) {
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
  qDebug() << "Retrieving latest manifest ver...";
  return s_latestVersions;
}

QList<ManifestEntry> VersionManifest::getVersions(const QList<VersionType>& types) {
  qDebug() << "Retrieving manifest ver...";
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
  return ManifestLatest {
    .release = latestData.value("release").toString(),
    .snapshot = latestData.value("snapshot").toString()
  };
}

ManifestEntry VersionManifest::parseManifestEntry(const QVariantMap& entryData) {
  const VersionType TYPE {
    Versions::convertToVersionType(entryData.value("type").toString())
  };

  const QString VERSION_ID {entryData.value("id").toString()};
  const QString FILE_NAME {VERSION_ID + ".json"};
  const QString PATH {
    FileSystem::joinPaths({Versions::getVersionsPath(), VERSION_ID, FILE_NAME})
  };

  return ManifestEntry {
    .item = DownloadItem{
      .hash = entryData.value("sha1").toString(),
      .id = VERSION_ID,
      .name = "",
      .path = PATH,
      .size = 0,
      .totalSize = 0,
      .url = entryData.value("url").toString()
    },
    .type = TYPE,
    .time = entryData.value("time").toString(),
    .releaseTime = entryData.value("releaseTime").toString(),
    .complianceLevel = entryData.value("complianceLevel").toInt()
  };
}