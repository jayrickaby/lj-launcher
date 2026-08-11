//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_AUTHENTICATION_H_
#define LJ_LAUNCHER_AUTHENTICATION_H_

#include <qqml.h>
#include <QCryptographicHash>
#include <QObject>
#include <QRandomGenerator>
#include <QUrlQuery>

struct PkceData {
  QString code_challenge;
  QString code_challenge_method {"S256"};
  QString code_verifier;
};

struct LoginData {
  QString code_verifier;
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
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Authentication(QObject *parent = nullptr);

public slots:
  bool hasRefreshToken();

private:
  LoginData getLoginData() const;
  QUrl getLoginUrl() const;
  PkceData generatePkceData() const;
  QString generateToken(int length) const;
  QString generateSafeToken(int length) const;


  inline static const QString kClientId{"478514ce-2d7e-4e71-9301-29eb2241e2d6"};
  inline static const QUrl kRedirectUri{"http://localhost"};

  bool authenticated_ {false};

  LoginData login_data_;
  UserData user_data_;

  inline static const QUrl kAuthUrl{"https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize"};
  inline static const QUrl kTokenUrl{"https://login.microsoftonline.com/consumers/oauth2/v2.0/token"};
  inline static const QString kScope{"XboxLive.signin offline_access"};

};

#endif  // LJ_LAUNCHER_AUTHENTICATION_H_
