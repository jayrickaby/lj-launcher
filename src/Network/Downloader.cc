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
QQueue<QPair<QString, QString>> Downloader::s_downloadQueue{};

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

void Downloader::addDownload(const QString& onlineUrl, const QString& localUrl) {
  qDebug() << "Queued download:" << onlineUrl << "to:" << localUrl;
  s_downloadQueue.enqueue(QPair<QString, QString>(onlineUrl, localUrl));

  if (s_downloadState == DownloadState::IDLE) {
    downloadNext();
  }
}

void Downloader::downloadNext() {
  setState(DownloadState::DOWNLOADING);

  const auto& file {s_downloadQueue.dequeue()};
  const QNetworkRequest REQUEST {file.first};
  QNetworkReply* reply { Network::get(getInstance(), REQUEST) };
  reply->setProperty("localfile", file.second);

  connect (reply, &QNetworkReply::downloadProgress,
    getInstance(), &Downloader::setCurrentProgress);

  const QFileInfo URL {file.second};
  setCurrentFile(URL.fileName());
}

void Downloader::onNetworkReply(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    throw std::runtime_error(reply->errorString().toStdString());
  }

  const QString URL {reply->property("localfile").toString()};
  const QFileInfo fileInfo {URL};

  if (!fileInfo.exists()) {
    QDir().mkpath(fileInfo.absolutePath());
  }

  if (!System::touch(URL, true)
    or !System::write(URL, reply->readAll())) {
    throw std::runtime_error("Unable to download required version file:" + reply->url().toString().toStdString());
  };

  if (reply->url().toString() == Versions::getAvailableVersion(Profiles::getCurrentProfileVersion()).value("url").toString()) {
    // Adds more required downloads from the json
    processClientJson(URL);
  }

  if (s_downloadQueue.empty()) {
    setState(DownloadState::FINISHED);
  }
  else {
    downloadNext();
  }
}
void Downloader::processClientJson(const QString& url) {
  QVariantMap data {
    QJsonDocument::fromJson(
      System::read(url)
      .toUtf8()
    )
    .object()
    .toVariantMap()
  };

  const QVariantMap ASSETS_INDEX {data.value("assetIndex").toMap()};
  const QString INDEX_URL {ASSETS_INDEX.value("url").toString()};
  const QString INDEX_ID {ASSETS_INDEX.value("id").toString()};
  const QString INDEX_PATH {
    QString("%1/assets/indexes/%2.json")
    .arg(Launcher::getGameDirectory().toLocalFile(),INDEX_ID)
  };
  addDownload(INDEX_URL, INDEX_PATH);

  const QVariantMap JAR_DOWNLOADS {data.value("downloads").toMap()};
  const QVariantMap CLIENT_JAR {JAR_DOWNLOADS.value("client").toMap()};
  const QString JAR_URL {CLIENT_JAR.value("url").toString()};
  const QString JAR_PATH {
    QString("%1/%2.jar")
    .arg(QFileInfo(url).absolutePath(), Profiles::getCurrentProfileVersion())
  };
  addDownload(JAR_URL, JAR_PATH);

  const QVariantList LIBRARY_DOWNLOADS (data.value("libraries").toList());

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
    const QString ARTIFACT_PATH {
      QString("%1/libraries/%2")
      .arg(Launcher::getGameDirectory().toLocalFile(), ARTIFACT.value("path").toString())
    };
    addDownload(ARTIFACT_URL, ARTIFACT_PATH);
  }
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

