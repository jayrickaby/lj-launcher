//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"


#include "System.h"

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

  if (!System::touch(URL)
    or !System::write(URL, reply->readAll())) {
    throw std::runtime_error("Unable to download required version file:" + reply->url().toString().toStdString());
  };

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

