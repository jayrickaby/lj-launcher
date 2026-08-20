//
// Created by jay on 14/08/2026.
//

#include "Versions.h"

Versions* Versions::s_instance {nullptr};
Versions::ManifestState Versions::s_state {Versions::ManifestState::MISSING};

Versions::Versions(QObject *parent)
  : NetworkRequester(parent) {
  if (!s_instance) {
    s_instance = this;
  }

  requestManifest();
}

QVariantList Versions::versionsList() {
  const auto AVAILABLE_VERSIONS {getAvailableVersions()};
  // const QJsonObject DOWNLOADED_VERSIONS {getDownloadedVersions()};

  QVariantList versions {};

  for (const auto& version : AVAILABLE_VERSIONS) {
    const QString TYPE {version.value("type").toString()};
    const QString VERSION_ID {version.value("id").toString()};
    const QString VERSION_NAME {
      QString("%1 %2").arg(TYPE, VERSION_ID)
    };

    versions.append(
    QVariantMap {
      {"id", VERSION_ID},
      {"name", VERSION_NAME}
      }
    );
  }
  return versions;
}

QUrl Versions::findVersionsPath() {
  QString const ROOT_PATH {Launcher::getGameDirectory().toLocalFile() + "/versions"};

  QDir ROOT_DIR {ROOT_PATH};
  if (!ROOT_DIR.exists()) {
    ROOT_DIR.mkpath(".");
    qDebug() << "Created versions directory.";
  }

  qDebug() << "Found versions directory:" << ROOT_PATH;
  return {ROOT_PATH};
}

QUrl Versions::findJsonPath() {
  QDir const ROOT_DIR {VERSIONS_PATH.toString()};
  QString const FULL_PATH {ROOT_DIR.filePath("version_manifest_v2.json")};

  if (!System::touch(FULL_PATH, true)) {
    throw std::runtime_error("Could not create versions json!");
  }

  QFile const FILE {FULL_PATH};
  qDebug() << "Found profiles file: " << FULL_PATH;

  QUrl const URL {QUrl::fromLocalFile(FULL_PATH)};
  return URL;
}

QVariantMap Versions::getManifest() {
  const QByteArray JSON_RAW{System::read(JSON_PATH.toLocalFile()).toUtf8()};
  const QJsonDocument JSON_DOC {QJsonDocument::fromJson(JSON_RAW)};
  return JSON_DOC.object().toVariantMap();
}

QList<QVariantMap> Versions::getAvailableVersions(bool snapshot, bool historical) {
  const QVariantMap JSON {getManifest()};

  if (JSON.value("versions").toList().isEmpty()) {
    throw std::runtime_error("Couldn't get any versions.");
  }

  QStringList chosenTypes{"release"};

  if (snapshot) {chosenTypes.append("snapshot");}

  if (historical) {
    chosenTypes.append("old_alpha");
    chosenTypes.append("old_beta");
  }

  QList<QVariantMap> versions;
  const auto ENTRIES (JSON.value("versions").toList());

  for (const auto& entry : ENTRIES) {
    const QVariantMap map {entry.toMap()};

    if (chosenTypes.contains(map.value("type").toString())) {
      versions.emplace_back(map);
    }
  }

  return versions;
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

void Versions::requestManifest() {
  QNetworkRequest const REQUEST {MANIFEST_URL};

  setState(ManifestState::DOWNLOADING);
  Network::get(getInstance(), REQUEST);
}

void Versions::onNetworkReply(QNetworkReply* reply) {
  if (reply->error()) {
    qDebug() << "Error: " << reply->errorString();
    throw std::runtime_error("Couldn't obtain versions manifest!");
    return;
  }

  if (!System::write(JSON_PATH.toLocalFile(), reply->readAll())) {
    throw std::runtime_error("Couldn't write versions manifest to file!");
  }
  setState(ManifestState::PRESENT);
}

Versions* Versions::getInstance() {
  if (!s_instance) {
    s_instance = new Versions();
  }
  return s_instance;
}

QString Versions::getLatestVersion(bool snapshot) {
  const auto JSON {getManifest()};

  if (!JSON.value("latest").toMap().isEmpty()) {
    throw std::runtime_error("Couldn't get latest versions.");
  }

  const QVariantMap LATEST {JSON.value("latest").toMap()};

  return snapshot ? LATEST.value("snapshot").toString() : LATEST.value("release").toString();
}

Versions::ManifestState Versions::getState() {
  return s_state;
}

void Versions::setState(const ManifestState& state) {
  if (s_state == state) { return; }

  s_state = state;
  qDebug() << "Manifest state changed to:" << s_state;
  emit getInstance()->stateChanged();
}

Versions::ManifestState Versions::manifestState() {
  qDebug() << "QML is requesting manifest state:" << s_state;
  return s_state;
}