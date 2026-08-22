//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_LIBRARYINDEX_H_
#define LJ_LAUNCHER_LIBRARYINDEX_H_
#include <QList>
#include <QVariant>

#include "Launcher.h"
#include "Library.h"
#include "sys/io/FileSystem.h"

class LibraryIndex {
public:
  LibraryIndex(const QVariantList& data);

  static QString getLibraryPath();

  QList<Library> getLibraries() const;
private:
  Artifact parseArtifact(const QVariantMap& rawArtifact);
  Rule parseRule(const QVariantMap& rawRule);
  Action parseAction(const QString& rawAction);
  OperatingSystem parseOs(const QVariantMap& rawOs);

  QList<Library> m_libraries;

  static QString findLibraryPath();
  inline static const QString LIBRARY_PATH {findLibraryPath()};
};

#endif  // LJ_LAUNCHER_LIBRARYINDEX_H_
