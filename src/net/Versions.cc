//
// Created by jay on 14/08/2026.
//

#include "Versions.h"

#include "minecraft/ver/VersionManifest.h"

Versions* Versions::s_instance {nullptr};

Versions::Versions(QObject *parent)
  : NetworkRequester(parent) {}

QList<QVariantMap> Versions::versionsList() {
  const auto AVAILABLE_VERSIONS{
    VersionManifest::getVersions({VersionType::RELEASE})
  };

  QList<QVariantMap> versions{};

  versions.append(
    QVariantMap{
    {"id", "latest-release"},
    {"name", "(Use Latest Release)"}
    }
  );

  for (const auto& version : AVAILABLE_VERSIONS) {
    QString type {convertFromVersionType(version.type)};

    const QString VERSION_NAME{QString("%1 %2").arg(type, version.item.id)};

    versions.append(QVariantMap{{"id", version.item.id}, {"name", VERSION_NAME}});
  }
  return versions;
}

QString Versions::convertFromVersionType(const VersionType& versionType) {
  switch (versionType) {
    case VersionType::RELEASE:
      return "release";
    case VersionType::SNAPSHOT:
      return "snapshot";
    case VersionType::OLD_ALPHA:
      return "old_alpha";
      break;
    case VersionType::OLD_BETA:
      return "old_beta";
      break;
  }

  qDebug() << "Unknown version type:" << static_cast<int>(versionType);
  return "release";
}

VersionType Versions::convertToVersionType(const QString& type) {
  if (type == "release") {
    return VersionType::RELEASE;
  }
  if (type == "snapshot") {
    return VersionType::SNAPSHOT;
  }
  if (type == "old_alpha" or type == "old-alpha") {
    return VersionType::OLD_ALPHA;
  }
  if (type == "old_beta" or type == "old-beta") {
    return VersionType::OLD_BETA;
  }

  qDebug() << "Unknown version type:" << type;
  return VersionType::RELEASE;
}

QUrl Versions::findVersionsPath() {
  QString const ROOT_PATH {FileSystem::joinPaths({Launcher::getGameDirectory().toLocalFile(), "versions"})};

  FileSystem::makePath(ROOT_PATH);

  qDebug() << "Found ver directory:" << ROOT_PATH;
  return {ROOT_PATH};
}

QVariantMap Versions::getDownloadedVersion(const QString& versionId) {
  for (const QVariantMap& entry : getDownloadedVersions()) {
    if (entry.value("id") == versionId) {
      return entry;
    }
  }
  return {};
}

bool Versions::isDownloaded(const QString& versionId) {
  return std::ranges::any_of(getDownloadedVersions(), [&versionId](const auto& entry) {
    return entry.value("id").toString() == versionId;
  });
}

QList<QVariantMap> Versions::getDownloadedVersions() {
  QDirIterator iterator(VERSIONS_PATH.toString(),
    QDir::Dirs | QDir::NoDotAndDotDot);

  QList<QVariantMap> versions;
  while (iterator.hasNext()) {
    const QString FOLDER {iterator.next()};
    const QDir FOLDER_DIR {FOLDER};
    const QString VERSION {FOLDER_DIR.dirName()};
    const QString JSON_PATH {
      FOLDER_DIR.filePath(
        QString("%1.json")
        .arg(VERSION)
      )
    };

    if (QDir(JSON_PATH).exists()) {
      qDebug() << "Found version:" << VERSION;
      versions.append (
          QJsonDocument::fromJson(FileSystem::read(JSON_PATH).toUtf8())
          .object()
          .toVariantMap()
      );
    }
  }

  if (versions.empty()) {
    qDebug() << "No downloaded versions found.";
  }

  return versions;
}

Versions* Versions::getInstance() {
  if (!s_instance) {
    s_instance = new Versions();
  }
  return s_instance;
}
