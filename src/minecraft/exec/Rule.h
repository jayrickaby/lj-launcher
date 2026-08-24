//
// Created by jay on 24/08/2026.
//

#ifndef LJ_LAUNCHER_RULE_H_
#define LJ_LAUNCHER_RULE_H_
#include "sys/info/OperatingSystem.h"

enum class Action {
  ALLOW,
  DISALLOW,
  NONE
};

struct Rule {
  Action action {Action::NONE};
  OperatingSystem os {};
};

#endif  // LJ_LAUNCHER_RULE_H_
