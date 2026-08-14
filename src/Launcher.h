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

#include "Launcher.h"
#include "Network/Authentication.h"

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
  Q_PROPERTY(QString userMessage READ userMessage NOTIFY userMessageChanged)
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Launcher(QObject *parent = nullptr);

  QString userMessage();

  static void sendError(ErrorMessage &message);

  static QUrl getGameDirectory();
  static QString getUsername();
  static QString getTime(bool def = false);

  static void setUsername(const QString &username);

  static Launcher* getInstance() { return s_instance; };

signals:
  void launcherError(const ErrorMessage &message);
  void userMessageChanged();
  void usernameChanged();

private:
  static QUrl findGameDirectory();

  static QString s_username;
  static QUrl s_gameDirectory;

  static Launcher* s_instance;
};

#endif  // LJ_LAUNCHER_LAUNCHER_H_
