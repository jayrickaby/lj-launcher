//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_LIBRARY_H_
#define LJ_LAUNCHER_LIBRARY_H_

#include "minecraft/exec/Rule.h"
#include "net/Downloader.h"
#include "sys/info/OperatingSystem.h"

struct Library : public RuleBearer{
  DownloadItem artifact {};

  bool isDownloaded() const;
};

#endif  // LJ_LAUNCHER_LIBRARY_H_
