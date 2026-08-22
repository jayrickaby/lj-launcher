//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"

#include "System.h"
#include "Versions.h"
#include "minecraft/assets/AssetIndex.h"
#include "minecraft/lib/LibraryIndex.h"
#include "minecraft/ver/VersionManifest.h"
#include "sys/info/SystemInfo.h"
#include "sys/io/JsonUtils.h"

Downloader* Downloader::s_instance{nullptr};
Downloader::DownloadState Downloader::s_downloadState{DownloadState::IDLE};
QString Downloader::s_currentFile{};
qint64 Downloader::s_currentProgress{0};
qint64 Downloader::s_currentProgressMax{0};
QQueue<DownloadPair> Downloader::s_downloadQueue{};

Downloader::Downloader(QObject* parent)
  : QNetworkAccessManager(parent)
{
  if (!s_instance) {
    s_instance = this;
  }

  connect(getInstance(), &QNetworkAccessManager::finished,&onNetworkReply);
}

void Downloader::setCurrentFile(const QString& currentFile) {
  if (s_currentFile == currentFile) {
    return;
  }

  s_currentFile = currentFile;
  emit getInstance()->currentFileChanged();
}

void Downloader::setCurrentProgress(qint64 received, qint64 total) {
  if (s_currentProgress != received) {
    s_currentProgress = received;
    emit getInstance()->currentProgressChanged();
  }
  if (s_currentProgressMax != total) {
    s_currentProgressMax = total;
    emit getInstance()->currentProgressMaxChanged();
  }
}

void Downloader::addDownload(NetworkRequester* requester, const DownloadItem& downloadItem) {
  qDebug() << "Queued download:" << downloadItem.path << "to:" << downloadItem.url;

  s_downloadQueue.enqueue(DownloadPair {
    .requester = requester,
    .downloadItem = downloadItem
  });

  if (s_downloadState == DownloadState::IDLE) {
    downloadNext();
  }
}

void Downloader::addDownload(const DownloadItem& downloadItem) {
  addDownload(nullptr, downloadItem);
}

void Downloader::downloadNext() {
  if (s_downloadQueue.empty()) {
    setState(DownloadState::IDLE);
    return;
  }
  setState(DownloadState::DOWNLOADING);

  const auto& pair {s_downloadQueue.dequeue()};

  qDebug() << "Processing:" << pair.downloadItem.url << "to:" << pair.downloadItem.path;

  const QNetworkRequest REQUEST {pair.downloadItem.url};

  QNetworkReply* reply {nullptr};

  if (pair.requester) {
    reply = get(pair.requester, REQUEST);
  }
  else {
    reply = getInstance()->QNetworkAccessManager::get(REQUEST);
  }
  reply->setProperty("requestParameters", QVariant::fromValue(pair.downloadItem));

  connect (reply, &QNetworkReply::downloadProgress,
    getInstance(), &Downloader::setCurrentProgress);

  if (!pair.downloadItem.name.isEmpty()) {
    setCurrentFile(pair.downloadItem.name);
  }
  else {
    const QFileInfo URL {pair.downloadItem.url};
    setCurrentFile(URL.fileName());
  }
}

void Downloader::onNetworkReply(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    if (reply->property("requestParameters").isValid()) {
      DownloadItem item {reply->property("requestParameters").value<DownloadItem>()};
    }
    throw std::runtime_error(reply->errorString().toStdString());
  }

  if (reply->property("requestParameters").isValid()) {
    processDownload(reply);
  }
  if (reply->property("requester").isValid()) {
    processGenericRequest(reply);
  }

  setState(DownloadState::IDLE);
}

void Downloader::processDownload(QNetworkReply* reply) {
  const QByteArray DATA {reply->readAll()};
  const DownloadItem ITEM {reply->property("requestParameters").value<DownloadItem>()};

  if (ITEM.size != 0 and DATA.size() != ITEM.size) {
    qDebug() << "File:" << ITEM.path << "was corrupted! Redownloading...";
    qDebug() << "Calculated size:" << DATA.size() << "is meant to be:" << ITEM.size;
  }

  const QByteArray DATA_HASH {System::getSha1Checksum(DATA)};

  if (!ITEM.hash.isEmpty() and DATA_HASH != ITEM.hash) {
    qDebug() << "File:" << ITEM.path << "was corrupted! Redownloading...";
    qDebug() << "Calculated hash:" << DATA_HASH << "is meant to be:" << ITEM.hash;
    addDownload(ITEM);

    return;
  }

  FileSystem::makePath(FileSystem::getParentDirectory(ITEM.path));

  if (!System::write(ITEM.path, DATA)) {
    throw std::runtime_error("Unable to download required version file: " + ITEM.path.toStdString());
  };

  downloadNext();
}

void Downloader::processGenericRequest(QNetworkReply* reply) {
  QVariant const REQUESTER_PROPERTY = reply->property("requester");

  if (REQUESTER_PROPERTY.isNull() or !REQUESTER_PROPERTY.isValid()) {
    qDebug() << "Unknown network requester!";
    return;
  }

  auto* requester {REQUESTER_PROPERTY.value<NetworkRequester*>()};
  if (!requester) {
    qDebug() << "Requester is null!";
    return;
  }

  requester->onNetworkReply(reply);
}

Downloader* Downloader::getInstance() {
  if (!s_instance) {
    s_instance = new Downloader();
  }
  return s_instance;
}

void Downloader::setState(const DownloadState& state) {
  if (s_downloadState == state) { return; }

  s_downloadState = state;
  emit getInstance()->downloadStateChanged();
}

QNetworkReply* Downloader::get(const NetworkRequester* requester, const QNetworkRequest& request) {
  qDebug() << "Requesting GET from:" << request.url();
  auto* reply {getInstance()->QNetworkAccessManager::get(request)};

  reply->setProperty("requester", QVariant::fromValue(requester));

  return reply;
}

QNetworkReply* Downloader::post(const NetworkRequester* requester, const QNetworkRequest& request, const QByteArray& data) {
  qDebug() << "Requesting POST from:" << request.url();
  auto* reply {getInstance()->QNetworkAccessManager::post(request,data)};

  reply->setProperty("requester", QVariant::fromValue(requester));

  return reply;
}