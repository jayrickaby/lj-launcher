//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_AUTHENTICATION_H_
#define LJ_LAUNCHER_AUTHENTICATION_H_

#include <qqml.h>
#include <QCryptographicHash>
#include <QDebug>
#include <QObject>
#include <QRandomGenerator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

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
  QString username;
  QString uuid;
  QString token;
};

class Authentication : public QObject {
  Q_OBJECT
  Q_PROPERTY(QUrl codeUrl READ codeUrl)
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Authentication(QObject *parent = nullptr);

  [[nodiscard]] QUrl codeUrl() const { return m_loginData.url; }

  enum class AuthType {
    MICROSOFT_TOKEN,
    XBOX_LIVE_TOKEN,
    XBOX_SERVICES_TOKEN,
    MINECRAFT_TOKEN
  };

  Q_ENUM(AuthType)

public slots:
  [[nodiscard]] bool hasRefreshToken() const;
  [[nodiscard]] bool isUrlLocalhost(const QUrl &url) const;
  void parseLocalhost(const QUrl &url);

private:
  [[nodiscard]] PkceData generatePkceData() const;
  [[nodiscard]] QString generateSafeToken(int length) const;

  [[nodiscard]] QUrl getCodeUrl(const QString &state) const;
  [[nodiscard]] LoginData getLoginData() const;

  QString requestRefreshToken(const QString& oldToken);
  void completeAuth(const QString &accessToken);

  void requestMicrosoftAuth(const QString& code);
  QString parseMicrosoftTokens(const QJsonObject& json);

  void requestXboxLiveAuth(const QString& accessToken);
  QString parseXboxLiveAuth(const QJsonObject& json);

  void requestXboxServicesAuth(const QString& xblToken);
  XboxServicesData parseXboxServicesAuthData(const QJsonObject& json);

  void requestMinecraftAuth(const QString& token);
  QString parseMinecraftToken(const QJsonObject& json);


  bool m_authenticated {false};
  QNetworkAccessManager m_networkManager;
  PkceData m_pkceData;
  LoginData m_loginData;
  UserData m_userData;

  inline static const QString CLIENT_ID{"478514ce-2d7e-4e71-9301-29eb2241e2d6"};
  inline static const QUrl REDIRECT_URI{"http://localhost"};

  inline static const QUrl AUTH_URL{"https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize"};
  inline static const QUrl TOKEN_URL{"https://login.microsoftonline.com/consumers/oauth2/v2.0/token"};
  inline static const QUrl XBOX_LIVE_URL{"https://user.auth.xboxlive.com/user/authenticate"};
  inline static const QUrl XBOX_SERVICES_URL{" https://xsts.auth.xboxlive.com/xsts/authorize"};
  inline static const QString SCOPE{"XboxLive.signin offline_access"};

private slots:
  void onTokenReceived(QNetworkReply *reply);
};

#endif  // LJ_LAUNCHER_AUTHENTICATION_H_
