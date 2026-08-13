//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILES_H_
#define LJ_LAUNCHER_PROFILES_H_
#include <qqml.h>

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>

#include "Launcher.h"

class Profiles : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Profiles(QObject *parent = nullptr);

  static QString createProfile(
    const QString& copyProfileId={},
    QJsonObject parameters={},
    bool defaultTime=false
  );

private:
  static QUrl findJsonPath();

  static QJsonObject cleanProfile(const QJsonObject &profile, bool recursive=false);

  static void dumpJson(const QJsonObject& data);

  static void editProfile(const QString& profileId, QJsonObject& newParameters);
  static QJsonObject getProfile(const QString& profileId);
  static QJsonObject getProfileFormat();
  static QJsonObject getProfiles();
  static bool isProfile(const QString& profileId);

  static QJsonObject getDefaultProfile();

  static QJsonObject getJsonData();
  static QJsonObject getJsonFormat();

  static void saveProfiles(const QJsonObject& profiles);

  static QString generateUuid();

  static inline const QUrl JSON_PATH {findJsonPath()};
};

#endif  // LJ_LAUNCHER_PROFILES_H_
