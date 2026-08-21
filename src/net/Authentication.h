//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_AUTHENTICATION_H_
#define LJ_LAUNCHER_AUTHENTICATION_H_

#include <qqml.h>

#include <QCryptographicHash>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QRandomGenerator>
#include <QUrlQuery>
#include <QVariantList>

#include "../Launcher.h"
#include "../Settings.h"
#include "Network.h"
#include "NetworkRequester.h"

struct PkceData {
  QString codeChallenge;
  QString codeChallengeMethod {"S256"};
  QString codeVerifier;
};

struct LoginData {
  QString state;
  QUrl url;
};

struct XboxServicesData {
  QString token;
  QString userHash;
};

struct UserData {
  QString id;
  QString name;
  QString accessToken;
};

class Authentication : public NetworkRequester {
  Q_OBJECT
  Q_PROPERTY(QUrl codeUrl READ codeUrl)
  Q_PROPERTY(AuthState authState READ authState NOTIFY authStateChanged)
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Authentication(QObject *parent = nullptr);

  enum class AuthState {
    UNAUTHENTICATED,
    AUTHENTICATING,
    AUTHENTICATING_REFRESH,
    AUTHENTICATED
  };
  Q_ENUM(AuthState)
  enum class AuthType {
    MICROSOFT_TOKEN,
    XBOX_LIVE_TOKEN,
    XBOX_SERVICES_TOKEN,
    MINECRAFT_TOKEN,
    GAME_OWNERSHIP,
    PROFILE
  };

  [[nodiscard]] QUrl codeUrl() const { return m_loginData.url; }
  [[nodiscard]] AuthState authState() const { return s_authState; }
  [[nodiscard]] static AuthState getAuthState() { return s_authState; }

  static Authentication* getInstance();

  void onNetworkReply(QNetworkReply* reply) override;

signals:
  void authStateChanged();

public slots:
  [[nodiscard]] bool hasRefreshToken() const;
  [[nodiscard]] bool isUrlLocalhost(const QUrl &url) const;
  void parseLocalhost(const QUrl &url);
  void tryStoredRefreshToken();


private:
  [[nodiscard]] PkceData generatePkceData() const;
  [[nodiscard]] QString generateSafeToken(int length) const;

  [[nodiscard]] QUrl getCodeUrl(const QString &state) const;
  [[nodiscard]] LoginData getLoginData() const;

  void requestMicrosoftAuthViaRefresh(const QString& oldToken);

  void requestMicrosoftAuth(const QString& code);
  QString parseMicrosoftTokens(const QJsonObject& json);

  void requestXboxLiveAuth(const QString& accessToken);
  QString parseXboxLiveAuth(const QJsonObject& json);

  void requestXboxServicesAuth(const QString& xblToken);
  XboxServicesData parseXboxServicesAuthData(const QJsonObject& json);

  void requestMinecraftAuth(const XboxServicesData& data);
  QString parseMinecraftToken(const QJsonObject& json);

  void requestGameOwnership(const QString& accessToken);
  bool parseGameOwnership(const QJsonObject& json);

  void requestMinecraftProfile(const QString& accessToken);
  UserData parseMinecraftProfile(const QJsonObject& json);

  void setState(const AuthState& state);

  PkceData m_pkceData;
  LoginData m_loginData;
  UserData m_userData;

  static AuthState s_authState;

  inline static const QString CLIENT_ID{"478514ce-2d7e-4e71-9301-29eb2241e2d6"};
  inline static const QUrl REDIRECT_URI{"http://localhost"};

  inline static const QUrl AUTH_URL{"https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize"};
  inline static const QUrl TOKEN_URL{"https://login.microsoftonline.com/consumers/oauth2/v2.0/token"};
  inline static const QUrl XBOX_LIVE_URL{"https://user.auth.xboxlive.com/user/authenticate"};
  inline static const QUrl XBOX_SERVICES_URL{"https://xsts.auth.xboxlive.com/xsts/authorize"};
  inline static const QUrl MINECRAFT_URL{"https://api.minecraftservices.com"};
  inline static const QString SCOPE{"XboxLive.signin offline_access"};

  static Authentication* s_instance;
};

#endif  // LJ_LAUNCHER_AUTHENTICATION_H_
