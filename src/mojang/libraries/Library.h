//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_LIBRARY_H_
#define LJ_LAUNCHER_LIBRARY_H_
#include <QList>
#include <QString>

#include "sys/info/OperatingSystem.h"

struct Artifact {
  QString path;
  QString sha1;
  quint64 size;
  QString url;
};

enum class Action {
  ALLOW,
  DISALLOW,
  NONE
};

struct Rule {
  Action action {Action::NONE};
  OperatingSystem os;
};

struct Library {
  Artifact artifact;
  QString name;
  QList<Rule> rules;

  bool isUserSuitable(const OperatingSystem& user) const;
};

#endif  // LJ_LAUNCHER_LIBRARY_H_
