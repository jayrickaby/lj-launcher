//
// Created by jay on 11/08/2026.
//

#include "Authentication.h"
#include "Settings.h"

Authentication::Authentication(QObject* parent) :
QObject(parent),
pkce_data_(generatePkceData()),
login_data_(getLoginData())
{}

LoginData Authentication::getLoginData() {
  LoginData data;

  QString state = generateSafeToken(16);

  data.url = getCodeUrl(state);
  data.state = state;

  return data;
}

bool Authentication::hasRefreshToken() {
  if (Settings::getRefreshToken().canConvert<QString>()) {
    return true;
  }
  if (!Settings::getRefreshToken().toString().isEmpty()) {
    return true;
  }

  return false;
}

QString Authentication::generateSafeToken(int length) const {
  const QString allowed_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
  QString token;
  token.reserve(length);

  int allowed_length { static_cast<int>(allowed_characters.length()) };

  for (int i = 0; i < length; i++) {
    const int index {QRandomGenerator::global()->bounded(allowed_length)};
    token.append(allowed_characters[index]);
  }

  return token;
}

PkceData Authentication::generatePkceData() const {
  PkceData pkce_data;

  QString token {generateSafeToken(128)};
  pkce_data.code_verifier = token;

  QByteArray hash = QCryptographicHash::hash(
    token.toLatin1(),
    QCryptographicHash::Sha256
    );

  pkce_data.code_challenge = hash.toBase64(
    QByteArray::Base64UrlEncoding
    | QByteArray::OmitTrailingEquals
    );

  return pkce_data;
}

bool Authentication::isUrlLocalhost(const QUrl& url) {
  return url.host() == "localhost";
}

QString Authentication::requestRefreshToken(QString old_token) {
  QUrl url {kTokenUrl};
  QUrlQuery query;
  query.addQueryItem("client_id", kClientId);
  query.addQueryItem("grant_type", "refresh_token");
  query.addQueryItem("scope", kScope);
  query.addQueryItem("refresh_token", old_token);
}
QString Authentication::requestToken() {
  QUrl url {kTokenUrl};
  QUrlQuery query;
  query.addQueryItem("client_id", kClientId);
  query.addQueryItem("grant_type", "refresh_token");
  query.addQueryItem("scope", kScope);
}

QUrl Authentication::getCodeUrl(const QString& state) {
  QUrl url {kAuthUrl};
  QUrlQuery query;
  query.addQueryItem("client_id", kClientId);
  query.addQueryItem("response_type", "code");
  query.addQueryItem("redirect_uri", kRedirectUri.toString());
  query.addQueryItem("scope", kScope);
  query.addQueryItem("response_mode", "query");
  query.addQueryItem("code_challenge", pkce_data_.code_challenge);
  query.addQueryItem("code_challenge_method", pkce_data_.code_challenge_method);
  query.addQueryItem("state", state);
  url.setQuery(query);

  return url;
}

