//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_SETTINGS_H_
#define LJ_LAUNCHER_SETTINGS_H_
#include <qqml.h>
#include <QObject>
#include <QSettings>

#include "Application.h"

class Settings : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(QString refreshToken READ refreshToken)

public:
  explicit Settings(QObject *parent = nullptr);

  QString refreshToken() const;
  static QVariant getRefreshToken();

  static void setInstance(QSettings* settings) {settings_ = settings;};

private:
  static QSettings* settings_;
};

#endif  // LJ_LAUNCHER_SETTINGS_H_
