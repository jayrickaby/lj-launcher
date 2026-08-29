  //
  // Created by jay on 11/08/2026.
  //
  #include <QGuiApplication>
  #include <QIcon>
  #include <QLoggingCategory>
  #include <QQmlApplicationEngine>
  #include <QQuickStyle>

#include "Application.h"
#include "ProfileEntry.h"
#include "Profiles.h"
#include "ProfilesTable.h"
#include "minecraft/exec/Game.h"
#include "minecraft/exec/JavaVirtualMachine.h"
#include "minecraft/ver/VersionManifest.h"
#include "net/Authentication.h"
#include "net/Downloader.h"
#include "net/Versions.h"

int main(int argc, char *argv[]) {
  qputenv("QT_FORCE_STDERR_LOGGING", "1");
  QGuiApplication app(argc, argv);

  // Default to org.kde.desktop style unless the user forces another style
  auto system {SystemInfo::getOperatingSystem()};
  if (system.name == SystemName::LINUX and qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
    // QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    QQuickStyle::setStyle("Fusion");
  }

  Launcher::addLog(
    QString("%1 %2 started on %3...")
    .arg(
      Application::getApplicationName(),
      Application::getApplicationVersion(),
      QSysInfo::productType()
    )
  );

  JavaVirtualMachine::setVariable("launcher_name", Application::getApplicationName());
  JavaVirtualMachine::setVariable("launcher_version", Application::getApplicationVersion());

  QGuiApplication::setApplicationName(Application::getApplicationName());
  QGuiApplication::setOrganizationDomain(Application::getOrganisationDomain());
  QGuiApplication::setOrganizationName(Application::getOrganisationName());

  QGuiApplication::setWindowIcon(QIcon(Application::getDefaultIcon()));

  ProfileManager::refreshProfiles();

  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Application", Application::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Authentication", Authentication::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Downloader", Downloader::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Game", Game::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Launcher", Launcher::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Profiles", Profiles::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Versions", Versions::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "VersionManifest", VersionManifest::getInstance());

  qmlRegisterType<ProfilesTable>("jayrickaby.lj_launcher", 1, 0, "ProfilesTable");

  QQmlApplicationEngine engine;
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreationFailed,
    &app, []() {
      QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection
  );

  engine.loadFromModule("jayrickaby.lj_launcher", "Main");

  return QGuiApplication::exec();
}