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

enum class LibraryIndexState {
  UNINITIALISED,
  LOADING,
  LOADED,
  DOWNLOADING,
  DOWNLOADED,
  INITIALISED
};

class LibraryIndex : public NetworkRequester {
  Q_OBJECT

signals:
  void stateChanged();

public:
  explicit LibraryIndex(const QVariantList& data, QObject *parent = nullptr);

  static QString getLibraryPath();
  LibraryIndexState getState() const;

  void requestLibraries();

  void onNetworkReply(QNetworkReply* reply) override;

private:
  DownloadItem parseArtifact(const QVariantMap& rawArtifact);
  Rule parseRule(const QVariantMap& rawRule);
  Action parseAction(const QString& rawAction);
  OperatingSystem parseOs(const QVariantMap& rawOs);
  void setState(const LibraryIndexState& state);

  quint64 expectedLibraryReplies {0};

  QQueue<Library> m_libraries;
  LibraryIndexState m_state{LibraryIndexState::UNINITIALISED};

  static QString findLibraryPath();
  inline static const QString LIBRARY_PATH {findLibraryPath()};
};

#endif  // LJ_LAUNCHER_LIBRARYINDEX_H_
