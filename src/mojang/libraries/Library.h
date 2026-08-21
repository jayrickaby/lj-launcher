//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_LIBRARY_H_
#define LJ_LAUNCHER_LIBRARY_H_
#include <QList>
#include <QString>

#include "Artifact.h"
#include "Rule.h"

struct Library {
  Artifact artifact;
  QString name;
  QList<Rule> rules;

  bool isUserSuitable(const OperatingSystem& user) const;
};

#endif  // LJ_LAUNCHER_LIBRARY_H_
