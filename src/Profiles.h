//
// Created by jay on 13/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILES_H_
#define LJ_LAUNCHER_PROFILES_H_

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <algorithm>

#include "Launcher.h"
#include "ProfileManager.h"
#include "net/Versions.h"

class Profiles : public QObject {
  Q_OBJECT
  Q_PROPERTY(QList<QVariantMap> profiles READ profiles NOTIFY profilesChanged)

  Q_PROPERTY(QString currentProfileId READ currentProfileId WRITE setCurrentProfileId NOTIFY currentProfileIdChanged)

  Q_PROPERTY(QString defaultJavaArgs READ defaultJavaArgs CONSTANT)
  Q_PROPERTY(Resolution defaultResolution READ defaultResolution CONSTANT)

signals:
  void profilesChanged();
  void currentProfileIdChanged();

public:
  explicit Profiles(QObject *parent = nullptr);

  QList<QVariantMap> profiles();
  QString currentProfileId() {return s_currentProfileId; };
  QString defaultJavaArgs() {return Profile::defaultJavaArgs; };
  Resolution defaultResolution() {return Profile::defaultResolution; };

  static QString getCurrentProfileId() {return s_currentProfileId; };

  static void renameCurrentProfileIfDefault();

  static Profiles* getInstance();


public slots:
  void copyProfile(const QString& profileId, const QVariantMap& parameters={});
  void createProfile(const QVariantMap& parameters={});
  void deleteProfile(const QString& profileId);
  void editProfile(const QString& profileId, const QVariantMap& parameters={});
  Profile* getProfile(const QString& profileId);
  void setCurrentProfileId(const QString& profileId);

private:
  static QString s_currentProfileId;

  static Profiles* s_instance;
};

#endif  // LJ_LAUNCHER_PROFILES_H_
