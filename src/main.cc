//
// Created by jay on 11/08/2026.
//
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Application.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QGuiApplication::setApplicationName(Application::getApplicationName());
  QGuiApplication::setOrganizationDomain(Application::getOrganisationDomain());
  QGuiApplication::setOrganizationName(Application::getOrganisationName());

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