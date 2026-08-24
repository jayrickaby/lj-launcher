//
// Created by jay on 24/08/2026.
//

#ifndef LJ_LAUNCHER_RULE_H_
#define LJ_LAUNCHER_RULE_H_
#include <QList>

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

struct RuleBearer {
  QList<Rule> rules;
  bool isUserSuitable(const OperatingSystem& user) const;
};

#endif  // LJ_LAUNCHER_RULE_H_
