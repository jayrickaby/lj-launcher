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
  Q_PROPERTY(QString name READ getName NOTIFY nameChanged)
  Q_PROPERTY(ProfileType type READ getType NOTIFY typeChanged)
  Q_PROPERTY(QString created READ getCreated NOTIFY createdChanged)
  Q_PROPERTY(QString lastUsed READ getLastUsed NOTIFY lastUsedChanged)
  Q_PROPERTY(QString icon READ getIcon NOTIFY iconChanged)
  Q_PROPERTY(QString lastVersionId READ getLastVersionId NOTIFY lastVersionIdChanged)
  Q_PROPERTY(QVariant gameDir READ getGameDir NOTIFY gameDirChanged)
  Q_PROPERTY(QVariant javaDir READ getJavaDir NOTIFY javaDirChanged)
  Q_PROPERTY(QVariant javaArgs READ getJavaArgs NOTIFY javaArgsChanged)
  Q_PROPERTY(QVariant resolution READ getResolution NOTIFY resolutionChanged)

signals:
  void nameChanged();
  void typeChanged();
  void createdChanged();
  void lastUsedChanged();
  void iconChanged();
  void lastVersionIdChanged();
  void gameDirChanged();
  void javaDirChanged();
  void javaArgsChanged();
  void resolutionChanged();

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

  QString getName();
  ProfileType getType();
  QString getCreated();
  QString getLastUsed();
  QString getIcon();
  QString getLastVersionId();
  QVariant getGameDir();
  QVariant getJavaDir();
  QVariant getJavaArgs();
  QVariant getResolution();

  void setName(const QString& name);
  void setType(const ProfileType& type);
  void setCreated(const QString& created);
  void setLastUsed(const QString& lastUsed);
  void setIcon(const QString& icon);
  void setLastVersionId(const QString& lastVersionId);
  void setGameDir(const QVariant& gameDir);
  void setJavaDir(const QVariant& javaDir);
  void setJavaArgs(const QVariant& javaArgs);
  void setResolution(const QVariant& resolution);

  inline static const Resolution defaultResolution {
    .width = 854,
    .height = 480,
  };

  inline static const QString defaultJavaArgs {"-Xms2G -Xmx4G -XX:+UseCompactObjectHeaders -XX:+AlwaysPreTouch -XX:+UseStringDeduplication"};

private:
  QString m_name {"(Default)"};
  ProfileType m_type {ProfileType::LATEST_RELEASE};
  QString m_created {"1970-01-01T00:00:00.000Z"};
  QString m_lastUsed {"1970-01-01T00:00:00.000Z"};
  QString m_icon {"grass"};
  QString m_lastVersionId {"latest-release"};
  std::optional<QString> m_gameDir {std::nullopt};
  std::optional<QString> m_javaDir {std::nullopt};
  std::optional<QString> m_javaArgs {std::nullopt};
  std::optional<Resolution> m_resolution {std::nullopt};
};

#endif  // LJ_LAUNCHER_PROFILE_H_
