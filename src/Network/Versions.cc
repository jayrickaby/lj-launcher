//
// Created by jay on 14/08/2026.
//

#include "Versions.h"

QUrl Versions::findJsonPath() {
  QString const ROOT_PATH {Launcher::getGameDirectory().toLocalFile() + "/versions"};
  QDir const ROOT_DIR {ROOT_PATH};
  QString const FULL_PATH {ROOT_DIR.filePath("version_manifest_v2.json")};

  if (!ROOT_DIR.exists()) {
    ROOT_DIR.mkpath(".");
    qDebug() << "Created versions directory.";
  }

  if (!System::touch(FULL_PATH, true)) {
    throw std::runtime_error("Could not create versions json!");
  }

  QFile const FILE {FULL_PATH};
  qDebug() << "Found profiles file: " << FULL_PATH;

  QUrl const MANIFEST_URL {QUrl::fromLocalFile(FULL_PATH)};
  return MANIFEST_URL;
}
