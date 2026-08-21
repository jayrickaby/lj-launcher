//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"

#include "System.h"
#include "Versions.h"

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
  qDebug() << "Queued download:" << downloadItem.onlineUrl << "to:" << downloadItem.localUrl;

  s_downloadQueue.enqueue(downloadItem);

  if (s_downloadState == DownloadState::IDLE) {
    downloadNext();
  }
}

void Downloader::downloadNext() {
  setState(DownloadState::DOWNLOADING);

  const auto& file {s_downloadQueue.dequeue()};
  const QNetworkRequest REQUEST {file.onlineUrl};
  QNetworkReply* reply { Network::get(getInstance(), REQUEST) };
  reply->setProperty("localfile", file.localUrl);
  reply->setProperty("hash", file.hash);
  reply->setProperty("type", QVariant::fromValue(file.type));

  connect (reply, &QNetworkReply::downloadProgress,
    getInstance(), &Downloader::setCurrentProgress);

  const QFileInfo URL {file.localUrl};
  setCurrentFile(URL.fileName());
}

void Downloader::onNetworkReply(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    throw std::runtime_error(reply->errorString().toStdString());
  }

  const QByteArray DATA {reply->readAll()};
  const QByteArray DATA_HASH {System::getSha1Checksum(DATA)};
  const QByteArray DATA_TRUE_HASH {reply->property("hash").toByteArray()};
  const QString DATA_LOCAL_URL {reply->property("localfile").toString()};
  const QString DATA_ONLINE_URL {reply->url().toString()};
  const DownloadType DATA_TYPE {reply->property("type").value<DownloadType>()};

  if (!DATA_TRUE_HASH.isEmpty() and DATA_HASH != DATA_TRUE_HASH) {
    qDebug() << "File:" << DATA_ONLINE_URL << "was corrupted! Redownloading...";
    qDebug() << "Calculated hash:" << DATA_HASH << "is meant to be:" << DATA_TRUE_HASH;
    addDownload(
      DownloadItem(
      DATA_ONLINE_URL, DATA_LOCAL_URL, DATA_TYPE, DATA_TRUE_HASH
      )
    );

    return;
  }

  const QFileInfo fileInfo {DATA_LOCAL_URL};

  if (!fileInfo.exists()) {
    QDir().mkpath(fileInfo.absolutePath());
  }

  if (!System::touch(DATA_LOCAL_URL, true)
    or !System::write(DATA_LOCAL_URL, DATA)) {
    throw std::runtime_error("Unable to download required version file:" + reply->url().toString().toStdString());
  };

  switch (DATA_TYPE) {
    case DownloadType::ASSET_INDEX:
      processAssetsIndex(DATA_LOCAL_URL);
      break;
    case DownloadType::CLIENT_JSON: {
      processClientJson(DATA_LOCAL_URL);
      break;
    };
    default:;
  }

  if (reply->url().toString() == Versions::getAvailableVersion(Profiles::getCurrentProfileVersion()).value("url").toString()) {
    // Adds more required downloads from the json
    processClientJson(DATA_LOCAL_URL);
  }

  if (s_downloadQueue.empty()) {
    setState(DownloadState::FINISHED);
  }
  else {
    downloadNext();
  }
}
void Downloader::processClientJson(const QString& url) {
  const QVariantMap DATA {
    QJsonDocument::fromJson(
      System::read(url)
      .toUtf8()
    )
    .object()
    .toVariantMap()
  };

  // Saves having to keep getting it from Profiles
  const QString VERSION_ID {DATA.value("id").toString()};

  const QVariantMap ASSETS_INDEX {DATA.value("assetIndex").toMap()};
  const QString INDEX_URL {ASSETS_INDEX.value("url").toString()};
  const QString INDEX_ID {ASSETS_INDEX.value("id").toString()};
  const QString INDEX_HASH {ASSETS_INDEX.value("sha1").toString()};
  const QString INDEX_PATH {
    QString("%1/assets/indexes/%2.json")
    .arg(Launcher::getGameDirectory().toLocalFile(),INDEX_ID)
  };
  addDownload(
    DownloadItem(
      INDEX_URL, INDEX_PATH, DownloadType::ASSET_INDEX, INDEX_HASH
    )
  );

  const QVariantMap JAR_DOWNLOADS {DATA.value("downloads").toMap()};
  const QVariantMap CLIENT_JAR {JAR_DOWNLOADS.value("client").toMap()};
  const QString JAR_URL {CLIENT_JAR.value("url").toString()};
  const QString JAR_HASH {CLIENT_JAR.value("sha1").toString()};
  const QString JAR_PATH {
    QString("%1/%2.jar")
    .arg(QFileInfo(url).absolutePath(), VERSION_ID)
  };
  addDownload(
    DownloadItem(
      JAR_URL, JAR_PATH, DownloadType::CLIENT_JAR, JAR_HASH
    )
  );

  const QVariantList LIBRARY_DOWNLOADS (DATA.value("libraries").toList());

  for (const auto& lib : LIBRARY_DOWNLOADS) {
    const QVariantMap LIBRARY {lib.toMap()};
    const QVariantList RULES (LIBRARY.value("rules").toList());
    const QString NAME (LIBRARY.value("name").toString());

    if (shouldSkipFromJsonRules(RULES)) {
      qDebug() << "Skipping" << NAME << "due to imposed rules";
      continue;
    }

    const QVariantMap ARTIFACT {LIBRARY.value("downloads").toMap().value("artifact").toMap()};
    const QString ARTIFACT_URL {ARTIFACT.value("url").toString()};
    const QString ARTIFACT_HASH {ARTIFACT.value("sha1").toString()};
    const QString ARTIFACT_PATH {
      QString("%1/libraries/%2")
      .arg(Launcher::getGameDirectory().toLocalFile(), ARTIFACT.value("path").toString())
    };
    addDownload(
      DownloadItem(
        ARTIFACT_URL, ARTIFACT_PATH, DownloadType::LIBRARY, ARTIFACT_HASH
      )
    );
  }
}

void Downloader::processAssetsIndex(const QString& url) {

}

bool Downloader::shouldSkipFromJsonRules(const QVariantList& rules) {
  for (const auto& r : rules) {
    const QVariantMap RULE {r.toMap()};
    const QString RULE_ACTION {RULE.value("action").toString()};
    const QString RULE_OS {RULE.value("os").toMap().value("name").toString()};
    const QString RULE_ARCH {RULE.value("os").toMap().value("architecture").toString()};

    const QString USER_OS {System::getOs()};
    const QString USER_ARCH {System::getArchitecture()};

    bool targetMatches{true};

    if (!RULE_OS.isEmpty() and RULE_OS != USER_OS) {
      targetMatches = false;
    }
    if (!RULE_ARCH.isEmpty() and RULE_ARCH != USER_ARCH) {
      targetMatches = false;
    }

    if (RULE_ACTION == "allow" and !targetMatches) {
     return true;
    }
    if (RULE_ACTION == "disallow" and targetMatches) {
      return true;
    }
  }

  return false;
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

