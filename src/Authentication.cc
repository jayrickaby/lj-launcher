//
// Created by jay on 11/08/2026.
//

#include <iostream>
#include "Authentication.h"
#include "Settings.h"

Authentication::Authentication(QObject* parent) :
QObject(parent),
m_pkceData(generatePkceData()),
m_loginData(getLoginData()) {
  connect(&m_networkManager, &QNetworkAccessManager::finished,
        this, &Authentication::onTokenReceived);
}

LoginData Authentication::getLoginData() const{
  LoginData data;

  // There isn't a standard, but 64 is good enough.
  QString const STATE = generateSafeToken(64);

  data.url = getCodeUrl(STATE);
  data.state = STATE;

  return data;
}

bool Authentication::hasRefreshToken() const {
  if (Settings::getRefreshToken().canConvert<QString>()) {
    return true;
  }
  if (!Settings::getRefreshToken().toString().isEmpty()) {
    return true;
  }

  return false;
}

QString Authentication::generateSafeToken(int length) const {
  const QString ALLOWED_CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
  QString token;
  token.reserve(length);

  int const ALLOWED_LENGTH { static_cast<int>(ALLOWED_CHARACTERS.length()) };

  for (int i = 0; i < length; i++) {
    const int INDEX {QRandomGenerator::global()->bounded(ALLOWED_LENGTH)};
    token.append(ALLOWED_CHARACTERS[INDEX]);
  }

  return token;
}

PkceData Authentication::generatePkceData() const {
  PkceData pkceData;

  QString const TOKEN {generateSafeToken(128)};
  pkceData.codeVerifier = TOKEN;

  QByteArray const HASH = QCryptographicHash::hash(
    TOKEN.toLatin1(),
    QCryptographicHash::Sha256
    );

  pkceData.codeChallenge = HASH.toBase64(
    QByteArray::Base64UrlEncoding
    | QByteArray::OmitTrailingEquals    // RFC7636 states to omit trailing '='
  );

  return pkceData;
}

bool Authentication::isUrlLocalhost(const QUrl& url) const {
  return url.host() == "localhost";
}

QString Authentication::requestRefreshToken(const QString& oldToken) {
  QUrl url {TOKEN_URL};
  QUrlQuery query;
  query.addQueryItem("client_id", CLIENT_ID);
  query.addQueryItem("grant_type", "refresh_token");
  query.addQueryItem("scope", SCOPE);
  query.addQueryItem("refresh_token", oldToken);
  return "balls";
}
void Authentication::requestMicrosoftTokens(const QString& code) {
  const QUrl URL {TOKEN_URL};
  QNetworkRequest request{URL};

  request.setHeader(QNetworkRequest::ContentTypeHeader,
    "application/x-www-form-urlencoded"
    );

  QUrlQuery query;
  query.addQueryItem("client_id", CLIENT_ID);
  query.addQueryItem("scope", SCOPE);
  query.addQueryItem("code", code);
  query.addQueryItem("redirect_uri", REDIRECT_URI.toString());
  query.addQueryItem("grant_type", "authorization_code");
  query.addQueryItem("code_verifier", m_pkceData.codeVerifier);

  QByteArray const DATA {query.toString(QUrl::FullyEncoded).toUtf8()};

  QNetworkReply* reply = m_networkManager.post(request, DATA);
  reply->setProperty("type", QVariant::fromValue(AuthType::MICROSOFT_TOKEN));
}

void Authentication::onTokenReceived(QNetworkReply* reply) {
  reply->deleteLater();

  if (reply->error()) {
    throw std::runtime_error(reply->errorString().toStdString());
    return;
  }

  if (!reply->property("type").isValid()) {
    throw std::runtime_error("Received token data of unknown type!");
  }

  switch (auto authType = reply->property("type").value<AuthType>()) {
    case AuthType::MICROSOFT_TOKEN:
      parseMicrosoftTokens(reply);
      break;
    default:
      throw std::runtime_error("Received token data of unknown type!");
      break;
  }
}

void Authentication::parseMicrosoftTokens(QNetworkReply* reply) {
  QJsonDocument const JSON_DOC {QJsonDocument::fromJson(reply->readAll())};
  QJsonObject const JSON {JSON_DOC.object()};

  QVariantMap const RESPONSE {JSON.toVariantMap()};

  // Check before saving refresh incase its invalid
  if (!RESPONSE.contains("access_token")
    or RESPONSE.value("access_token").toString().isEmpty()) {
    throw std::runtime_error("No access token returned!");
    }

  if (!RESPONSE.contains("refresh_token")
    or RESPONSE.value("refresh_token").toString().isEmpty()) {
    throw std::runtime_error("No refresh token returned!");
    }

  // Save to avoid manual in next time
  Settings::setRefreshToken(RESPONSE.value("refresh_token").toString());
}

QUrl Authentication::getCodeUrl(const QString& state) const {
  QUrl url {AUTH_URL};
  QUrlQuery query;
  query.addQueryItem("client_id", CLIENT_ID);
  query.addQueryItem("response_type", "code");
  query.addQueryItem("redirect_uri", REDIRECT_URI.toString());
  query.addQueryItem("scope", SCOPE);
  query.addQueryItem("response_mode", "query");
  query.addQueryItem("code_challenge", m_pkceData.codeChallenge);
  query.addQueryItem("code_challenge_method", m_pkceData.codeChallengeMethod);
  query.addQueryItem("state", state);
  url.setQuery(query);

  return url;
}

void Authentication::parseLocalhost(const QUrl& url) {
  if (!url.hasQuery()) {
    throw std::invalid_argument("Localhost URL has no returned data!");
  }

  QUrlQuery const QUERY {url.query()};

  // Early return to avoid unnecessary checks
  if (QUERY.hasQueryItem("error")) {
    throw std::runtime_error(
      QUERY.queryItemValue("error").toStdString()
      + '\n'
      +  QUERY.queryItemValue("error_description").toStdString()
      );
  }

  // Check state before code incase of mismatch
  if (!QUERY.hasQueryItem("state")
    or QUERY.queryItemValue("state").isEmpty()) {
    throw std::invalid_argument("Localhost URL has no returned state!");
  }
  // Mismatch means possible interception
  if (QUERY.queryItemValue("state") != m_loginData.state) {
    throw std::invalid_argument("Localhost URL state mismatch!");
  }

  if (!QUERY.hasQueryItem("code")
    or QUERY.queryItemValue("code").isEmpty()) {
    throw std::invalid_argument("Localhost URL returned invalid code!");
  }

  requestMicrosoftTokens(QUERY.queryItemValue("code"));
}

void Authentication::completeAuth(const QString& accessToken) {
  
}