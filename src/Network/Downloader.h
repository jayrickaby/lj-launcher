 //
// Created by jay on 20/08/2026.
//

#ifndef LJ_LAUNCHER_DOWNLOADER_H_
#define LJ_LAUNCHER_DOWNLOADER_H_
#include <qqml.h>

#include <QNetworkReply>
#include <QObject>

#include "NetworkRequester.h"

class Downloader : public NetworkRequester {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

signals:
  void downloadStateChanged();

public:
  explicit Downloader(QObject *parent = nullptr);

  enum class DownloadState {
    IDLE,
    DOWNLOADED,
    FINISHED
  };
  Q_ENUM(DownloadState);

  void downloadFromClientJson(const QString& jsonUrl);
  static Downloader* getInstance();

  void onNetworkReply(QNetworkReply *reply) override;

private:
  void setState(const DownloadState& state);

  static DownloadState s_downloadState;

  static Downloader* s_instance;
};

#endif  // LJ_LAUNCHER_DOWNLOADER_H_
