//
// Created by jay on 14/08/2026.
//

#include "Network.h"

#include "NetworkRequester.h"

Network* Network::s_instance {nullptr};

Network::Network(QObject* parent)
: QNetworkAccessManager(parent) {

  connect(this, &QNetworkAccessManager::finished,&onNetworkReply);
}

QNetworkReply* Network::get(const NetworkRequester* requester, const QNetworkRequest& request) {
  auto* reply {getInstance()->QNetworkAccessManager::get(request)};

  reply->setProperty("requester", QVariant::fromValue(requester));

  return reply;
}

QNetworkReply* Network::post(const NetworkRequester* requester, const QNetworkRequest& request, const QByteArray& data) {
  auto* reply {getInstance()->QNetworkAccessManager::post(request,data)};

  reply->setProperty("requester", QVariant::fromValue(requester));

  return reply;
}

void Network::onNetworkReply(QNetworkReply* reply) {
  QVariant const REQUESTER_PROPERTY = reply->property("requester");

  if (REQUESTER_PROPERTY.isNull() or !REQUESTER_PROPERTY.isValid()) {
    qDebug() << "Unknown network requester!";
    return;
  }

  auto* requester {REQUESTER_PROPERTY.value<NetworkRequester*>()};
  if (!requester) {
    qDebug() << "Requester is null!";
    return;
  }

  requester->onNetworkReply(reply);
}

Network* Network::getInstance() {
  if (!s_instance) {
    s_instance = new Network();
  }
  return s_instance;
}