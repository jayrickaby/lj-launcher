//
// Created by jay on 11/08/2026.
//

#include "Authentication.h"

#include <iostream>

#include "Settings.h"

Authentication::Authentication(QObject* parent) :
QObject(parent),
pkce_data_(generatePkceData()),
login_data_(getLoginData()) {
  connect(&network_manager_, &QNetworkAccessManager::finished,
        this, &Authentication::onTokenReceived);
}

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
void Authentication::requestToken(const QString& code) {
  QUrl url {kTokenUrl};
  QNetworkRequest request{url};

  request.setHeader(QNetworkRequest::ContentTypeHeader,
    "application/x-www-form-urlencoded"
    );

  QUrlQuery query;
  query.addQueryItem("client_id", kClientId);
  query.addQueryItem("scope", kScope);
  query.addQueryItem("code", code);
  query.addQueryItem("redirect_uri", kRedirectUri.toString());
  query.addQueryItem("grant_type", "authorization_code");
  query.addQueryItem("code_verifier", pkce_data_.code_verifier);

  QByteArray data {query.toString(QUrl::FullyEncoded).toUtf8()};

  network_manager_.post(request, data);
}

void Authentication::onTokenReceived(QNetworkReply* reply) {
  reply->deleteLater();

  if (reply->error()) {
    throw std::runtime_error(reply->errorString().toStdString());
    return;
  }

  QJsonDocument json_doc {QJsonDocument::fromJson(reply->readAll())};
  QJsonObject json {json_doc.object()};

  VariantMap response = json.toVariantMap();

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

QVariantMap Authentication::parseLocalhost(const QUrl& url) {
  if (!url.hasQuery()) {
    throw std::invalid_argument("Localhost URL has no returned data!");
  }

  QUrlQuery const query {url.query()};

  QVariantMap data;
  for (const auto & [key, value] : query.queryItems()) {
    data[key] = value;
  }

  // Early return to avoid unneccesary checks
  if (data.contains("error")) {
    return data;
  }

  if (!data.contains("code")
    or data.value("code").typeName() != QString("QString")
    or data.value("code").toString().isEmpty()) {
    throw std::invalid_argument("Localhost URL returned invalid code!");
  }

  if (!data.contains("state")
    or data.value("state").typeName() != QString("QString")
    or data.value("state").toString().isEmpty()) {
    throw std::invalid_argument("Localhost URL has no returned state!");
  }
  if (query.queryItemValue("state") != login_data_.state) {
    throw std::invalid_argument("Localhost URL state mismatch!");
  }

  return data;
}

void Authentication::completeAuth(const QString& code) {
  requestToken(code);
}