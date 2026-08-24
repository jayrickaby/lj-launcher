//
// Created by jay on 24/08/2026.
//

#ifndef LJ_LAUNCHER_ARGUMENT_H_
#define LJ_LAUNCHER_ARGUMENT_H_

#include "Rule.h"

struct Argument {
  QStringList values;
  QList<Rule> rules;
};

#endif  // LJ_LAUNCHER_ARGUMENT_H_
