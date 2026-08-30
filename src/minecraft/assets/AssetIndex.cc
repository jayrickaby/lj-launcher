//
// Created by jay on 21/08/2026.
//

#include "AssetIndex.h"

#include "minecraft/exec/JavaVirtualMachine.h"

AssetIndex::AssetIndex(const QVariantMap& data, QObject *parent)
: NetworkRequester(parent) {
  const QString INDEX_ID {data.value("id").toString()};
  const QString FILE_NAME {INDEX_ID + ".json"};

  m_downloadItem = DownloadItem {
    .hash = data.value("sha1").toString(),
    .id = INDEX_ID,
    .name = "",
    .path = FileSystem::joinPaths({getAssetsPath(), "indexes", FILE_NAME}),
    .size = data.value("size").toULongLong(),
    .totalSize = data.value("totalSize").toULongLong(),
    .url = data.value("url").toString()
  };

  JavaVirtualMachine::setVariable("assets_root", ASSETS_PATH);
  JavaVirtualMachine::setVariable("assets_index_name", INDEX_ID);
}

void AssetIndex::refreshIndex() {
  const QVariantMap DATA {JsonUtils::readJson(m_downloadItem.path).toVariantMap()};
  const QVariantMap OBJECTS (DATA.value("objects").toMap());

  m_objects.clear();
  m_objects.reserve(OBJECTS.size());

  for (const auto& filePath : OBJECTS.keys()) {
    const QVariantMap OBJECT {OBJECTS.value(filePath).toMap()};
    const QString HASH {OBJECT.value("hash").toString()};
    const quint64 SIZE {OBJECT.value("size").toULongLong()};

    // Skip non-existent just in case
    if (filePath.isEmpty() or HASH.isEmpty() or SIZE == 0) {
      continue;
    }

    // File contains whole hash, parent folder contains first two digits of same hash
    // e
    const QString BASE {
      FileSystem::joinPaths({HASH.left(2), HASH})
    };

    const QString URL {
      ASSETS_URL + "/" + BASE
    };
    const QString PATH {
      FileSystem::joinPaths({ASSETS_PATH, "objects", BASE})
    };

    m_objects.enqueue(
      DownloadItem {
      .hash = HASH,
      .id = "",
      .name = filePath,
      .path = PATH,
      .size = SIZE,
      .totalSize = 0,
      .url = URL
      }
    );
  }
}

void AssetIndex::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();

  const DownloadItem REQUEST {reply->property("requestParameters").value<DownloadItem>()};
  // Branch if AssetIndex
  if (REQUEST.url == m_downloadItem.url) {
    setState(AssetIndexState::DOWNLOADED_INDEX);
    refreshIndex();
    requestAssets();
    return;
  }

  // Finished downloading x for job '...': Downloaded successfully and hash matched
  Launcher::addLog(
    QString("Finished downloading %1 for job 'Resources': Downloaded successfuly and hash matched")
    .arg(REQUEST.path)
  );

  expectedAssetReplies--;
  if (expectedAssetReplies == 0) {
    setState(AssetIndexState::DOWNLOADED_ASSETS);
    setState(AssetIndexState::INITIALISED);
    Launcher::addLog("Job 'Version & Libraries' finished successfully");
  }
}

void AssetIndex::requestIndex() {
  if (m_downloadItem.isDownloaded()) {
    setState(AssetIndexState::DOWNLOADED_INDEX);
    refreshIndex();
    requestAssets();
    return;
  }

  Downloader::addDownload(this, m_downloadItem);
  setState(AssetIndexState::DOWNLOADING_INDEX);
}

void AssetIndex::requestAssets() {
  // Download job 'Resources' started (x files)
  Launcher::addLog(
    QString("Download job 'Resources' started (%1 files)")
    .arg(m_objects.size())
  );

  quint64 folderSize {
    FileSystem::getFolderSize(
      FileSystem::joinPaths({ASSETS_PATH, "objects"})
    )
  };

  if (folderSize == m_downloadItem.totalSize) {
    qDebug() << "All assets already downloaded!";
    setState(AssetIndexState::DOWNLOADED_ASSETS);
    setState(AssetIndexState::INITIALISED);
    return;
  }

  while (!m_objects.isEmpty()) {
    const auto ASSET {m_objects.dequeue()};

    if (ASSET.isDownloaded()) {
      continue;
    }

    expectedAssetReplies++;

    // Attempting do download x for jobs '...'...
    Launcher::addLog(
      QString("Attempting to download %1 for job 'Resources'...")
      .arg(ASSET.path)
    );

    Downloader::addDownload(this, ASSET);

    // Making directory x
    Launcher::addLog(
      QString("Making directory %1")
      .arg(FileSystem::getParentDirectory(ASSET.path))
    );

    setState(AssetIndexState::DOWNLOADING_ASSETS);
  }

  if (expectedAssetReplies == 0) {
    setState(AssetIndexState::DOWNLOADED_ASSETS);
    setState(AssetIndexState::INITIALISED);
  }
}

AssetIndexState AssetIndex::getState() const {
  return m_state;
}

void AssetIndex::setState(const AssetIndexState& state) {
  if (m_state != state) {
    m_state = state;
    emit stateChanged();
  }
}

QString AssetIndex::getAssetsPath() { return ASSETS_PATH; }
QString AssetIndex::getAssetsUrl() { return ASSETS_URL; }