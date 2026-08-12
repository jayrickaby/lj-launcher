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

  [[nodiscard]] QString refreshToken() const;
  [[nodiscard]] static QVariant getRefreshToken();

  static void clearRefreshToken();
  static void setRefreshToken(const QString& refreshToken);
  static void setInstance(QSettings* settings) {s_settings = settings;};

private:
  static QSettings* s_settings;
};

#endif  // LJ_LAUNCHER_SETTINGS_H_
