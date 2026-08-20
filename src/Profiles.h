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
  Q_PROPERTY(QList<QVariantMap> profiles READ profiles NOTIFY profilesChanged)

  Q_PROPERTY(QString currentProfileId READ currentProfileId WRITE setCurrentProfileId NOTIFY currentProfileIdChanged)
  Q_PROPERTY(QVariantMap currentProfile READ currentProfile NOTIFY currentProfileIdChanged)
  Q_PROPERTY(QString currentProfileVersion READ currentProfileVersion NOTIFY currentProfileIdChanged)

  Q_PROPERTY(QString defaultJavaArgs READ defaultJavaArgs CONSTANT)
  Q_PROPERTY(QVariantMap defaultResolution READ defaultResolution CONSTANT)

signals:
  void profilesChanged();
  void currentProfileIdChanged();

public:
  explicit Profiles(QObject *parent = nullptr);

  QList<QVariantMap> profiles();
  QVariantMap currentProfile();
  QString currentProfileId();
  QString currentProfileVersion() {return getCurrentProfileVersion();};
  QString defaultJavaArgs();
  QVariantMap defaultResolution() {return DEFAULT_RESOLUTION;};

  static void editProfile(const QString& profileId, QVariantMap& newParameters);
  static bool isProfile(const QString& profileId);
  static QVariantMap getCurrentProfile() { return getProfiles().value(getCurrentProfileId()).toMap(); };
  static QString getCurrentProfileId();
  static QString getCurrentProfileVersion();
  static QVariantMap getProfile(const QString& profileId);
  static QVariantMap getProfiles();
  static QString getProfileVersion(const QString& profileId, bool raw=false);

  static void renameCurrentProfileIfDefault();

  static Profiles* getInstance();

  static QString createProfile(
    const QString& copyProfileId={},
    QVariantMap parameters={},
    bool defaultTime=false
  );

public slots:
  void addNewProfile(QVariantMap parameters={}) {
    const QString NEW_PROFILE_ID {createProfile(QString(), parameters, false)};
    setCurrentProfileId(NEW_PROFILE_ID);
    emit profilesChanged();
  };
  void editCurrentProfile(QVariantMap parameters={}) {
    editProfile(getCurrentProfileId(), parameters);
    emit profilesChanged();
  };
  void setCurrentProfileId(const QString& profileId);

private:
  static QVariantMap cleanProfile(const QVariantMap &profile, bool recursive=false);
  static QVariantMap getDefaultProfile();
  static QVariantMap getProfileFormat();
  static void saveProfiles(const QVariantMap& profiles);

  static void dumpJson(const QVariantMap& data);
  static QUrl findJsonPath();
  static QVariantMap getJsonData();
  static QVariantMap getJsonFormat();

  static QString generateUuid();

  static QString s_currentProfileId;
  static inline const QUrl JSON_PATH {findJsonPath()};

  static inline const QString DEFAULT_JAVA_ARGS {R"("-Xms2G", "-Xmx4G", "-XX:+UseCompactObjectHeaders", "-XX:+AlwaysPreTouch", "-XX:+UseStringDeduplication")"};
  static inline const QVariantMap DEFAULT_RESOLUTION {
      {"width", 854},
      {"height", 480},
    };

  static Profiles* s_instance;
};

#endif  // LJ_LAUNCHER_PROFILES_H_
