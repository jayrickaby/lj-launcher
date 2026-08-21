//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_APPLICATION_H_
#define LJ_LAUNCHER_APPLICATION_H_
#include <qqml.h>

#include <QObject>
#include <QString>

#include "Launcher.h"

class Application : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString defaultIcon READ defaultIcon CONSTANT)
  Q_PROPERTY(QString defaultTitle READ defaultTitle CONSTANT)

public:
  explicit Application(QObject *parent = nullptr);

  [[nodiscard]] QString defaultIcon() const { return DEFAULT_ICON; };
  [[nodiscard]] QString defaultTitle() const { return DEFAULT_TITLE; };

  [[nodiscard]] static QString getDefaultIcon() { return DEFAULT_ICON; }
  [[nodiscard]] static QString getDefaultTitle() { return DEFAULT_TITLE; }

  [[nodiscard]] static QString getApplicationName() { return APPLICATION_NAME; };
  [[nodiscard]] static QString getApplicationVersion() { return APPLICATION_VERSION; };
  [[nodiscard]] static QString getOrganisationDomain() { return ORGANISATION_DOMAIN; };
  [[nodiscard]] static QString getOrganisationName() { return ORGANISATION_NAME; };

  static Application* getInstance();

private:
  inline static const QString DEFAULT_ICON {":/jayrickaby/lj_launcher/assets/icons/icon.png"};
  inline static const QString DEFAULT_TITLE {"LJ Launcher"};

  inline static const QString APPLICATION_NAME {"LJ-Launcher"};
  inline static const QString APPLICATION_VERSION {"v0.5.0a"};
  inline static const QString ORGANISATION_DOMAIN {"jayrickaby.com"};
  inline static const QString ORGANISATION_NAME {"JayRickaby"};

  static Application* s_instance;
};


#endif //LJ_LAUNCHER_APPLICATION_H_
