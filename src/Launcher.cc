//
// Created by jay on 12/08/2026.
//

#include "Launcher.h"

Launcher* Launcher::s_instance = nullptr;

Launcher::Launcher(QObject *parent)
  : QObject(parent) {
  s_instance = this;
}

void Launcher::sendError(ErrorMessage& message) {
  if (!s_instance) {
    return;
  }
  if (message.errorFriendly.isEmpty()) {
    message.errorFriendly = "Oops! An error has occurred!";
  }
  emit s_instance->launcherError(message);
}
