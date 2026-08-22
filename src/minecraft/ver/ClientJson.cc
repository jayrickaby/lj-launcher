//
// Created by jay on 22/08/2026.
//

#include "ClientJson.h"

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

  requestJson();
}

void ClientJson::refreshJson() {
  const QVariantMap DATA {JsonUtils::readJson(m_clientJson.path).toVariantMap()};
  m_assetIndex = DATA.value("assetIndex").toMap();
  m_libraryIndex = new LibraryIndex(DATA.value("libraries").toList());

  m_mainClass = DATA.value("mainClass").toString();

  m_clientJar = (
    parseClientJar(
    DATA
      .value("downloads").toMap()
      .value("client").toMap()
    )
  );
}

void ClientJson::requestJson() {
  qDebug() << "Requesting client.json file...";


  Downloader::addDownload(this, m_clientJson);
}

void ClientJson::requestJar() {
  qDebug() << "Requesting client.jar file...";

  if (isJarDownloaded()) {
    qDebug() << "Skipped client.jar as it was already downloaded!";
    return;
  }

  // Don't add this as requester or else it will be an infinite loop of refresh -> request
  Downloader::addDownload(m_clientJar);
}

bool ClientJson::isJarDownloaded() {
  qDebug() << "Checking if client.jar is downloaded...";

  if (!FileSystem::isFile(m_clientJar.path)) {
    qDebug() << "Client.jar is not downloaded!";
    return false;
  }

  quint64 size {FileSystem::getFileSize(m_clientJar.path)};

  if (size != m_clientJar.size) {
    qDebug() << "Client.jar size mismatch!";
    qDebug() << "Expected:" << m_clientJar.size << "but locally:" << size;
    return false;
  }

  QByteArray hash {System::getSha1Checksum(System::cat(m_clientJar.path))};

  if (hash != m_clientJar.hash) {
    qDebug() << "Client.jar hash mismatch!";
    qDebug() << "Expected:" << m_clientJar.hash << "but locally:" << hash;
    return false;
  }

  return true;
}

void ClientJson::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();
  refreshJson();
  requestJar();
  m_libraryIndex->requestLibraries();
}

void ClientJson::requestAssets() {
  const QString OBJECTS_PATH {AssetIndex::getAssetsPath() + "/objects/"};

  for (const auto& asset : m_assetIndex.getObjects()) {
    Downloader::addDownload(this, asset);
  }
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