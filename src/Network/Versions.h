//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_VERSIONS_H_
#define LJ_LAUNCHER_VERSIONS_H_

#include <QString>
#include <QUrl>

#include "../Launcher.h"
#include "../System.h"

class Versions {

private:
  static QUrl findJsonPath();

  static inline const QUrl JSON_PATH {findJsonPath()};
};

#endif  // LJ_LAUNCHER_VERSIONS_H_
