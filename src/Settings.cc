//
// Created by jay on 11/08/2026.
//

#include "Settings.h"

QSettings* Settings::s_settings = nullptr;

Settings::Settings(QObject* parent)
: QObject(parent)
{}

QString Settings::refreshToken() const {
  return getRefreshToken().toString();
}

QVariant Settings::getRefreshToken() {
  return s_settings->value("Account/refreshToken");
}

void Settings::setRefreshToken(const QString& refreshToken) {
  s_settings->setValue("Account/refreshToken", refreshToken);
}
