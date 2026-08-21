//
// Created by jay on 11/08/2026.
//
#include <QGuiApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

#include "Application.h"
#include "Profiles.h"
#include "Settings.h"
#include "minecraft/versions/VersionManifest.h"
#include "net/Authentication.h"
#include "net/Downloader.h"
#include "net/Versions.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QGuiApplication::setApplicationName(Application::getApplicationName());
  QGuiApplication::setOrganizationDomain(Application::getOrganisationDomain());
  QGuiApplication::setOrganizationName(Application::getOrganisationName());

  QGuiApplication::setWindowIcon(QIcon(Application::getDefaultIcon()));

  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Application", Application::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Authentication", Authentication::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Downloader", Downloader::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Launcher", Launcher::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Profiles", Profiles::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Settings", Settings::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "Versions", Versions::getInstance());
  qmlRegisterSingletonInstance("jayrickaby.lj_launcher", 1, 0, "VersionManifest", VersionManifest::getInstance());

  QQmlApplicationEngine engine;
  QObject::connect(
    &engine,
    &QQmlApplicationEngine::objectCreationFailed,
    &app,
    []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);

  engine.loadFromModule("jayrickaby.lj_launcher", "Main");

  return QGuiApplication::exec();
}