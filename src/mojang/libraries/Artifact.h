//
// Created by jay on 21/08/2026.
//

#ifndef LJ_LAUNCHER_ARTIFACT_H_
#define LJ_LAUNCHER_ARTIFACT_H_
#include <qtypes.h>

#include <QString>

struct Artifact {
  QString path;
  QString sha1;
  quint64 size;
  QString url;
};

#endif  // LJ_LAUNCHER_ARTIFACT_H_
