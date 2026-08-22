//
// Created by jay on 22/08/2026.
//

#ifndef LJ_LAUNCHER_DOWNLOADITEM_H_
#define LJ_LAUNCHER_DOWNLOADITEM_H_
#include <QDebug>
#include <QString>

#include "sys/io/FileSystem.h"
#include "System.h"

struct DownloadItem {
  QString hash {""};
  QString id {""};
  QString name {""};
  QString path {""};
  quint64 size {0};
  quint64 totalSize {0};
  QString url {""};

  [[nodiscard]] bool isDownloaded() const;
};

#endif  // LJ_LAUNCHER_DOWNLOADITEM_H_
