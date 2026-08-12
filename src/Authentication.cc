//
// Created by jay on 11/08/2026.
//

#include "Authentication.h"

#include "Launcher.h"
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

  qInfo("Generated login data.");
  return data;
}

bool Authentication::hasRefreshToken() const {
  if (Settings::getRefreshToken().canConvert<QString>()
    and !Settings::getRefreshToken().toString().isEmpty()) {
    qDebug("Stored refresh token found!");
    return true;
  }

  qDebug("No stored refresh token found!");
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

  qDebug("Generated safe token of %i characters.", length);
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

  qInfo("Generated PKCE data");
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
void Authentication::requestMicrosoftAuth(const QString& code) {
  const QUrl URL {TOKEN_URL};
  QNetworkRequest request{URL};

  request.setHeader(
    QNetworkRequest::ContentTypeHeader,
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

  qDebug("Requesting Microsoft Auth Tokens");
  QNetworkReply* reply = m_networkManager.post(request, DATA);
  reply->setProperty("type", QVariant::fromValue(AuthType::MICROSOFT_TOKEN));
}

void Authentication::requestXboxLiveAuth(const QString& accessToken) {
  const QUrl URL {XBOX_LIVE_URL};
  QNetworkRequest request{URL};

  request.setHeader(
    QNetworkRequest::ContentTypeHeader,
    "application/json"
  );

  request.setRawHeader(
    "Accept",
    "application/json"
  );

  QJsonObject properties {};
  properties.insert("AuthMethod", "RPS");
  properties.insert("SiteName", "user.auth.xboxlive.com");
  properties.insert("RpsTicket", "d=" + accessToken);

  QJsonObject json {};
  json.insert("Properties", properties);
  json.insert("RelyingParty", "http://auth.xboxlive.com");
  json.insert("TokenType", "JWT");

  QJsonDocument const JSON_DOC {json};

  QNetworkReply* reply = m_networkManager.post(request, JSON_DOC.toJson());
  reply->setProperty("type", QVariant::fromValue(AuthType::XBOX_LIVE_TOKEN));

  qDebug("Requesting Xbox Live Auth Token");
}

void Authentication::requestXboxServicesAuth(const QString& token) {
  const QUrl URL {XBOX_SERVICES_URL};
  QNetworkRequest request{URL};

  request.setHeader(
    QNetworkRequest::ContentTypeHeader,
    "application/json"
  );

  request.setRawHeader(
    "Accept",
    "application/json"
  );

  QJsonObject properties {};
  properties.insert("SandboxId", "RETAIL");
  properties.insert("UserTokens", QJsonArray{token});

  QJsonObject json {};
  json.insert("Properties", properties);
  json.insert("RelyingParty", "rp://api.minecraftservices.com/");
  json.insert("TokenType", "JWT");

  QJsonDocument const JSON_DOC {json};

  QNetworkReply* reply = m_networkManager.post(request, JSON_DOC.toJson());
  reply->setProperty("type", QVariant::fromValue(AuthType::XBOX_SERVICES_TOKEN));

  qDebug("Requesting Xbox Services Auth Token");
}

void Authentication::onTokenReceived(QNetworkReply* reply) {
  reply->deleteLater();

  try {
    if (reply->error()) {
      throw std::runtime_error(reply->errorString().toStdString());
    }

    if (!reply->property("type").isValid()) {
      qDebug() << reply->readAll();
      throw std::runtime_error("Received reply of unknown type!");
    }

    QByteArray const JSON_RAW {reply->readAll()};
    QJsonDocument const JSON_DOC {QJsonDocument::fromJson(JSON_RAW)};
    QJsonObject const JSON {JSON_DOC.object()};

    switch (auto authType = reply->property("type").value<AuthType>()) {
      case AuthType::MICROSOFT_TOKEN: {
        qDebug("Received Microsoft Auth Tokens");
        QString const ACCESS_TOKEN {parseMicrosoftTokens(JSON)};
        requestXboxLiveAuth(ACCESS_TOKEN);
        break;
      }
      case AuthType::XBOX_LIVE_TOKEN: {
        qDebug("Received Xbox Auth Token");
        QString const TOKEN {parseXboxLiveAuth(JSON)};
        requestXboxServicesAuth(TOKEN);
        break;
      }
      case AuthType::XBOX_SERVICES_TOKEN: {
        qDebug("Received Xbox Service Token");
        XboxServicesData const DATA {parseXboxServicesAuthData(JSON)};
        break;
      }
      default: {
        throw std::runtime_error("Received token data of unknown type!");
        break;
      }
    }
  }
  catch (std::exception const& e) {
    setState(AuthState::UNAUTHENTICATED);
    ErrorMessage message;
    message.errorTechnical = e.what();
    Launcher::sendError(message);

    // Redundant to keep if potentially invalid
    Settings::clearRefreshToken();
  }
}



QString Authentication::parseMicrosoftTokens(const QJsonObject& json) {
  // Check before saving refresh incase its invalid
  if (!json.contains("access_token")
    or !json["access_token"].isString()
    or json["access_token"].toString().isEmpty()) {
    throw std::runtime_error("No access token returned!");
  }

  qDebug("Found requested Access Token");

  if (!json.contains("refresh_token")
    or !json["access_token"].isString()
    or json["refresh_token"].toString().isEmpty()) {
    throw std::runtime_error("No refresh token returned!");
  }

  qDebug("Found requested Refresh Token");

  // Save to avoid manual login next time
  Settings::setRefreshToken(json["refresh_token"].toString());
  return json["access_token"].toString();
}

QString Authentication::parseXboxLiveAuth(const QJsonObject& json) {
  if (!json.contains("Token")
    or !json["Token"].isString()
    or json["Token"].toString().isEmpty()) {
    throw std::runtime_error("No Xbox Live token returned!");
    }

  qDebug("Found requested Xbox Live token");
  return json.value("Token").toString();
}

XboxServicesData Authentication::parseXboxServicesAuthData(const QJsonObject& json) {
  XboxServicesData data;

  if (!json.contains("Token")
    or !json["Token"].isString()
    or json["Token"].toString().isEmpty()) {
    throw std::runtime_error("No Xbox services token returned!");
    }

  qDebug("Found requested Xbox Services token");
  data.token = json.value("Token").toString();

  if (!json.contains("DisplayClaims")
    or !json["DisplayClaims"].isObject()
    or json["DisplayClaims"].toObject().isEmpty()) {
    throw std::runtime_error("No display claims returned!");
  }

  QJsonObject const DISPLAY_CLAIMS {
    json.value("DisplayClaims").toObject()
  };

  if (!DISPLAY_CLAIMS.contains("xui")
    or !DISPLAY_CLAIMS["xui"].isArray()
    or DISPLAY_CLAIMS["xui"].toArray().isEmpty()
    or !DISPLAY_CLAIMS["xui"][0].isObject()
    or DISPLAY_CLAIMS["xui"][0].toObject().isEmpty()) {
    throw std::runtime_error("No xui returned!");
  }

  QJsonObject const XUI {
    DISPLAY_CLAIMS.value("xui").toArray()[0].toObject()
  };

  if (!XUI.contains("uhs")
    or !XUI["uhs"].isString()
    or XUI["uhs"].toString().isEmpty()) {
    throw std::runtime_error("No user hash returned!");
  }

  qDebug("Found requested Xbox user hash");
  data.userHash = XUI["uhs"].toString();

  return data;
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
  try {
    if (!url.hasQuery()) {
      throw std::invalid_argument("Localhost URL has no returned data!");
    }

    QUrlQuery const QUERY {url.query()};

    // Early return to avoid unnecessary checks
    if (QUERY.hasQueryItem("error")) {
      QString const ERROR = QUERY.queryItemValue("error");
      QString const DESC = QUERY.queryItemValue("error_description");

      throw std::runtime_error(QString(ERROR + ": " + DESC).toStdString());
    }

    // Check state before code incase of mismatch
    if (!QUERY.hasQueryItem("state")
      or QUERY.queryItemValue("state").isEmpty()) {
      throw std::invalid_argument("Localhost URL has no returned state!");
    }
    qDebug("Found State in URL");
    // Mismatch means possible interception
    if (QUERY.queryItemValue("state") != m_loginData.state) {
      throw std::invalid_argument("Localhost URL state mismatch!");
    }

    if (!QUERY.hasQueryItem("code")
      or QUERY.queryItemValue("code").isEmpty()) {
      throw std::invalid_argument("Localhost URL returned invalid code!");
      }
    qDebug("Found Code in URL. Beginning authentication");

    setState(AuthState::AUTHENTICATING);
    requestMicrosoftAuth(QUERY.queryItemValue("code"));
  }
  catch (std::exception const& e) {
    setState(AuthState::UNAUTHENTICATED);
    ErrorMessage message;
    message.errorTechnical = e.what();
    Launcher::sendError(message);
  }
}

void Authentication::setState(const AuthState& state) {
  m_authState = state;
  emit authStateChanged();
}