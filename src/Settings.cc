//
// Created by jay on 11/08/2026.
//

#include "Settings.h"

QSettings* Settings::settings_ = nullptr;

Settings::Settings(QObject* parent)
: QObject(parent)
{}

QString Settings::refreshToken() const {
  return getRefreshToken().toString();
}

QVariant Settings::getRefreshToken() {
  return settings_->value("Account/refreshToken");
}
