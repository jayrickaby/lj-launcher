//
// Created by jay on 11/08/2026.
//
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>

#include "Application.h"
#include "Settings.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QGuiApplication::setApplicationName(Application::getApplicationName());
  QGuiApplication::setOrganizationDomain(Application::getOrganisationDomain());
  QGuiApplication::setOrganizationName(Application::getOrganisationName());

  app.setWindowIcon(QIcon(Application::getDefaultIcon()));

  QSettings settings;
  Settings::setInstance(&settings);

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