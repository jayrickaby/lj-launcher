//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_NETWORK_H_
#define LJ_LAUNCHER_NETWORK_H_
#include <qqml.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "NetworkRequester.h"

class Network : public QNetworkAccessManager {
  Q_OBJECT

public:
  explicit Network(QObject *parent = nullptr);

  static Network* getInstance();

  static QNetworkReply* get(const NetworkRequester* requester, const QNetworkRequest& request);
  static QNetworkReply* post(const NetworkRequester* requester, const QNetworkRequest& request, const QByteArray& data);

private:
  static Network* s_instance;
private slots:
  static void onNetworkReply(QNetworkReply* reply);
};

#endif  // LJ_LAUNCHER_NETWORK_H_
