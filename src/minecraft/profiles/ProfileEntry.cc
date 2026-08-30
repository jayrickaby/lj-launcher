//
// Created by jay on 23/08/2026.
//

#include "ProfileEntry.h"

#include <QJsonObject>

#include "Launcher.h"
#include "Resolution.h"
#include "minecraft/ver/VersionManifest.h"

ProfileEntry::ProfileEntry(const QString& uuid, QObject* parent)
  : QObject(parent),
    UUID(uuid) {
  connect(this, &ProfileEntry::nameChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::typeChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::createdChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::lastUsedChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::iconChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::lastVersionIdChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::gameDirChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::javaDirChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::javaArgsChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::resolutionChanged, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::showAlphaVersionsUpdated, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::showBetaVersionsUpdated, &ProfileEntry::profileUpdated);
  connect(this, &ProfileEntry::showSnapshotVersionsUpdated, &ProfileEntry::profileUpdated);
}

ProfileEntry::ProfileEntry(const QString& uuid, const QJsonObject& data, QObject *parent)
  : ProfileEntry(uuid, parent){
  copy(data);
}

ProfileEntry::ProfileEntry(const QString& uuid, const QVariantMap& data, QObject *parent)
  : ProfileEntry(uuid, parent){
  copy(data);
}

void ProfileEntry::copy(const QJsonObject& data) {
  copy(data.toVariantMap());
}

void ProfileEntry::copy(const QVariantMap& data) {
  if (data.contains("name")) {
    setName(data.value("name").toString());
  }

  if (data.contains("lastVersionId")) {
    // can't mix and match latest type with random version
    QString lastVersionId {data.value("lastVersionId").toString()};
    if (lastVersionId == "latest-release") {
      setType(ProfileType::LATEST_RELEASE);
    }
    else if (lastVersionId == "latest-snapshot") {
      setType(ProfileType::LATEST_SNAPSHOT);
    }
    else {
      setType(ProfileType::CUSTOM);
      setLastVersionId(lastVersionId);
    }
  }

  if (data.contains("created")) {
    setCreated(data.value("created").toString());
  }

  if (data.contains("lastUsed")) {
    setLastUsed(data.value("lastUsed").toString());
  }

  if (data.contains("icon")) {
    setIcon(data.value("icon").toString());
  }

  if (data.contains("gameDir")) {
    setGameDir(data.value("gameDir"));
  }

  if (data.contains("javaDir")) {
    setJavaDir(data.value("javaDir"));
  }

  if (data.contains("javaArgs")) {
    setJavaArgs(data.value("javaArgs"));
  }

  if (data.contains("resolution")) {
    setResolution(data.value("resolution"));
  }

  if (data.contains("showAlphaVersions")) {
    setShowAlphaVersions(data.value("showAlphaVersions").toBool());
  }

  if (data.contains("showBetaVersions")) {
    setShowBetaVersions(data.value("showBetaVersions").toBool());
  }

  if (data.contains("showSnapshotVersions")) {
    setShowSnapshotVersions(data.value("showSnapshotVersions").toBool());
  }
}

QVariantMap ProfileEntry::toMap() {
  QVariantMap result;
  result["name"] = getName();

  // can't mix and match latest type with random version
  switch (getType()) {
    case ProfileType::LATEST_SNAPSHOT:
    case ProfileType::LATEST_RELEASE: {
      result["type"] = getLastVersionId();
      result["lastVersionId"] = getLastVersionId();
      break;
    }
    case ProfileType::CUSTOM: {
      result["type"] = "custom";
      result["lastVersionId"] = getLastVersionId();
      break;
    }
  }

  result["created"] = m_created;
  result["lastUsed"] = m_lastUsed;
  result["icon"] = m_icon;

  if (m_gameDir.has_value()) {
    result["gameDir"] = getGameDir();
  }

  if (m_javaDir.has_value()) {
    result["javaDir"] = getJavaDir();
  }

  if (m_javaArgs.has_value()) {
    result["javaArgs"] = getJavaArgs();
  }

  if (m_resolution.has_value()) {
    result["resolution"] = m_resolution.value().toMap();
  }

  result["showAlphaVersions"] = getShowAlphaVersions();
  result["showBetaVersions"] = getShowBetaVersions();
  result["showSnapshotVersions"] = getShowSnapshotVersions();

  return result;
}

QJsonObject ProfileEntry::toJson() {
  return QJsonObject::fromVariantMap(toMap());
}

QString ProfileEntry::getName() const {
  return m_name;
}

ProfileEntry::ProfileType ProfileEntry::getType() const {
  return m_type;
}

QString ProfileEntry::getCreated() const {
  return m_created;
}

QString ProfileEntry::getLastUsed() const {
  return m_lastUsed;
}

QString ProfileEntry::getIcon() const {
  return m_icon;
}

QString ProfileEntry::getLastVersionId() const {
  return m_lastVersionId;
}

