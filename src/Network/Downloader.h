 //
// Created by jay on 20/08/2026.
//

#ifndef LJ_LAUNCHER_DOWNLOADER_H_
#define LJ_LAUNCHER_DOWNLOADER_H_
#include <qqml.h>

#include <QNetworkReply>
#include <QUrlQuery>
#include <QFileInfo>
#include <QObject>
#include <QQueue>

#include "Network.h"
#include "NetworkRequester.h"

class Downloader : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(DownloadState downloadState READ downloadState NOTIFY downloadStateChanged)
  Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)

signals:
  void currentFileChanged();
  void downloadStateChanged();

public:
  explicit Downloader(QObject *parent = nullptr);

  enum class DownloadState {
    IDLE,
    DOWNLOADING,
    FINISHED
  };
  Q_ENUM(DownloadState);

  DownloadState downloadState() { return s_downloadState; };

  QString currentFile() { return s_currentFile; };

  static void addDownload(const QString& onlineUrl, const QString& localUrl);
  static void downloadNext();
  static Downloader* getInstance();

  void onNetworkReply(QNetworkReply* reply) override;

private:
  static void setCurrentFile(const QString& currentFile);
  static void setState(const DownloadState& state);


  static DownloadState s_downloadState;
  static QQueue<QPair<QString, QString>> s_downloadQueue;
  static QString s_currentFile;

  static Downloader* s_instance;
};

#endif  // LJ_LAUNCHER_DOWNLOADER_H_
