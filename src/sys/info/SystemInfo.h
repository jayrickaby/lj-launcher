//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_SYSTEMINFO_H_
#define LJ_LAUNCHER_SYSTEMINFO_H_

#include <QString>

#include "OperatingSystem.h"

class SystemInfo {
public:
  [[nodiscard]] static OperatingSystem getOperatingSystem();
};

#endif  // LJ_LAUNCHER_SYSTEMINFO_H_
