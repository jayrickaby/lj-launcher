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
  Q_PROPERTY(QString currentProfileId READ currentProfileId WRITE setCurrentProfileId NOTIFY currentProfileIdChanged)
  Q_PROPERTY(QJsonObject currentProfile READ currentProfile NOTIFY currentProfileIdChanged)
  Q_PROPERTY(QString defaultJavaArgs READ defaultJavaArgs CONSTANT)
  Q_PROPERTY(QJsonObject defaultResolution READ defaultResolution CONSTANT)

signals:
  void profilesChanged();
  void currentProfileIdChanged();

public:
  explicit Profiles(QObject *parent = nullptr);

  QVariantList profiles();
  QJsonObject currentProfile();
  QString currentProfileId();
  QString defaultJavaArgs();
  QJsonObject defaultResolution() {return DEFAULT_RESOLUTION;};

  static void editProfile(const QString& profileId, QJsonObject& newParameters);
  static bool isProfile(const QString& profileId);
  static QString getCurrentProfileId();
  static QString getCurrentProfileVersion();
  static QJsonObject getProfile(const QString& profileId);
  static QJsonObject getProfiles();
  static QString getProfileVersion(const QString& profileId, bool raw=false);

  static Profiles* getInstance();

  static QString createProfile(
    const QString& copyProfileId={},
    QJsonObject parameters={},
    bool defaultTime=false
  );

public slots:
  void addNewProfile(QJsonObject parameters={}) {
    const QString NEW_PROFILE_ID {createProfile(QString(), parameters, false)};
    setCurrentProfileId(NEW_PROFILE_ID);
    emit profilesChanged();
  };
  void editCurrentProfile(QJsonObject parameters={}) {
    editProfile(getCurrentProfileId(), parameters);
    emit profilesChanged();
  };
  void setCurrentProfileId(const QString& profileId);

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

  static inline const QString DEFAULT_JAVA_ARGS {"-Xmx1G -XX:+UseConcMarkSweepGC -XX:+CMSIncrementalMode -XX:-UseAdaptiveSizePolicy -Xmn128M"};
  static inline const QJsonObject DEFAULT_RESOLUTION {
      {"width", 854},
      {"height", 480},
    };

  static Profiles* s_instance;
};

#endif  // LJ_LAUNCHER_PROFILES_H_
