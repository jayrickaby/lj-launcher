//
// Created by jay on 24/08/2026.
//

#include "Rule.h"

bool RuleBearer::isUserSuitable(const OperatingSystem& user) const {
  bool userSuitable{true};

  for (const auto& rule : rules) {
    bool targetMatches {true};

    if (rule.os.name != SystemName::NONE and rule.os.name != user.name) {
      targetMatches = false;
    }
    if (rule.os.arch != SystemArchitecture::NONE and rule.os.arch != user.arch) {
      targetMatches = false;
    }

    switch (rule.action) {
      case Action::ALLOW: {
        userSuitable = targetMatches;
        break;
      }
      case Action::DISALLOW: {
        // early return as user explicitly disallowed
        if (targetMatches) {
          return false;
        };
        break;
      }
      default:;
    }
  }
  return userSuitable;
}