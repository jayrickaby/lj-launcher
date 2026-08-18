//
// Created by jay on 14/08/2026.
//

#include "Versions.h"

Versions* Versions::s_instance {nullptr};
Versions::ManifestState Versions::s_state {Versions::ManifestState::MISSING};

Versions::Versions(QObject *parent)
  : NetworkRequester(parent)
{
  if (!s_instance) {
    s_instance = this;
  }

  requestManifest();
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

QString Versions::getLatest(bool snapshot) {
  const QByteArray JSON_RAW{System::read(JSON_PATH.toLocalFile()).toUtf8()};
  const QJsonDocument JSON_DOC {QJsonDocument::fromJson(JSON_RAW)};
  const QJsonObject JSON {JSON_DOC.object()};

  if (!JSON.contains("latest")
    or !JSON["latest"].isObject()
    or JSON["latest"].toObject().isEmpty()) {
    throw std::runtime_error("Couldn't get latest versions.");
  }

  const QJsonObject LATEST {JSON["latest"].toObject()};


  return snapshot ? LATEST["snapshot"].toString() : LATEST["release"].toString();
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