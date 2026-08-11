//
// Created by jay on 11/08/2026.
//

#include "Authentication.h"
#include "Settings.h"

Authentication::Authentication(QObject* parent) :
QObject(parent),
login_data_(getLoginData())
{}

LoginData Authentication::getLoginData() const {
  LoginData data;

  PkceData pkce {generatePkceData()};
  data.code_verifier = pkce.code_verifier;

  data.state = generateToken(16);

  QUrl url {kAuthUrl};
  QUrlQuery query;
  query.addQueryItem("client_id", kClientId);
  query.addQueryItem("response_type", "code");
  query.addQueryItem("redirect_uri", kRedirectUri.toString());
  query.addQueryItem("response_mode", "query");
  query.addQueryItem("scope", kScope);
  query.addQueryItem("state", data.state);
  query.addQueryItem("code_challenge", pkce.code_challenge);
  query.addQueryItem("code_challenge_method", pkce.code_challenge_method);
  url.setQuery(query);

  data.url = url;

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

QString Authentication::generateToken(int length) const {
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

  QString token {generateToken(128)};
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