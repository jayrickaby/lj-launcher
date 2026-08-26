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

enum class Feature {
  IS_DEMO_USER,
  HAS_CUSTOM_RESOLUTION,
  HAS_QUICK_PLAYS_SUPPORT,
  IS_QUICK_PLAY_SINGLEPLAYER,
  IS_QUICK_PLAY_MULTIPLAYER,
  IS_QUICK_PLAY_REALMS,
  NONE
};

struct Rule {
  Action action {Action::NONE};
  Feature feature {Feature::NONE};
  OperatingSystem os {};
};

struct RuleBearer {
  QList<Rule> rules;
  [[nodiscard]] bool isUserSuitable(const OperatingSystem& user) const;
};

#endif  // LJ_LAUNCHER_RULE_H_
