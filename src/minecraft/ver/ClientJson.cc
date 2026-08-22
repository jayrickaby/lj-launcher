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

  connect(this, &ClientJson::receivedNetworkReply, &ClientJson::refreshState);
  requestJson();
}

void ClientJson::refreshJson() {
  setState(ClientState::PREPARING);
  const QVariantMap DATA {JsonUtils::readJson(m_clientJson.path).toVariantMap()};
  m_assetIndex = new AssetIndex(DATA.value("assetIndex").toMap());
  m_libraryIndex = new LibraryIndex(DATA.value("libraries").toList());

  connect(m_assetIndex, &AssetIndex::stateChanged, this, &ClientJson::refreshState);
  connect(m_libraryIndex, &LibraryIndex::stateChanged, this, &ClientJson::refreshState);

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

  setState(ClientState::DOWNLOADING_JSON);
  Downloader::addDownload(this, m_clientJson);
}

void ClientJson::requestJar() {
  qDebug() << "Requesting client.jar file...";

  if (m_clientJar.isDownloaded()) {
    qDebug() << "Skipped client.jar as it was already downloaded!";
    return;
  }

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