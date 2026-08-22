//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"

#include "System.h"
#include "Versions.h"
#include "minecraft/assets/AssetIndex.h"
#include "minecraft/libraries/LibraryIndex.h"
#include "minecraft/versions/VersionManifest.h"
#include "sys/info/SystemInfo.h"
#include "sys/io/JsonUtils.h"

Downloader* Downloader::s_instance{nullptr};
Downloader::DownloadState Downloader::s_downloadState{DownloadState::IDLE};
QString Downloader::s_currentFile{};
qint64 Downloader::s_currentProgress{0};
qint64 Downloader::s_currentProgressMax{0};
QQueue<DownloadItem> Downloader::s_downloadQueue{};

Downloader::Downloader(QObject* parent)
  : NetworkRequester(parent)
{
  if (!s_instance) {
    s_instance = this;
  }
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

void Downloader::addDownload(const DownloadItem& downloadItem) {
  qDebug() << "Queued download:" << downloadItem.path << "to:" << downloadItem.url;

  s_downloadQueue.enqueue(downloadItem);

  if (s_downloadState == DownloadState::IDLE) {
    downloadNext();
  }
}

void Downloader::downloadNext() {
  if (s_downloadQueue.empty()) {
    setState(DownloadState::IDLE);
    return;
  }
  setState(DownloadState::DOWNLOADING);

  const auto& file {s_downloadQueue.dequeue()};

  if (alreadyDownloaded(file)) {
    qDebug() << "File:" << file.url << "is already downloaded!";
    downloadNext();
    return;
  }

  const QNetworkRequest REQUEST {file.path};
  QNetworkReply* reply { Network::get(getInstance(), REQUEST) };
  reply->setProperty("requestParameters", QVariant::fromValue(file));

  connect (reply, &QNetworkReply::downloadProgress,
    getInstance(), &Downloader::setCurrentProgress);

  const QFileInfo URL {file.url};
  if (!file.name.isEmpty()) {
    setCurrentFile(file.name);
  }
  else {
    setCurrentFile(URL.fileName());
  }
}

void Downloader::onNetworkReply(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    throw std::runtime_error(reply->errorString().toStdString());
  }

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

  FileSystem::makePath(ITEM.url);

  if (!System::touch(ITEM.url, true)
    or !System::write(ITEM.url, DATA)) {
    throw std::runtime_error("Unable to download required version file: " + ITEM.path.toStdString());
  };

  setState(DownloadState::FINISHED);

  switch (ITEM.type) {
    case DownloadType::ASSET_INDEX:
      processAssetsIndex(ITEM.url);
      break;
    case DownloadType::CLIENT_JSON: {
      processClientJson(ITEM.url);
      break;
    };
    default:;
  }

  downloadNext();
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

QString Downloader::findAssetsPath() {
  return Launcher::getGameDirectory().toLocalFile() + "/assets/";
}

bool Downloader::alreadyDownloaded(const DownloadItem& downloadItem) {
  const QString FILE_PATH {downloadItem.url};

  if (!FileSystem::isFile(FILE_PATH)) {
    return false;
  }

  const QByteArray FILE_HASH {System::getSha1Checksum(
    System::read(FILE_PATH).toUtf8())
  };

  return downloadItem.hash == FILE_HASH;
}