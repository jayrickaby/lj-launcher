//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_OPERATINGSYSTEM_H_
#define LJ_LAUNCHER_OPERATINGSYSTEM_H_

enum class SystemName {
  WINDOWS,
  OSX,
  LINUX,
  NONE
};

enum class SystemArchitecture {
  X86,
  NONE
};

struct OperatingSystem {
  SystemArchitecture arch {SystemArchitecture::NONE};
  SystemName name {SystemName::NONE};
};


#endif  // LJ_LAUNCHER_OPERATINGSYSTEM_H_
