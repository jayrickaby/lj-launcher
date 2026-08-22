//
// Created by jay on 22/08/2026.
//

#include "ClientJson.h"

#include "sys/info/SystemInfo.h"

ClientJson::ClientJson(const QString& path)
// TODO: Find some way to circumvent this horribleness
: m_assetIndex{JsonUtils::readJson(path).toVariantMap().value("assetIndex").toMap()},
  m_libraryIndex{JsonUtils::readJson(path).toVariantMap().value("libraries").toList()},
  CLIENT_PATH(FileSystem::getParentDirectory(path)) {
  const QVariantMap DATA {JsonUtils::readJson(path).toVariantMap()};

  m_id = DATA.value("id").toString();
  m_mainClass = DATA.value("mainClass").toString();
  m_releaseTime = DATA.value("releaseTime").toString();
  m_time = DATA.value("time").toString();

  m_type = Versions::convertToVersionType(DATA.value("type").toString());


  m_clientJar = (
    parseClientJar(
    DATA
      .value("downloads").toMap()
      .value("client").toMap()
    )
  );
}

void ClientJson::requestJar() {
  qDebug() << "Requesting client.jar file...";
  Downloader::addDownload(m_clientJar);
}

void ClientJson::requestLibraries() {
  qDebug() << "Requesting libraries";

  const OperatingSystem USER_OS {SystemInfo::getOperatingSystem()};

  for (const auto& library : m_libraryIndex.getLibraries()) {
    if (!library.isUserSuitable(USER_OS)) {
      qDebug() << "Skipping" << library.name << "due to imposed rules";
      continue;
    }

    Downloader::addDownload(library.artifact);
  }
}

void ClientJson::requestAssets() {
  const QString OBJECTS_PATH {AssetIndex::getAssetsPath() + "/objects/"};

  for (const auto& asset : m_assetIndex.getObjects()) {
    Downloader::addDownload(asset);
  }
}

DownloadItem ClientJson::parseClientJar(const QVariantMap& data) {
  const QString JAR_NAME {m_id + ".jar"};

  return DownloadItem{
    .hash = data.value("sha").toString(),
    .id = "",
    .name = "",
    .path = FileSystem::joinPaths({CLIENT_PATH, JAR_NAME}),
    .size = data.value("size").toULongLong(),
    .totalSize = 0,
    .url = data.value("url").toString()
  };
}