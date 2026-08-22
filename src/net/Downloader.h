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
#include <QNetworkAccessManager>
#include <QUrlQuery>

#include "Launcher.h"
#include "DownloadItem.h"
#include "NetworkRequester.h"

 enum class DownloadType {
  CLIENT_JSON,
  CLIENT_JAR,
  ASSET,
  ASSET_INDEX,
  LIBRARY
};

struct DownloadPair {
  NetworkRequester* requester {nullptr};
  DownloadItem downloadItem {};
};

class Downloader : public QNetworkAccessManager {
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
    DOWNLOADING
  };
  Q_ENUM(DownloadState);

  DownloadState downloadState() { return s_downloadState; };

  QString currentFile() { return s_currentFile; };
  qint64 currentProgress() { return s_currentProgress; };
  qint64 currentProgressMax() { return s_currentProgressMax; };

  static void addDownload(const DownloadItem& downloadItem);
  static void addDownload(NetworkRequester* requester, const DownloadItem& downloadItem);
  static void downloadNext();
  static Downloader* getInstance();

  static QNetworkReply* get(const NetworkRequester* requester, const QNetworkRequest& request);
  static QNetworkReply* post(const NetworkRequester* requester, const QNetworkRequest& request, const QByteArray& data);

private slots:
  static void onNetworkReply(QNetworkReply* reply);

private:
  static void processDownload(QNetworkReply* reply);
  static void processGenericRequest(QNetworkReply* reply);

  static void setCurrentFile(const QString& currentFile);
  static void setCurrentProgress(qint64 received, qint64 total);
  static void setState(const DownloadState& state);

  static DownloadState s_downloadState;
  static QQueue<DownloadPair> s_downloadQueue;
  static QString s_currentFile;
  static qint64 s_currentProgress;
  static qint64 s_currentProgressMax;

  static Downloader* s_instance;
};

#endif  // LJ_LAUNCHER_DOWNLOADER_H_
