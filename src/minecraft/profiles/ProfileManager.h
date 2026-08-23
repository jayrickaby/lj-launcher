//
// Created by jay on 23/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILEMANAGER_H_
#define LJ_LAUNCHER_PROFILEMANAGER_H_
#include <QObject>
#include <QString>

#include "Profile.h"

class ProfileManager : public QObject {
  Q_OBJECT

signals:
  void refreshedProfiles();
  void savedProfiles();

public:
  explicit ProfileManager(QObject *parent = nullptr);
  static bool deleteProfile(const QString& profileId);
  static bool editProfile(const QString& profileId, const QVariantMap& parameters);
  static bool isProfile(const QString& profileId);

  static QString copyProfile(const QString& profileId);
  static QString createProfile();
  static QString createProfile(const QVariantMap& parameters);
  static QString getProfilesPath();

  static QSharedPointer<Profile> getProfile(const QString& profileId);

  static ProfileManager* getInstance();

  static QHash<QString, QSharedPointer<Profile>> getProfiles();

  static void refreshProfiles();

private:
  static QString findProfilesPath();
  static QString generateUuid();

  static void saveProfiles();

  static ProfileManager* s_instance;
  static QHash<QString, QSharedPointer<Profile>> s_profiles;
  inline static const QString PROFILES_PATH {findProfilesPath()};
};

#endif  // LJ_LAUNCHER_PROFILEMANAGER_H_
