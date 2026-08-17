//
// Created by jay on 12/08/2026.
//

#ifndef LJ_LAUNCHER_LAUNCHER_H_
#define LJ_LAUNCHER_LAUNCHER_H_

#include <qqml.h>

#include <QDir>
#include <QObject>
#include <QStandardPaths>
#include <QString>
#include <QUrl>

#include "Network/Authentication.h"
#include "Network/Versions.h"
#include "Profiles.h"


struct ErrorMessage {
  Q_GADGET
  Q_PROPERTY(QString errorFriendly MEMBER errorFriendly)
  Q_PROPERTY(QString errorTechnical MEMBER errorTechnical)
  QML_VALUE_TYPE(errormessage)

public:
  QString errorFriendly;
  QString errorTechnical;
};

class Launcher : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString gameDirectory READ gameDirectory CONSTANT)
  Q_PROPERTY(QString javaExecutable READ javaExecutable CONSTANT)
  Q_PROPERTY(QString userMessage READ userMessage NOTIFY userMessageChanged)

public:
  explicit Launcher(QObject *parent = nullptr);

  QString gameDirectory() { return getGameDirectory().toString(); };
  QString javaExecutable() { return getJavaExecutable().toString(); };
  QString userMessage();

  static void sendError(ErrorMessage &message);

  static QUrl getGameDirectory();
  static QUrl getJavaExecutable();
  static QString getUsername();
  static QString getTime(bool def = false);

  static void setUsername(const QString &username);

  static Launcher* getInstance();

signals:
  void launcherError(const ErrorMessage &message);
  void userMessageChanged();
  void usernameChanged();

private:
  static QUrl findGameDirectory();

  static QString s_username;
  static QUrl s_gameDirectory;
  static QUrl s_javaExecutable;

  static Launcher* s_instance;
};

#endif  // LJ_LAUNCHER_LAUNCHER_H_
