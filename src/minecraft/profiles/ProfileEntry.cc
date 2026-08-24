//
// Created by jay on 23/08/2026.
//

#include <QJsonObject>

#include "Launcher.h"
#include "ProfileEntry.h"

ProfileEntry::ProfileEntry(QObject* parent)
  : QObject(parent) {
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
}

ProfileEntry::ProfileEntry(const QJsonObject& data, QObject *parent)
  : ProfileEntry(parent){
  copy(data);
}

ProfileEntry::ProfileEntry(const QVariantMap& data, QObject *parent)
  : ProfileEntry(parent){
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
    }
    setLastVersionId(lastVersionId);
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
    result["resolution"] = getResolution();
  }
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

void ProfileEntry::setName(const QString& name) {
  if (m_name != name) {
    m_name = name;
    emit nameChanged();
  }
}

void ProfileEntry::setType(const ProfileType& type) {
  if (m_type != type) {
    m_type = type;
    emit typeChanged();
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
  auto str {gameDir.toString()};
  if (gameDir.isValid() and !gameDir.isNull() and m_gameDir != str) {
    m_gameDir = str;
    emit gameDirChanged();
  }
}

void ProfileEntry::setJavaDir(const QVariant& javaDir) {
  auto str {javaDir.toString()};
  if (javaDir.isValid() and !javaDir.isNull() and m_javaDir != str) {
    m_javaDir = str;
    emit javaDirChanged();
  }
}

void ProfileEntry::setJavaArgs(const QVariant& javaArgs) {
  auto str {javaArgs.toString()};
  if (javaArgs.isValid() and !javaArgs.isNull() and m_javaArgs != str) {
    m_javaArgs = str;
    emit javaArgsChanged();
  }
}

void ProfileEntry::setResolution(const QVariant& resolution) {
  auto res = resolution.value<Resolution>();
  if (resolution.isValid()
    and !resolution.isNull()
    and (m_resolution->width != res.width
    and m_resolution->height != res.height)) {
    m_resolution = res;
    emit resolutionChanged();
  }
}