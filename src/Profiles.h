//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILES_H_
#define LJ_LAUNCHER_PROFILES_H_
#include <qqml.h>

#include <QDir>
#include <QFileInfo>
#include <QJsonObject>
#include <QObject>

#include "Launcher.h"

class Profiles : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Profiles(QObject *parent = nullptr);


private:
  static QUrl findJsonPath();
  static QJsonObject getDefaultJson();

  static inline const QUrl JSON_PATH {findJsonPath()};
};

#endif  // LJ_LAUNCHER_PROFILES_H_
