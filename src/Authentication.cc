//
// Created by jay on 11/08/2026.
//

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
void Authentication::requestToken(const QString& code) {
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

  m_networkManager.post(request, DATA);
}

void Authentication::onTokenReceived(QNetworkReply* reply) {
  reply->deleteLater();

  if (reply->error()) {
    throw std::runtime_error(reply->errorString().toStdString());
    return;
  }

  QJsonDocument const JSON_DOC {QJsonDocument::fromJson(reply->readAll())};
  QJsonObject const JSON {JSON_DOC.object()};

  QVariantMap const RESPONSE {JSON.toVariantMap()};

  if (!RESPONSE.contains("refresh_token")
    or RESPONSE.value("refresh_token").toString().isEmpty()) {
    throw std::runtime_error("No refresh token returned!");
  }

  Settings::setRefreshToken(RESPONSE.value("refresh_token").toString());

  if (!RESPONSE.contains("access_token")
    or RESPONSE.value("access_token").toString().isEmpty()) {
    throw std::runtime_error("No access token returned!");
    }
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

QVariantMap Authentication::parseLocalhost(const QUrl& url) const {
  if (!url.hasQuery()) {
    throw std::invalid_argument("Localhost URL has no returned data!");
  }

  QUrlQuery const QUERY {url.query()};

  QVariantMap data;
  for (const auto & [KEY, VALUE] : QUERY.queryItems()) {
    data[KEY] = VALUE;
  }

  // Early return to avoid unnecessary checks
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
  if (QUERY.queryItemValue("state") != m_loginData.state) {
    throw std::invalid_argument("Localhost URL state mismatch!");
  }

  return data;
}

void Authentication::completeAuth(const QString& accessToken) {
  
}