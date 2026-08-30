//
// Created by jay on 30/08/2026.
//

#ifndef LJ_LAUNCHER_RESOLUTION_H_
#define LJ_LAUNCHER_RESOLUTION_H_
#include <qtypes.h>

#include <QJsonObject>
#include <QObject>
#include <QVariant>
#include <QVariantMap>

class Resolution {
  Q_GADGET
  Q_PROPERTY(uint width MEMBER m_width)
  Q_PROPERTY(uint height MEMBER m_height)

public:
  Resolution() = default;

  Resolution(uint width, uint height);
  Resolution(const QVariantMap& data);
  Resolution(const QJsonObject& data);

  QVariantMap toMap();
  QJsonObject toJson();

  uint getWidth() { return m_width; };
  uint getHeight() { return m_height; };

  void setWidth(uint width) { m_width = width; };
  void setHeight(uint height) { m_height = height; };

private:
  uint m_width;
  uint m_height;
};

#endif  // LJ_LAUNCHER_RESOLUTION_H_
