//
// Created by jay on 30/08/2026.
//

#include "Resolution.h"

Resolution::Resolution(uint width, uint height)
  : m_width(width),
    m_height(height) {}

Resolution::Resolution(const QJsonObject& data)
  : Resolution(data.toVariantMap()) {}

Resolution::Resolution(const QVariantMap& data) {
  if (data.contains("width")
    and data.value("width").isValid()
    and !data.value("width").isNull()) {
    m_width = data.value("width").toUInt();
  }

  if (data.contains("height")
    and data.value("height").isValid()
      and !data.value("height").isNull()) {
    m_height = data.value("height").toUInt();
  }
}

QJsonObject Resolution::toJson() {
  return QJsonObject::fromVariantMap(toMap());
}

QVariantMap Resolution::toMap() {
  return {
    {"width", m_width},
    {"height", m_height},
  };
}