//
// Created by jay on 11/08/2026.
//

#include "Settings.h"

QSettings* Settings::s_settings{new QSettings(Application::getOrganisationName(), Application::getApplicationName())};
Settings* Settings::s_instance{nullptr};

Settings::Settings(QObject* parent)
: QObject(parent)
{
  if (!s_instance) {
  s_instance = this;
}
}

QString Settings::refreshToken() const {
  return getRefreshToken().toString();
}

QVariant Settings::getRefreshToken() {
  return s_settings->value("Account/refreshToken");
}

void Settings::setRefreshToken(const QString& refreshToken) {
  s_settings->setValue("Account/refreshToken", refreshToken);
}

void Settings::clearRefreshToken() {
  s_settings->remove("Account/refreshToken");
}

Settings* Settings::getInstance() {
  if (!s_instance) {
    s_instance = new Settings();
  }
  return s_instance;
}

