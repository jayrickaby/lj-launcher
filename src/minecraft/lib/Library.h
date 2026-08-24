//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_LIBRARY_H_
#define LJ_LAUNCHER_LIBRARY_H_

#include "minecraft/exec/Rule.h"
#include "net/Downloader.h"
#include "sys/info/OperatingSystem.h"

struct Library {
  DownloadItem artifact {};
  QList<Rule> rules;

  bool isDownloaded() const;
  bool isUserSuitable(const OperatingSystem& user) const;
};

#endif  // LJ_LAUNCHER_LIBRARY_H_
