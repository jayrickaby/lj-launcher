//
// Created by jay on 20/08/2026.
//

#include "Downloader.h"

Downloader* Downloader::s_instance{nullptr};
Downloader::DownloadState Downloader::s_downloadState{DownloadState::IDLE};

Downloader::Downloader(QObject* parent)
  : NetworkRequester(parent)
{
  if (!s_instance) {
    s_instance = this;
  }
}

void Downloader::downloadFromClientJson(const QString& jsonUrl) {

}

// void Downloader::onNetworkReply(QNetworkReply* reply) {
//   qDebug() << "Downloader::onNetworkReply()";
// }

Downloader* Downloader::getInstance() {
  if (!s_instance) {
    s_instance = new Downloader();
  }
  return s_instance;
}

void Downloader::setState(const DownloadState& state) {
  if (s_downloadState == state) { return; }

  s_downloadState = state;
  emit downloadStateChanged();
}

