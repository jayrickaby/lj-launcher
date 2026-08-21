//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONS_H_
#define LJ_LAUNCHER_VERSIONS_H_

#include <QDirIterator>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include "NetworkRequester.h"
#include "../Launcher.h"
#include "../System.h"

enum class VersionType {
  RELEASE,
  SNAPSHOT,
  OLD_ALPHA,
  OLD_BETA
};

class Versions : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(QList<QVariantMap> versionsList READ versionsList NOTIFY versionsListChanged);
  QML_ELEMENT
  QML_SINGLETON

signals:
  void versionsListChanged();

public:
  explicit Versions(QObject *parent = nullptr);

  QList<QVariantMap> versionsList();
  
  static QList<QVariantMap> getDownloadedVersions();
  static QVariantMap getDownloadedVersion(const QString& versionId);
  static bool isDownloaded(const QString& versionId);

  static QString getVersionsPath() { return VERSIONS_PATH.path(); };

  static Versions* getInstance();

private:
  static QUrl findVersionsPath();

  static inline const QUrl VERSIONS_PATH {findVersionsPath()};

  static Versions* s_instance;
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
