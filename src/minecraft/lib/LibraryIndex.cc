//
// Created by jay on 21/08/2026.
//

#include "LibraryIndex.h"

#include "minecraft/exec/JavaVirtualMachine.h"
#include "minecraft/ver/ClientJson.h"
#include "sys/info/SystemInfo.h"

LibraryIndex::LibraryIndex(const QVariantList& data, QObject *parent)
: NetworkRequester(parent) {
  if (data.isEmpty()) {
    return;
  }

  setState(LibraryIndexState::LOADING);

  m_libraries.clear();
  m_libraries.reserve(data.size());

  for (const auto& rawVariant : data) {
    Library library;

    const QVariantMap RAW_LIBRARY {rawVariant.toMap()};

    library.artifact = parseArtifact(
      RAW_LIBRARY
      .value("downloads").toMap()
      .value("artifact").toMap()
    );

    library.artifact.name = RAW_LIBRARY.value("name").toString();

    const QVariantList RAW_RULES (RAW_LIBRARY.value("rules").toList());
    for (const auto& rawRule : RAW_RULES) {
      const QVariantMap RULE {rawRule.toMap()};
      library.rules.append(ClientJson::parseRule(RULE));
    }
    JavaVirtualMachine::appendVariable("classpath", library.artifact.path, ":");

    m_libraries.enqueue(library);
  }
  setState(LibraryIndexState::LOADED);
}

void LibraryIndex::requestLibraries() {
  qDebug() << "Requesting libraries";

  // Download job 'Version & Libraries' started (x files)
  Launcher::addLog(
    QString("Download job 'Version & Libraries' started (%1 files)")
    .arg(m_libraries.size())
  );

  const OperatingSystem USER_OS {SystemInfo::getOperatingSystem()};

  while (!m_libraries.isEmpty()) {
    const Library LIBRARY {m_libraries.dequeue()};

    // TODO: Move to library parsing
    if (!LIBRARY.isUserSuitable(USER_OS)) {
      qDebug() << "Skipping" << LIBRARY.artifact.name << "due to imposed rules";
      continue;
    }

    if (LIBRARY.artifact.isDownloaded()) {
      qDebug() << "Skipping" << LIBRARY.artifact.name << "due to it already being downloaded!";
      continue;
    }

    expectedLibraryReplies++;

    // Attempting do download x for jobs '...'...
    Launcher::addLog(
      QString("Attempting to download %1 for job 'Versions & Libraries'...")
      .arg(LIBRARY.artifact.path)
    );

    Downloader::addDownload(this, LIBRARY.artifact);

    // Making directory x
    Launcher::addLog(
      QString("Making directory %1")
      .arg(FileSystem::getParentDirectory(LIBRARY.artifact.path))
    );
  }

  if (expectedLibraryReplies == 0) {
    setState(LibraryIndexState::DOWNLOADED);
    setState(LibraryIndexState::INITIALISED);
  }
  else {
    setState(LibraryIndexState::DOWNLOADING);
  }
}

void LibraryIndex::onNetworkReply(QNetworkReply* reply) {
  reply->deleteLater();
  if (reply->error()) {
    throw std::runtime_error("Failed to download library");
  }

  DownloadItem library {reply->property("requestParameters").value<DownloadItem>()};
  // Finished downloading x for job '...': Downloaded successfully and hash matched
  Launcher::addLog(
    QString("Finished downloading %1 for job 'Libraries & Versions': Downloaded successfuly and hash matched")
    .arg(library.path)
  );


  expectedLibraryReplies--;

  if (expectedLibraryReplies == 0) {
    setState(LibraryIndexState::DOWNLOADED);
    setState(LibraryIndexState::INITIALISED);
    Launcher::addLog("Job 'Resources' finished successfully");
  }
}

DownloadItem LibraryIndex::parseArtifact(const QVariantMap& rawArtifact) {
  if (rawArtifact.isEmpty()) {
    return {};
  }

  const QString PATH {rawArtifact.value("path").toString()};

  return DownloadItem{
    .hash = rawArtifact.value("sha1").toString(),
    .id = "",
    .name = "",
    // Artifact is relative to library path, so make absolute
    .path = FileSystem::joinPaths({getLibraryPath(), PATH}),
    .size = rawArtifact.value("size").toULongLong(),
    .totalSize = 0,
    .url = rawArtifact.value("url").toString()
  };
}

QString LibraryIndex::getLibraryPath() {
  return LIBRARY_PATH;
}

QString LibraryIndex::findLibraryPath() {
  return FileSystem::joinPaths(
    { Launcher::getGameDirectory().toLocalFile(), "libraries" }
  );
}

LibraryIndexState LibraryIndex::getState() const {
  return m_state;
}

void LibraryIndex::setState(const LibraryIndexState& state) {
  if (m_state != state) {
    m_state = state;
    emit stateChanged();
  }
}