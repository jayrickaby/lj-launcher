//
// Created by jay on 13/08/2026.
//

#include "Profiles.h"

#include "minecraft/ver/VersionManifest.h"

QString Profiles::s_currentProfileId;
Profiles* Profiles::s_instance {nullptr};

Profiles::Profiles(QObject *parent)
  : QObject(parent) {

  if (!s_instance) {
    s_instance = this;
  }

  connect(Authentication::getInstance(), &Authentication::authStateChanged,
    this, &Profiles::renameCurrentProfileIfDefault);

  connect(this, &Profiles::currentProfileIdChanged,
  this, &Profiles::renameCurrentProfileIfDefault);

  connect (ProfileManager::getInstance(), &ProfileManager::savedProfiles,
    this, &Profiles::profilesChanged);

  // Current profile on startup should be first
  setCurrentProfileId(ProfileManager::getProfiles().keys().first());
  renameCurrentProfileIfDefault();
}


void Profiles::setCurrentProfileId(const QString& profileId) {
  qDebug() << "Setting current profile to:" << profileId;
  if (!ProfileManager::isProfile(profileId)) {
    qDebug() << "Tried to set current profile to one that doesn't exist!";
    return;
  }
  if (s_currentProfileId == profileId) {
    return;
  }

  s_currentProfileId = profileId;
  emit currentProfileIdChanged();
}

QList<QVariantMap> Profiles::profiles() {
  QList<QVariantMap> profilesList;
  auto profiles {ProfileManager::getProfiles()};

  for (const auto& [profileId, profile] : profiles.asKeyValueRange()) {
    profilesList.append(
      QVariantMap{
      {"id", profileId},
      {"name", profile->getName()},
      {"created", profile->getCreated()},
      }
    );
  }

  // Sort by creation date, not alphabetically
  std::ranges::sort(profilesList, [](QVariantMap& a, QVariantMap& b) {
    auto aCreated = a.value("created").toString();
    a.remove("created");
    auto bCreated = b.value("created").toString();
    b.remove("created");

    return aCreated < bCreated;
  });

  if (!profiles.contains(s_currentProfileId)) {
    setCurrentProfileId(profilesList.first().value("id").toString());
  }

  return profilesList;
}

Profiles* Profiles::getInstance() {
  if (!s_instance) {
    s_instance = new Profiles;
  }
  return s_instance;
}

void Profiles::renameCurrentProfileIfDefault() {
  const bool AUTHENTICATED {Authentication::getAuthState() == Authentication::AuthState::AUTHENTICATED};

  if (!AUTHENTICATED) {
    return;
  }

  const QString DEFAULT_DATE {Launcher::getTime(true)};
  const QString PROFILE_DATE {ProfileManager::getProfile(s_currentProfileId)->getCreated()};
  const QString PROFILE_NAME {ProfileManager::getProfile(s_currentProfileId)->getName()};

  if (PROFILE_DATE == DEFAULT_DATE and PROFILE_NAME == "(Default)") {
    const QString USERNAME {Launcher::getUsername()};

    QVariantMap params({{"name", USERNAME}});
    ProfileManager::editProfile(s_currentProfileId, params);
  }
}

void Profiles::copyProfile(const QString& profileId, const QVariantMap& parameters) {
  ProfileManager::copyProfile(profileId);
}
void Profiles::createProfile(QVariantMap parameters) {
  parameters["created"] = Launcher::getTime();

  ProfileManager::createProfile(parameters);
};
void Profiles::deleteProfile(const QString& profileId) {
  ProfileManager::deleteProfile(profileId);
};
void Profiles::editProfile(const QString& profileId, const QVariantMap& parameters) {
  ProfileManager::editProfile(s_currentProfileId, parameters);
};
ProfileEntry* Profiles::getProfile(const QString& profileId) {
  if (!ProfileManager::isProfile(profileId)) {
    return nullptr;
  }

  return ProfileManager::getProfile(profileId).data();
};