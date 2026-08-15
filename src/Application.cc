//
// Created by jay on 11/08/2026.
//

#include "Application.h"
#include "Launcher.h"

Application* Application::s_instance {nullptr};

Application::Application(QObject* parent)
: QObject(parent)
{
  if (!s_instance) {
    s_instance = this;
  }
}

Application* Application::getInstance() {
  if (!s_instance) {
    s_instance = new Application();
  }
  return s_instance;
}