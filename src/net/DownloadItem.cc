//
// Created by jay on 22/08/2026.
//

#include "DownloadItem.h"

bool DownloadItem::isDownloaded() const {
  qDebug() << "Checking if item" << url << "is downloaded...";

  if (!FileSystem::isFile(path)) {
    qDebug() << "Item" << url << " does not exist!";
    return false;
  }

  quint64 true_size {FileSystem::getFileSize(path)};

  if (size > 0 and size != true_size) {
    qDebug() << "Item" << url << "does not have the correct size!";
    qDebug() << "Expected:" << size << "got locally:" << true_size;
    return false;
  }

  QString true_hash {System::getSha1Checksum(System::cat(path))};

  if (!hash.isEmpty() and hash != true_hash) {
    qDebug() << "Item" << url << "does not have the correct checksum!";
    qDebug() << "Expected:" << hash << "got locally: " << true_hash;
    return false;
  }

  return true;
}