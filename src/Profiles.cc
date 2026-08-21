//
// Created by jay on 13/08/2026.
//

#include "Profiles.h"

#include "minecraft/versions/VersionManifest.h"

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

  QFile const FILE {JSON_PATH.toLocalFile()};

  if (FILE.size() == 0) { dumpJson(getJsonFormat()); }

  // Needs to be atleast one profile on startup
  if (getProfiles().isEmpty()) {
    try {
      createProfile({}, {}, true);
    }
    catch (std::runtime_error& e) {
      qDebug() << e.what();
      throw std::runtime_error("Failed to write default values to profiles json!");
    }
  }

  // Current profile on startup should be first
  setCurrentProfileId(getProfiles().keys().first());
}

QVariantMap Profiles::currentProfile() {
  return Profiles::getProfile(getCurrentProfileId());
}

void Profiles::setCurrentProfileId(const QString& profileId) {
  qDebug() << "Setting current profile to:" << profileId;
  if (!isProfile(profileId)) {
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
  const QVariantMap PROFILES {getProfiles()};

  for (const auto & profileId : PROFILES.keys()) {
    const auto & profile = PROFILES[profileId].toMap();

    profilesList.append(QVariantMap{{"id", profileId}, {"name", profile.value("name")}});
  }

  // Sort by creation date, not alphabetically
  std::ranges::sort(profilesList, [](const QVariant& a, const QVariant& b) {
    const QVariantMap PROFILE_A {getProfile(a.toMap()["id"].toString())};
    const QVariantMap PROFILE_B {getProfile(b.toMap()["id"].toString())};

    return PROFILE_A["created"].toString() < PROFILE_B["created"].toString();
  });

  return profilesList;
}

QUrl Profiles::findJsonPath() {
  QString const ROOT_PATH {Launcher::getGameDirectory().toLocalFile()};
  QString const FULL_PATH {QDir(ROOT_PATH).filePath("launcher_profiles.json")};

  if (!System::touch(FULL_PATH, true)) {
    throw std::runtime_error("Could not create profiles json!");
  }

  QFile const FILE {FULL_PATH};
  qDebug() << "Found profiles file: " << FULL_PATH;

  QUrl const PROFILE_URL {QUrl::fromLocalFile(FULL_PATH)};
  return PROFILE_URL;
}

QVariantMap Profiles::getJsonFormat() {
  const QVariantMap JSON {
    {"profiles", QJsonObject{}},
    {"settings", QJsonObject{}},
    {"version", 6}
  };
  return JSON;
}

QString Profiles::createProfile(const QString& copyProfileId,
                                QVariantMap parameters,
                                bool defaultTime) {

  QVariantMap const NEW_PROFILE {getDefaultProfile()};
  QVariantMap const CLEANED_PROFILE {cleanProfile(NEW_PROFILE, true)};

  QVariantMap profiles {getProfiles()};
  QString const NEW_UUID {generateUuid()};
  profiles[NEW_UUID] = CLEANED_PROFILE;

  saveProfiles(profiles);

  if (!copyProfileId.isEmpty()) {
    QVariantMap originalProfile{getProfile(copyProfileId)};
    editProfile(NEW_UUID, originalProfile);
  }

  if (!parameters.isEmpty()) {
    editProfile(NEW_UUID, parameters);
  }

  if (!defaultTime) {
    QVariantMap timeParam;
    timeParam["created"] = Launcher::getTime();

    editProfile(NEW_UUID, timeParam);
  }

  return NEW_UUID;
}

QString Profiles::currentProfileId() {
  return s_currentProfileId;
}

QString Profiles::defaultJavaArgs() {
  return DEFAULT_JAVA_ARGS;
}

QString Profiles::getCurrentProfileVersion() {
  return getProfileVersion(getCurrentProfileId());
}

QString Profiles::getCurrentProfileId() {
  return s_currentProfileId;
}

QString Profiles::getProfileVersion(const QString& profileId, bool raw) {
  const QVariantMap PROFILE {getProfile(profileId)};

  if (raw) {
    return PROFILE["lastVersionId"].toString();
  }
  if (PROFILE["type"] == "latest-release") {
    return VersionManifest::getLatestVersions().release;
  }
  if (PROFILE["type"] == "latest-snapshot") {
    return VersionManifest::getLatestVersions().snapshot;
  }
  return PROFILE["lastVersionId"].toString();
}

void Profiles::editProfile(const QString& profileId, QVariantMap& newParameters) {
  QVariantMap target = getProfile(profileId);

  for (auto const& key : newParameters.keys()) {
    auto value {newParameters[key]};

    if (getProfileFormat().contains(key)) {
      target[key] = value;
      continue;
    }
    const QString MSG {"Key \"%1\" is not a valid profile parameter! Skipping..."};
    qDebug() << MSG.arg(key);
  }

  QStringList const LATEST_VERSIONS{"latest-release", "latest-snapshot"};

  if (LATEST_VERSIONS.contains(target["lastVersionId"])) {
    target["type"] = target["lastVersionId"];
  }
  else {
    target["type"] = "custom";
  }

  target = cleanProfile(target, true);

  QVariantMap profiles {getProfiles()};
  profiles[profileId] = target;
  saveProfiles(profiles);
}

QVariantMap Profiles::getProfile(const QString& profileId) {
  const QVariantMap PROFILES = getProfiles();

  if (!isProfile(profileId)) {
    const QString MSG ("Could not find profile %1");
    throw std::runtime_error(MSG.arg(profileId).toStdString());
  }

  return PROFILES.value(profileId).toMap();
}

bool Profiles::isProfile(const QString& profileId) {
  const QVariantMap PROFILES = getProfiles();

  return PROFILES.contains(profileId)
  and PROFILES.value(profileId).metaType().id() == QMetaType::QVariantMap;
}

void Profiles::saveProfiles(const QVariantMap& profiles) {
  QVariantMap json {getJsonData()};
  json["profiles"] = profiles;
  dumpJson(json);

  emit getInstance()->profilesChanged();
}

void Profiles::dumpJson(const QVariantMap& data) {
  for (const auto& key : data.keys()) {
    if (!getJsonFormat().contains(key)) {
      qDebug() << "Invalid key: " << key;
      throw std::runtime_error("Json being dumped is of incorrect format!");
    }
  }

  QString const RAW {
    QJsonDocument::fromVariant(data).toJson()
  };

  if (!System::write(JSON_PATH.toLocalFile(), RAW)) {
    throw std::runtime_error("Failed to write to JSON file!");
  }
}

QVariantMap Profiles::getProfiles() {
  return getJsonData().value("profiles").toMap();
}

QVariantMap Profiles::getJsonData() {
  const QString DATA{System::read(JSON_PATH.toLocalFile())};

  const QJsonObject JSON {
    QJsonDocument::fromJson(DATA.toUtf8()).object()
  };

  return JSON.toVariantMap();
}

QVariantMap Profiles::cleanProfile(const QVariantMap &profile, bool recursive) {
  QVariantMap cleanedProfile;

  for (const auto& key : profile.keys()) {
    auto value {profile.value(key)};

    if (value.isNull()) { continue; }

    switch (value.metaType().id()) {
      case QMetaType::Int:
      case QMetaType::LongLong: {
        if (value.toLongLong() == 0) {
          continue;
        }
        break;
      }

      case QMetaType::Float:
      case QMetaType::Double: {
        if (value.toDouble() == 0.0) {
          continue;
        }
        break;
      }

      case QMetaType::QString: {
        const QString STR {value.toString().simplified()};
        if (STR.isEmpty()) { continue; }
        value = STR;
        break;
      }

      case QMetaType::QVariantMap: {
        QVariantMap map {value.toMap()};
        if (recursive) {
          map = cleanProfile(map, true);
        }

        if (map.isEmpty()) { continue; }
        value = map;
        break;
      }

      default:
        break;
    }

    cleanedProfile[key] = value;
  }

  return cleanedProfile;
}

QString Profiles::generateUuid() {
  return QUuid::createUuid().toString(QUuid::Id128);
}

QVariantMap Profiles::getDefaultProfile() {
  QVariantMap defaultProfile {getProfileFormat()};

  defaultProfile["name"] = "(Default)";
  defaultProfile["created"] = Launcher::getTime(true);
  defaultProfile["lastUsed"] = Launcher::getTime(true);
  defaultProfile["icon"] = "grass";
  defaultProfile["lastVersionId"] = "latest-release";
  defaultProfile["type"] = "latest-release";

  return defaultProfile;
}

QVariantMap Profiles::getProfileFormat() {
  const QVariantMap JSON {
  {"created", ""},
  {"icon", ""},
  {"gameDir", QJsonValue::Null},
  {"lastUsed", ""},
  {"lastVersionId", ""},
  {"name", ""},
  {"javaDir", QJsonValue::Null},
  {"javaArgs", QJsonValue::Null},
  {
      "resolution", QVariantMap{
      {"width", 0},
      {"height", 0}
      }
    },
    {"type", ""}
  };
  return JSON;
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
  const QString PROFILE_DATE {getCurrentProfile().value("created").toString()};
  const QString PROFILE_NAME {getCurrentProfile().value("name").toString()};

  if (PROFILE_DATE == DEFAULT_DATE and PROFILE_NAME == "(Default)") {
    const QString USERNAME {Launcher::getUsername()};

    QVariantMap params({{"name", USERNAME}});
    editProfile(getCurrentProfileId(), params);
  }
}