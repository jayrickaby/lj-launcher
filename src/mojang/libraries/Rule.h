//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_RULES_H_
#define LJ_LAUNCHER_RULES_H_
#include <QString>

#include "../../sys/info/OperatingSystem.h"

enum class Action {
  ALLOW,
  DISALLOW,
  NONE
};

struct Rule {
  Action action {Action::NONE};
  OperatingSystem os;
};

#endif  // LJ_LAUNCHER_RULES_H_
