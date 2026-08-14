//
// Created by jay on 14/08/2026.
//

#ifndef LJ_LAUNCHER_NETWORKREQUESTER_H_
#define LJ_LAUNCHER_NETWORKREQUESTER_H_

#include <qqml.h>

#include <QNetworkReply>

class NetworkRequester : public QObject {
  Q_OBJECT

public:
  explicit NetworkRequester(QObject *parent = nullptr);

  virtual void onNetworkReply(QNetworkReply* reply);
};

#endif  // LJ_LAUNCHER_NETWORKREQUESTER_H_