QVariant ProfileEntry::getGameDir() const {
  if (m_gameDir.has_value()) {
    return m_gameDir.value();
  }
  return {};
}

QVariant ProfileEntry::getJavaDir() const {
  if (m_javaDir.has_value()) {
    return m_javaDir.value();
  }
  return {};
}

QVariant ProfileEntry::getJavaArgs() const {
  if (m_javaArgs.has_value()) {
    return m_javaArgs.value();
  }
  return {};
}

QVariant ProfileEntry::getResolution() const {
  if (m_resolution.has_value()) {
    return QVariant::fromValue(m_resolution.value());
  }
  return {};
}

bool ProfileEntry::getShowAlphaVersions() const {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);

  qDebug() << "Settings file path:" << settings.fileName();

  return settings.value("showAlphaVersions", false).toBool();
}

bool ProfileEntry::getShowBetaVersions() const {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);


  return settings.value("showBetaVersions", false).toBool();
}

bool ProfileEntry::getShowSnapshotVersions() const {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);

  return settings.value("showSnapshotVersions", false).toBool();
}

void ProfileEntry::setName(const QString& name) {
  if (m_name != name) {
    m_name = name;
    emit nameChanged();
  }
}

void ProfileEntry::setType(const ProfileType& type) {
  if (m_type == type) {
    return;
  }

  m_type = type;
  emit typeChanged();

  switch (m_type) {
    case ProfileType::LATEST_RELEASE:
      setLastVersionId("latest-release");
      break;
    case ProfileType::LATEST_SNAPSHOT:
      setLastVersionId("latest-snapshot");
      break;
    case ProfileType::CUSTOM:
      setLastVersionId(VersionManifest::getLatestVersions().release);
  }
}

void ProfileEntry::setCreated(const QString& created) {
  if (m_created != created) {
    m_created = created;
    emit createdChanged();
  }
}

void ProfileEntry::setLastUsed(const QString& lastUsed) {
  if (m_lastUsed != lastUsed) {
    m_lastUsed = lastUsed;
    emit lastUsedChanged();
  }
}

void ProfileEntry::setIcon(const QString& icon) {
  if (m_icon != icon) {
    m_icon = icon;
    emit iconChanged();
  }
}

void ProfileEntry::setLastVersionId(const QString& lastVersionId) {
  if (m_lastVersionId != lastVersionId) {
    m_lastVersionId = lastVersionId;
    emit lastVersionIdChanged();
  }
}

void ProfileEntry::setGameDir(const QVariant& gameDir) {
  if (gameDir.isNull() or !gameDir.isValid()) {
    m_gameDir = std::nullopt;
    emit gameDirChanged();
    return;
  }

  auto str {gameDir.toString()};
  if (m_gameDir != str) {
    m_gameDir = str;
    emit gameDirChanged();
  }
}

void ProfileEntry::setJavaDir(const QVariant& javaDir) {
  if (javaDir.isNull() or !javaDir.isValid()) {
    m_javaDir = std::nullopt;
    emit javaDirChanged();
    return;
  }

  auto str {javaDir.toString()};
  if (m_javaDir != str) {
    m_javaDir = str;
    emit javaDirChanged();
  }
}

void ProfileEntry::setJavaArgs(const QVariant& javaArgs) {
  if (javaArgs.isNull() or !javaArgs.isValid()) {
    m_javaArgs = std::nullopt;
    emit javaArgsChanged();
    return;
  }

  auto str {javaArgs.toString()};
  if (m_javaArgs != str) {
    m_javaArgs = str;
    emit javaArgsChanged();
  }
}

void ProfileEntry::setResolution(const QVariant& resolution) {
  if (resolution.isNull() or !resolution.isValid()) {
    m_resolution = std::nullopt;
    emit resolutionChanged();
    return;
  }

  auto data {resolution.toMap()};

  Resolution res {data};

  if (!m_resolution.has_value()
    or m_resolution.value().getWidth() != res.getWidth()
    or m_resolution.value().getHeight() != res.getHeight()) {
    m_resolution = res;
    emit resolutionChanged();
  }
}

void ProfileEntry::setShowAlphaVersions(bool showAlphaVersions) {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);

  if (getShowAlphaVersions() != showAlphaVersions) {
    settings.setValue("showAlphaVersions", showAlphaVersions);
    emit showAlphaVersionsUpdated();
  }
}

void ProfileEntry::setShowBetaVersions(bool showBetaVersions) {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);

  if (getShowBetaVersions() != showBetaVersions) {
    settings.setValue("showBetaVersions", showBetaVersions);
    emit showBetaVersionsUpdated();
  }
}

void ProfileEntry::setShowSnapshotVersions(bool showSnapshotVersions) {
  QSettings settings;
  settings.beginGroup("Profiles");
  settings.beginGroup(UUID);

  if (getShowSnapshotVersions() != showSnapshotVersions) {
    settings.setValue("showSnapshotVersions", showSnapshotVersions);
    emit showSnapshotVersionsUpdated();
  }
}