//
// Created by jay on 23/08/2026.
//

#include "ProfileManager.h"

#include "Launcher.h"
#include "sys/io/JsonUtils.h"

ProfileManager* ProfileManager::s_instance {nullptr};
QHash<QString, QSharedPointer<Profile>> ProfileManager::s_profiles;

ProfileManager::ProfileManager(QObject* parent)
:QObject(parent) {
  if (!s_instance) {
    s_instance = this;
  }
}

bool ProfileManager::deleteProfile(const QString& profileId) {
  if (!isProfile(profileId)) {
    return false;
  }

  bool result {s_profiles.remove(profileId)};

  if (s_profiles.empty()) {
    createProfile();
  }

  saveProfiles();
  return result;
}

bool ProfileManager::editProfile(const QString& profileId,
                                 const QVariantMap& parameters) {
  if (!s_profiles.contains(profileId)) {
    return false;
  }

  auto profile {s_profiles.value(profileId)};

  if (profile) {
    profile->copy(parameters);
    saveProfiles();
    return true;
  }

  return false;
}

bool ProfileManager::isProfile(const QString& profileId) {
  return s_profiles.contains(profileId);
}

QString ProfileManager::copyProfile(const QString& profileId) {
  if (!s_profiles.contains(profileId)) {
    return {};
  }

  auto profile {s_profiles.value(profileId)};

  if (profile) {
    return createProfile(profile->toMap());
  }

  return {};
}

QString ProfileManager::createProfile() {
  return createProfile({});
}

QString ProfileManager::createProfile(const QVariantMap& parameters) {
  QString uuid {generateUuid()};

  s_profiles.insert(uuid, QSharedPointer<Profile>::create(parameters));
  saveProfiles();
  return uuid;
}

QString ProfileManager::findProfilesPath() {
  return FileSystem::joinPaths({Launcher::getGameDirectory().toLocalFile(), "launcher_profiles.json"});
}

QString ProfileManager::generateUuid() {
  return QUuid::createUuid().toString(QUuid::Id128);
}

QString ProfileManager::getProfilesPath() {
  return PROFILES_PATH;
}

QSharedPointer<Profile> ProfileManager::getProfile(const QString& profileId) {
  if (!s_profiles.contains(profileId)) {
    return QSharedPointer<Profile>::create();
  }

  return s_profiles.value(profileId);
}

ProfileManager* ProfileManager::getInstance() {
  if (!s_instance) {
    s_instance = new ProfileManager();
  }

  return s_instance;
}

QHash<QString, QSharedPointer<Profile>> ProfileManager::getProfiles() {
  return s_profiles;
}

void ProfileManager::refreshProfiles() {
  QVariantMap data {JsonUtils::readJson(PROFILES_PATH).toVariantMap()};

  if (data.empty()
    or !data.contains("profiles")
    or !data.value("profiles").isValid()
    or data.value("profiles").isNull()) {
    createProfile();

    // get it again since it updated
    data = JsonUtils::readJson(PROFILES_PATH).toVariantMap();
  }

  auto entries {data.value("profiles").toMap()};

  s_profiles.clear();
  s_profiles.reserve(entries.count());

  for (const auto& profileId : entries.keys()) {
    auto profileData {entries.value(profileId).toMap()};

    auto profile {QSharedPointer<Profile>::create(profileData)};
    auto instance {getInstance()};

    connect(profile.get(), &Profile::profileUpdated, instance, [instance, profileId] {
      emit instance->profileUpdated(profileId);
    });

    s_profiles.insert(profileId, profile);
  }

  emit getInstance()->refreshedProfiles();
}

void ProfileManager::saveProfiles() {
  QVariantMap profiles {};

  for (const auto& profileId : s_profiles.keys()) {
    profiles[profileId] = getProfile(profileId)->toMap();
  }

  auto data {JsonUtils::readJson(PROFILES_PATH).toVariantMap()};
  data["profiles"] = QVariant::fromValue(profiles);
  JsonUtils::writeJson(PROFILES_PATH, data);

  emit getInstance()->savedProfiles();
}
