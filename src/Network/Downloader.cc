//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"


Downloader* Downloader::s_instance{nullptr};
Downloader::DownloadState Downloader::s_downloadState{DownloadState::IDLE};
QString Downloader::s_currentFile{};
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

  const QFileInfo URL {file.second};
  setCurrentFile(URL.fileName());
}

void Downloader::onNetworkReply(QNetworkReply* reply) {
  qDebug() << reply->property("localfile").toString();
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

