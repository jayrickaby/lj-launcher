//
// Created by jay on 11/08/2026.
//

#include "Authentication.h"

#include "Versions.h"
#include "minecraft/exec/JavaVirtualMachine.h"

Authentication* Authentication::s_instance {nullptr};
Authentication::AuthState Authentication::s_authState {AuthState::UNAUTHENTICATED};

Authentication::Authentication(QObject* parent) :
NetworkRequester(parent),
m_pkceData(generatePkceData()),
m_loginData(getLoginData()) {
  if (!s_instance) {
    s_instance = this;
  }
}

void Authentication::tryStoredRefreshToken() {
  Launcher::addLog("Refreshing auth...");
  if (!hasRefreshToken()) {
    return;
  }

  QString const TOKEN {getRefreshToken().toString()};
  requestMicrosoftAuthViaRefresh(TOKEN);
}

QVariant Authentication::getRefreshToken() const {
  QSettings settings;
  settings.beginGroup("Account");
  return settings.value("refreshToken");
}

void Authentication::setRefreshToken(const QString& refreshToken) {
  QSettings settings;
  settings.beginGroup("Account");
  settings.setValue("refreshToken", refreshToken);
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

void Authentication::clearRefreshToken() {
  QSettings settings;
  settings.beginGroup("Account");
  settings.remove("refreshToken");
}

bool Authentication::hasRefreshToken() const {
  QSettings settings;
  settings.beginGroup("Account");
  return settings.contains("refreshToken");
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

void Authentication::requestMicrosoftAuthViaRefresh(const QString& oldToken) {
  QNetworkRequest request{TOKEN_URL};
  request.setHeader(
    QNetworkRequest::ContentTypeHeader,
    "application/x-www-form-urlencoded"
    );

  QUrlQuery query;
  query.addQueryItem("client_id", CLIENT_ID);
  query.addQueryItem("scope", SCOPE);
  query.addQueryItem("refresh_token", oldToken);
  query.addQueryItem("grant_type", "refresh_token");

  QByteArray const DATA {query.toString(QUrl::FullyEncoded).toUtf8()};

  qDebug("Requesting Microsoft Auth Tokens via Stored Refresh");
  QNetworkReply* reply = Downloader::post(s_instance, request, DATA);
  reply->setProperty("type", QVariant::fromValue(AuthType::MICROSOFT_TOKEN));
  setState(AuthState::AUTHENTICATING_REFRESH);
}
void Authentication::requestMicrosoftAuth(const QString& code) {
  QNetworkRequest request{TOKEN_URL};

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
  QNetworkReply* reply = Downloader::post(s_instance, request, DATA);
  reply->setProperty("type", QVariant::fromValue(AuthType::MICROSOFT_TOKEN));
}

void Authentication::requestXboxLiveAuth(const QString& accessToken) {
  QNetworkRequest request{XBOX_LIVE_URL};

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

  QNetworkReply* reply = Downloader::post(s_instance, request, JSON_DOC.toJson());
  reply->setProperty("type", QVariant::fromValue(AuthType::XBOX_LIVE_TOKEN));

  qDebug("Requesting Xbox Live Auth Token");
}

void Authentication::requestXboxServicesAuth(const QString& token) {
  QNetworkRequest request{XBOX_SERVICES_URL};

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

  QNetworkReply* reply = Downloader::post(s_instance, request, JSON_DOC.toJson());
  reply->setProperty("type", QVariant::fromValue(AuthType::XBOX_SERVICES_TOKEN));

  qDebug("Requesting Xbox Services Auth Token");
}

void Authentication::requestMinecraftAuth(const XboxServicesData& data) {
  const QUrl URL {MINECRAFT_URL.toString() + "/authentication/login_with_xbox"};
  QNetworkRequest request{URL};

  request.setHeader(
    QNetworkRequest::ContentTypeHeader,
    "application/json"
  );

  request.setRawHeader(
    "Accept",
    "application/json"
  );

  QString const IDENTITY_TOKEN {
    QString("XBL3.0 x=%1;%2")
    .arg(data.userHash, data.token)
  };

  QJsonObject json {};
  json.insert("identityToken", IDENTITY_TOKEN);

  QJsonDocument const JSON_DOC {json};

  QNetworkReply* reply = Downloader::post(s_instance, request, JSON_DOC.toJson());
  reply->setProperty("type", QVariant::fromValue(AuthType::MINECRAFT_TOKEN));

  qDebug("Requesting Minecraft Services Access Token");
}

void Authentication::requestGameOwnership(const QString& accessToken) {
  const QUrl URL {MINECRAFT_URL.toString() + "/entitlements/mcstore"};
  QNetworkRequest request{URL};

  request.setRawHeader(
    "Authorization",
    QString("Bearer %1").arg(accessToken).toUtf8()
  );

  QNetworkReply* reply = Downloader::get(this, request);
  reply->setProperty("type", QVariant::fromValue(AuthType::GAME_OWNERSHIP));

  qDebug("Requesting Minecraft Game Ownership");
}

void Authentication::requestMinecraftProfile(const QString& accessToken) {
  const QUrl URL {MINECRAFT_URL.toString() + "/minecraft/profile"};
  QNetworkRequest request{URL};

  request.setRawHeader(
    "Authorization",
    QString("Bearer %1").arg(accessToken).toUtf8()
  );

  QNetworkReply* reply = Downloader::get(s_instance, request);
  reply->setProperty("type", QVariant::fromValue(AuthType::PROFILE));

  qDebug("Requesting Minecraft Profile");
}


void Authentication::onNetworkReply(QNetworkReply* reply) {
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

    switch (reply->property("type").value<AuthType>()) {
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
        requestMinecraftAuth(DATA);
        break;
      }
      case AuthType::MINECRAFT_TOKEN: {
        qDebug("Received Minecraft Token");
        m_userData.accessToken = parseMinecraftToken(JSON);
        requestGameOwnership(m_userData.accessToken);
        break;
      }
      case AuthType::GAME_OWNERSHIP: {
        qDebug("Received Game Ownership");
        if (parseGameOwnership(JSON)) {
          requestMinecraftProfile(m_userData.accessToken);
        }
        break;
      }
      case AuthType::PROFILE: {
        qDebug("Received Profile");
        m_userData = parseMinecraftProfile(JSON);
        Launcher::setUsername(m_userData.name);
        setState(AuthState::AUTHENTICATED);
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
    clearRefreshToken();
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
  setRefreshToken(json["refresh_token"].toString());
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

  JavaVirtualMachine::setVariable("auth_xuid", data.userHash);

  return data;
}

QString Authentication::parseMinecraftToken(const QJsonObject& json) {
  if (!json.contains("access_token")
    or !json.value("access_token").isString()
    or json.value("access_token").toString().isEmpty()) {
    throw std::runtime_error("No Minecraft access token returned!");
  }

  auto accessToken = json.value("access_token").toString();

  JavaVirtualMachine::setVariable("auth_access_token", accessToken);

  Launcher::addLog("Logging in with access token");

  return accessToken;
}

UserData Authentication::parseMinecraftProfile(const QJsonObject& json) {
  // FIXME: This is bad. Find some way to pass on MC Access Token elsewhere
  UserData data {m_userData};

  if (!json.contains("id")
    or !json["id"].isString()
    or json["id"].toString().isEmpty()) {
    throw std::runtime_error("No Minecraft Profile UUID returned!");
  }

  data.id = json["id"].toString();
  JavaVirtualMachine::setVariable("auth_uuid", data.id);

  if (!json.contains("name")
    or !json["name"].isString()
    or json["name"].toString().isEmpty()) {
    throw std::runtime_error("No Minecraft Profile Username returned!");
  }

  data.name = json["name"].toString();
  JavaVirtualMachine::setVariable("auth_player_name", data.name);

  qDebug("Found Minecraft Profile");
  qInfo() << "Hello," << data.name;
  qDebug() << "*He knows my name, my occupation. He can find out about my family. My family...*";
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

bool Authentication::parseGameOwnership(const QJsonObject& json) {
  if (!json.contains("items")
    or !json["items"].isArray()
    or json["items"].toArray().isEmpty()) {
    throw std::runtime_error("Cannot verify game ownership!");
  }

  QJsonArray const ITEMS (json["items"].toArray());

  for (auto const ITEM : ITEMS) {
    QJsonObject const OBJ {ITEM.toObject()};

    if (!OBJ.contains("name")
      or !OBJ["name"].isString()
      or OBJ["name"].toString().isEmpty()) continue;

    QString const NAME {OBJ["name"].toString()};

    if (NAME == "product_minecraft"
      or NAME == "game_minecraft"
      or NAME == "product_game_pass_pc"
      or NAME == "product_game_pass_ultimate") {

      qDebug("Verified game ownership!");
      return true;
    }
  throw std::runtime_error("Game is not registered under account!");
  }

  throw std::runtime_error("Cannot verify game ownership!");
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
  if (s_authState == state) { return; }

  s_authState = state;
  emit authStateChanged();
}

Authentication* Authentication::getInstance() {
  if (!s_instance) {
    s_instance = new Authentication();
  }
  return s_instance;
}

void Authentication::logOut() {
  QSettings settings;
  settings.beginGroup("Account");
  settings.remove("refreshToken");
  setState(AuthState::UNAUTHENTICATED);
}