//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_AUTHENTICATION_H_
#define LJ_LAUNCHER_AUTHENTICATION_H_

#include <qqml.h>
#include <QCryptographicHash>
#include <QObject>
#include <QRandomGenerator>
#include <QtNetwork>
#include <QUrlQuery>

struct PkceData {
  QString code_challenge;
  QString code_challenge_method {"S256"};
  QString code_verifier;
};

struct LoginData {
  QString state;
  QUrl url;
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

  QUrl codeUrl() { return login_data_.url; }

public slots:
  bool hasRefreshToken();
  bool isUrlLocalhost(const QUrl &url);
  QVariantMap parseLocalhost(const QUrl &url);

private:
  PkceData generatePkceData() const;
  QString generateSafeToken(int length) const;

  LoginData getLoginData();
  QUrl getLoginUrl() const;

  void getUserData();
  QString requestRefreshToken(QString old_token);
  QString requestToken();
  QUrl getCodeUrl(const QString &state);

  bool authenticated_ {false};

  PkceData pkce_data_;
  LoginData login_data_;
  UserData user_data_;
  QString auth_code_;

  inline static const QString kClientId{"478514ce-2d7e-4e71-9301-29eb2241e2d6"};
  inline static const QUrl kRedirectUri{"http://localhost"};

  inline static const QUrl kAuthUrl{"https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize"};
  inline static const QUrl kTokenUrl{"https://login.microsoftonline.com/consumers/oauth2/v2.0/token"};
  inline static const QString kScope{"XboxLive.signin"};

};

#endif  // LJ_LAUNCHER_AUTHENTICATION_H_
