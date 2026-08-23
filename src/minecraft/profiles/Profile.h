//
// Created by jay on 23/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILE_H_
#define LJ_LAUNCHER_PROFILE_H_
#include <QVariantMap>

struct Resolution {
  Q_GADGET
  Q_PROPERTY(uint width MEMBER width)
  Q_PROPERTY(uint height MEMBER height)

public:
  uint width;
  uint height;
};

class Profile : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(ProfileType type MEMBER type)
  Q_PROPERTY(QString created MEMBER created)
  Q_PROPERTY(QString lastUsed MEMBER lastUsed)
  Q_PROPERTY(QString icon MEMBER icon)
  Q_PROPERTY(QString lastVersionId MEMBER lastVersionId)
  Q_PROPERTY(QVariant gameDir READ getGameDir)
  Q_PROPERTY(QVariant javaDir READ getJavaDir)
  Q_PROPERTY(QVariant javaArgs READ getJavaArgs)
  Q_PROPERTY(QVariant resolution READ getResolution)

public:
  explicit Profile(QObject *parent = nullptr);

  enum class ProfileType {
    CUSTOM,
    LATEST_SNAPSHOT,
    LATEST_RELEASE
  };
  Q_ENUM(ProfileType);

  Profile(const QJsonObject& data, QObject *parent = nullptr);
  Profile(const QVariantMap& data, QObject *parent = nullptr);

  [[nodiscard]] QJsonObject toJson();
  [[nodiscard]] QVariantMap toMap();

  void copy(const QJsonObject& data);
  void copy(const QVariantMap& data);

  QVariant getGameDir();
  QVariant getJavaDir();
  QVariant getJavaArgs();
  QVariant getResolution();


  QString name {"(Default)"};
  ProfileType type {ProfileType::LATEST_RELEASE};
  QString created {"1970-01-01T00:00:00.000Z"};
  QString lastUsed {"1970-01-01T00:00:00.000Z"};
  QString icon {"grass"};
  QString lastVersionId {"latest-release"};
  std::optional<QString> gameDir {std::nullopt};
  std::optional<QString> javaDir {std::nullopt};
  std::optional<QString> javaArgs {std::nullopt};
  std::optional<Resolution> resolution {std::nullopt};

  inline static const Resolution defaultResolution {
    .width = 854,
    .height = 480,
  };

  inline static const QString defaultJavaArgs {"-Xms2G -Xmx4G -XX:+UseCompactObjectHeaders -XX:+AlwaysPreTouch -XX:+UseStringDeduplication"};
};

#endif  // LJ_LAUNCHER_PROFILE_H_
