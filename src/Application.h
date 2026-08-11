//
// Created by jay on 11/08/2026.
//

#ifndef LJ_LAUNCHER_APPLICATION_H_
#define LJ_LAUNCHER_APPLICATION_H_
#include <qqml.h>

#include <QObject>
#include <QString>


class Application : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString defaultIcon READ getDefaultIcon CONSTANT)
  Q_PROPERTY(QString defaultTitle READ getDefaultTitle CONSTANT)
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Application(QObject *parent = nullptr);

  QString getDefaultIcon() { return kDefaultIcon; }
  QString getDefaultTitle() { return kDefaultTitle; }

  static QString getApplicationName() { return kApplicationName; };
  static QString getOrganisationDomain() { return kOrganisationDomain; };
  static QString getOrganisationName() { return kOrganisationName; };

private:
  inline static const QString kDefaultIcon {":/qt/qml/assets/icons/icon.png"};
  inline static const QString kDefaultTitle {"LJ Launcher"};

  inline static const QString kApplicationName {"LJ-Launcher"};
  inline static const QString kOrganisationDomain {"jayrickaby.com"};
  inline static const QString kOrganisationName {"JayRickaby"};
};


#endif //LJ_LAUNCHER_APPLICATION_H
