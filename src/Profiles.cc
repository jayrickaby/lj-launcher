//
// Created by jay on 13/08/2026.
//

#include "Profiles.h"

#include <algorithm>

#include "Network/Versions.h"

QString Profiles::s_currentProfileId;
Profiles* Profiles::s_instance {nullptr};

Profiles::Profiles(QObject *parent)
  : QObject(parent) {

  if (!s_instance) {
    s_instance = this;
  }

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

QJsonObject Profiles::currentProfile() {
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

QVariantList Profiles::profiles() {
  QVariantList profilesList;
  QJsonObject const PROFILES {getProfiles()};
  for (auto const& id : PROFILES.keys()) {
    auto const& profile = PROFILES[id];

    profilesList.append(QVariantMap{{"id", id}, {"name", profile["name"]}});
  }

  // Sort by creation date, not alphabetically
  std::ranges::sort(profilesList, [](const QVariant& a, const QVariant& b) {
    const QJsonObject PROFILE_A {getProfile(a.toMap()["id"].toString())};
    const QJsonObject PROFILE_B {getProfile(b.toMap()["id"].toString())};

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

QJsonObject Profiles::getJsonFormat() {
  const QJsonObject JSON {
    {"profiles", QJsonObject{}},
    {"settings", QJsonObject{}},
    {"version", 6}
  };
  return JSON;
}

QString Profiles::createProfile(const QString& copyProfileId,
                                QJsonObject parameters,
                                bool defaultTime) {

  QJsonObject const NEW_PROFILE {getDefaultProfile()};
  QJsonObject const CLEANED_PROFILE {cleanProfile(NEW_PROFILE, true)};

  QJsonObject profiles {getProfiles()};
  QString const NEW_UUID {generateUuid()};
  profiles[NEW_UUID] = CLEANED_PROFILE;

  saveProfiles(profiles);

  if (!copyProfileId.isEmpty()) {
    QJsonObject originalProfile{getProfile(copyProfileId)};
    editProfile(NEW_UUID, originalProfile);
  }

  if (!parameters.isEmpty()) {
    editProfile(NEW_UUID, parameters);
  }

  if (!defaultTime) {
    QJsonObject timeParam;
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
  const QJsonObject PROFILE {getProfile(profileId)};

  if (raw) {
    return PROFILE["lastVersionId"].toString();
  }
  if (PROFILE["type"] == "latest-release") {
    return Versions::getLatest();
  }
  if (PROFILE["type"] == "latest-snapshot") {
    return Versions::getLatest(true);
  }
  return PROFILE["lastVersionId"].toString();
}

void Profiles::editProfile(const QString& profileId, QJsonObject& newParameters) {
  QJsonObject target = getProfile(profileId);

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

  QJsonObject profiles {getProfiles()};
  profiles[profileId] = target;
  saveProfiles(profiles);
}

QJsonObject Profiles::getProfile(const QString& profileId) {
  const QJsonObject PROFILES = getProfiles();

  if (!isProfile(profileId)) {
    const QString MSG ("Could not find profile %1");
    throw std::runtime_error(MSG.arg(profileId).toStdString());
  }

  return PROFILES[profileId].toObject();
}

bool Profiles::isProfile(const QString& profileId) {
  QJsonObject const PROFILES = getProfiles();

  return PROFILES.contains(profileId) and PROFILES[profileId].isObject();
}

void Profiles::saveProfiles(const QJsonObject& profiles) {
  QJsonObject json {getJsonData()};
  json["profiles"] = profiles;
  dumpJson(json);
}

void Profiles::dumpJson(const QJsonObject& data) {
  for (auto const key : data.keys()) {
    if (!getJsonFormat().contains(key)) {
      qDebug() << "Invalid key: " << key;
      throw std::runtime_error("Json being dumped is of incorrect format!");
    }
  }
  QJsonDocument const DOC {data};
  QString const RAW {DOC.toJson()};

  if (!System::write(JSON_PATH.toLocalFile(), RAW)) {
    throw std::runtime_error("Failed to write to JSON file!");
  }
}

QJsonObject Profiles::getProfiles() {
  const QJsonObject JSON {getJsonData()};

  if (JSON["profiles"].isObject()) {
    return JSON["profiles"].toObject();
  }

  return {};
}

QJsonObject Profiles::getJsonData() {
  const QString DATA{System::read(JSON_PATH.toLocalFile())};
  QJsonObject const JSON {QJsonDocument::fromJson(DATA.toUtf8()).object()};
  return JSON;
}

QJsonObject Profiles::cleanProfile(const QJsonObject &profile, bool recursive) {
  QJsonObject cleanedProfile;

  for (auto key : profile.keys()) {
    auto value {profile[key]};

    if (value.isDouble() and value.toDouble() == 0.0) { continue; }

    if (value.isString()) {
      value = QJsonValue(value.toString().simplified());
    }

    if (value.isString() and value.toString().isEmpty()) { continue; }

    if (value.isObject() and recursive) {
      value = QJsonValue(cleanProfile(value.toObject(), true));
    }

    if (value.isObject() and value.toObject().isEmpty()) { continue; }

    if (value.isNull()) { continue; }

    cleanedProfile[key] = value;
  }

  return cleanedProfile;
}

QString Profiles::generateUuid() {
  return QUuid::createUuid().toString(QUuid::Id128);
}

QJsonObject Profiles::getDefaultProfile() {
  QJsonObject defaultProfile {getProfileFormat()};

  defaultProfile["name"] = "(Default)";
  defaultProfile["created"] = Launcher::getTime(true);
  defaultProfile["lastUsed"] = Launcher::getTime(true);
  defaultProfile["icon"] = "grass";
  defaultProfile["lastVersionId"] = "latest-release";
  defaultProfile["type"] = "latest-release";

  return defaultProfile;
}

QJsonObject Profiles::getProfileFormat() {
  const QJsonObject JSON {
  {"created", ""},
  {"icon", ""},
  {"gameDir", QJsonValue::Null},
  {"lastUsed", ""},
  {"lastVersionId", ""},
  {"name", ""},
  {"javaDir", QJsonValue::Null},
  {"javaArgs", QJsonValue::Null},
  {
      "resolution", QJsonObject{
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