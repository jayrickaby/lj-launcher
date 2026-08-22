 //
// Created by jay on 20/08/2026.
//

#ifndef LJ_LAUNCHER_DOWNLOADER_H_
#define LJ_LAUNCHER_DOWNLOADER_H_
#include <qqml.h>

#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QObject>
#include <QQueue>
#include <QUrlQuery>

#include "minecraft/libraries/Library.h"
#include "minecraft/libraries/LibraryIndex.h"
#include "Launcher.h"
#include "Network.h"
#include "NetworkRequester.h"

 enum class DownloadType {
  CLIENT_JSON,
  CLIENT_JAR,
  ASSET,
  ASSET_INDEX,
  LIBRARY
};

struct DownloadItem {
  QString hash {""};
  QString id {""};
  QString name {""};
  QString path {""};
  quint64 size {0};
  quint64 totalSize {0};
  QString url {""};
};

class Downloader : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(QString currentFile READ currentFile NOTIFY currentFileChanged)
  Q_PROPERTY(qint64 currentProgress READ currentProgress NOTIFY currentProgressChanged)
  Q_PROPERTY(qint64 currentProgressMax READ currentProgressMax NOTIFY currentProgressMaxChanged)

  Q_PROPERTY(DownloadState downloadState READ downloadState NOTIFY downloadStateChanged)

signals:
  void currentFileChanged();
  void currentProgressChanged();
  void currentProgressMaxChanged();
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
  qint64 currentProgress() { return s_currentProgress; };
  qint64 currentProgressMax() { return s_currentProgressMax; };

  static void addDownload(const DownloadItem& downloadItem);
  static void downloadNext();
  static Downloader* getInstance();

  void onNetworkReply(QNetworkReply* reply) override;

private:
  static bool alreadyDownloaded(const DownloadItem& downloadItem);

  static void setCurrentFile(const QString& currentFile);
  static void setCurrentProgress(qint64 received, qint64 total);
  static void setState(const DownloadState& state);

  static QString findAssetsPath();

  static DownloadState s_downloadState;
  static QQueue<DownloadItem> s_downloadQueue;
  static QString s_currentFile;
  static qint64 s_currentProgress;
  static qint64 s_currentProgressMax;

  static Downloader* s_instance;
};

#endif  // LJ_LAUNCHER_DOWNLOADER_H_
