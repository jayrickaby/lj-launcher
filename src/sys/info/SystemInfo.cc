//
// Created by jay on 21/08/2026.
//

#include "SystemInfo.h"

OperatingSystem SystemInfo::getOperatingSystem() {
  OperatingSystem system;
  const QString NAME {QSysInfo::kernelType()};

  if (NAME == "winnt") { system.name == SystemName::WINDOWS; }
  else if (NAME == "darwin") { system.name == SystemName::OSX; }
  else if (NAME == "linux") { system.name == SystemName::LINUX; }
  else { system.name == SystemName::NONE; }

  const QString ARCH {QSysInfo::currentCpuArchitecture()};

  if (ARCH == "i386") { system.arch == SystemArchitecture::X86; }
  else { system.arch == SystemArchitecture::NONE; }

  return system;
}
