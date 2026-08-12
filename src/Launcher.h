//
// Created by jay on 12/08/2026.
//

#ifndef LJ_LAUNCHER_LAUNCHER_H_
#define LJ_LAUNCHER_LAUNCHER_H_

#include <qqml.h>
#include <QObject>

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
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Launcher(QObject *parent = nullptr);

  static void sendError(ErrorMessage &message);

signals:
  void launcherError(const ErrorMessage &message);

private:
  static Launcher* s_instance;
};

#endif  // LJ_LAUNCHER_LAUNCHER_H_
