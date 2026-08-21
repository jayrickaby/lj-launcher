//
// Created by jay on 14/08/2026.
//

#include "Versions.h"

#include "minecraft/versions/VersionManifest.h"

Versions* Versions::s_instance {nullptr};

Versions::Versions(QObject *parent)
  : NetworkRequester(parent) {}

QList<QVariantMap> Versions::versionsList() {
  const QList<ManifestEntry> AVAILABLE_VERSIONS{VersionManifest::getVersions({VersionType::RELEASE})};

  QList<QVariantMap> versions{};

  for (const auto& version : AVAILABLE_VERSIONS) {
    QString type;
    switch (version.type) {
      case VersionType::RELEASE:
        type = "release";
        break;
      case VersionType::SNAPSHOT:
        type = "snapshot";
        break;
      case VersionType::OLD_ALPHA:
        type = "old-alpha";
        break;
      case VersionType::OLD_BETA:
        type = "old-beta";
        break;
    }

    const QString VERSION_NAME{QString("%1 %2").arg(type, version.id)};

    versions.append(QVariantMap{{"id", version.id}, {"name", VERSION_NAME}});
  }
  return versions;
}

QUrl Versions::findVersionsPath() {
  QString const ROOT_PATH {FileSystem::joinPath(Launcher::getGameDirectory().toLocalFile(), "/versions")};

  FileSystem::makePath(ROOT_PATH);

  qDebug() << "Found versions directory:" << ROOT_PATH;
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
          QJsonDocument::fromJson(System::read(JSON_PATH).toUtf8())
          .object()
          .toVariantMap()
      );
    }
  }

  if (versions.empty()) {
    qDebug() << "No versions found.";
  }

  return versions;
}

Versions* Versions::getInstance() {
  if (!s_instance) {
    s_instance = new Versions();
  }
  return s_instance;
}
