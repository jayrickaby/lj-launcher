//
// Created by jay on 24/08/2026.
//

#include "Rule.h"

#include <qversionnumber.h>

bool RuleBearer::isUserSuitable(const OperatingSystem& user) const {
  bool userSuitable{false};


  auto userVer {QVersionNumber::fromString(user.version)};

  for (const auto& rule : rules) {
    bool targetMatches {true};

    if (rule.os.name != SystemName::NONE and rule.os.name != user.name) {
      targetMatches = false;
    }
    if (rule.os.arch != SystemArchitecture::NONE and rule.os.arch != user.arch) {
      targetMatches = false;
    }

    if (!rule.os.versionRange.min.isEmpty() and
      QVersionNumber::fromString(rule.os.versionRange.min) > userVer) {
      targetMatches = false;
    }
    if (!rule.os.versionRange.max.isEmpty() and
      QVersionNumber::fromString(rule.os.versionRange.max) < userVer) {
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