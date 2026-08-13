//
// Created by jay on 13/08/2026.
//

#include "Profiles.h"

#include "System.h"

Profiles::Profiles(QObject *parent)
  : QObject(parent)
{}

QUrl Profiles::findJsonPath() {
  QString const ROOT_PATH {Launcher::getGameDirectory().toLocalFile()};
  QString const FULL_PATH {QDir(ROOT_PATH).filePath("launcher_profiles.json")};

  QFileInfo const FILE_INFO {FULL_PATH};

  if (FILE_INFO.exists() and FILE_INFO.isDir()) {
    qDebug() << "Removing directory " << FULL_PATH;
    QDir dir{FULL_PATH};
    if (!dir.removeRecursively()) {
      QString const MSG {"Could not remove directory " + FULL_PATH};
      throw std::runtime_error(MSG.toUtf8());
    }
    QFile::remove(FULL_PATH);
  }

  if (!System::touch(FULL_PATH, true)) {
    throw std::runtime_error("Could not create profiles json!");
  };
  qDebug() << "Found profiles file: " << FULL_PATH;

  QUrl const PROFILE_URL {QUrl::fromLocalFile(FULL_PATH)};
  return PROFILE_URL;
}