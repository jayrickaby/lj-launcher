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

enum class VersionType {
  RELEASE,
  SNAPSHOT,
  OLD_ALPHA,
  OLD_BETA
};

class Versions : public NetworkRequester {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Versions(QObject *parent = nullptr);

  static QList<QVariantMap> getDownloadedVersions();
  static QVariantMap getDownloadedVersion(const QString& versionId);
  static bool isDownloaded(const QString& versionId);

  static VersionType convertToVersionType(const QString& type);
  static QString convertFromVersionType(const VersionType& versionType);
  static QString getVersionsPath() { return VERSIONS_PATH.path(); };

  static Versions* getInstance();

public slots:
  QList<QVariantMap> getVersionsList(bool snapshot=false,
                                     bool beta=false,
                                     bool alpha=false) const;
private:
  static QUrl findVersionsPath();

  static inline const QUrl VERSIONS_PATH {findVersionsPath()};

  static Versions* s_instance;
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
