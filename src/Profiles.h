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
#include "System.h"

class Profiles : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList profiles READ profiles NOTIFY profilesChanged)
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Profiles(QObject *parent = nullptr);


  QVariantList profiles();

  static void editProfile(const QString& profileId, QJsonObject& newParameters);
  static bool isProfile(const QString& profileId);
  static QString getCurrentProfileId();
  static QString getCurrentProfileVersion();
  static QJsonObject getProfile(const QString& profileId);
  static QJsonObject getProfiles();
  static QString getProfileVersion(const QString& profileId, bool raw=false);

  static QString createProfile(
    const QString& copyProfileId={},
    QJsonObject parameters={},
    bool defaultTime=false
  );

signals:
  void profilesChanged();

private:
  static QJsonObject cleanProfile(const QJsonObject &profile, bool recursive=false);
  static QJsonObject getDefaultProfile();
  static QJsonObject getProfileFormat();
  static void saveProfiles(const QJsonObject& profiles);

  static void dumpJson(const QJsonObject& data);
  static QUrl findJsonPath();
  static QJsonObject getJsonData();
  static QJsonObject getJsonFormat();

  static QString generateUuid();

  static QString s_currentProfileId;
  static inline const QUrl JSON_PATH {findJsonPath()};
};

#endif  // LJ_LAUNCHER_PROFILES_H_
