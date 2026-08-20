//
// Created by jay on 12/08/2026.
//

#include "Launcher.h"


Launcher* Launcher::s_instance {nullptr};
QUrl Launcher::s_gameDirectory;
QUrl Launcher::s_javaExecutable {QUrl::fromLocalFile(System::which("java"))}; // stores properly
QString Launcher::s_username;

Launcher::Launcher(QObject *parent)
  : QObject(parent) {
  connect(this, &Launcher::usernameChanged,
      this, &Launcher::userMessageChanged);

  connect(Authentication::getInstance(), &Authentication::authStateChanged,
    this, &Launcher::userMessageChanged);

  connect(Profiles::getInstance(), &Profiles::currentProfileIdChanged,
    this, &Launcher::userMessageChanged);

  connect(Profiles::getInstance(), &Profiles::profilesChanged,
    this, &Launcher::userMessageChanged);

  connect(Versions::getInstance(), &Versions::stateChanged,
  this, &Launcher::userMessageChanged);

  if (!s_instance) {
    s_instance = this;
  }
}

QUrl Launcher::getJavaExecutable() {
  if (s_javaExecutable.isEmpty() or !s_javaExecutable.isValid()) {
    throw std::runtime_error("Java path could not be found. Are you sure Java is installed?");
  }

  return s_javaExecutable;
}

QString Launcher::userMessage() {
  qDebug() << "Updating user message...";
  const bool AUTHENTICATED {Authentication::getAuthState() == Authentication::AuthState::AUTHENTICATED};
  const bool MANIFEST_DOWNLOADED {Versions::getState() == Versions::ManifestState::PRESENT};
  const QString CURRENT_VERSION{Profiles::getCurrentProfileVersion()};

  QString const USERNAME {getUsername()};

  QString message {QString("Welcome, <b>%1</b>").arg(USERNAME)};

  if (!AUTHENTICATED) {
    message.append("! Please log in.");
  }

  message.append("<br>");

  if (MANIFEST_DOWNLOADED and AUTHENTICATED) {
    const QString DOWNLOAD_MESSAGE {Versions::isDownloaded(CURRENT_VERSION) ? "" : "download & "};

    const QString READY_MESSAGE {"Ready to %1play Minecraft %2"};

    message.append(READY_MESSAGE.arg(DOWNLOAD_MESSAGE, CURRENT_VERSION));
  }
  else {
    message.append("Loading versions...");
  }

  return message;
}

void Launcher::sendError(ErrorMessage& message) {
  if (!s_instance) {
    return;
  }
  if (message.errorFriendly.isEmpty()) {
    message.errorFriendly = "Oops, an error has occurred!";
  }
  emit s_instance->launcherError(message);
}

QUrl Launcher::findGameDirectory() {
  QString rootPath;
#ifdef _WIN32
  rootPath = {QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)};
#elifdef __linux__
  rootPath = {QStandardPaths::writableLocation(QStandardPaths::HomeLocation)};
#else
  throw std::runtime_error("Unknown platform.");
#endif
  QString const FULL_PATH {QDir(rootPath).filePath(".minecraft")};

  QDir const GAME_DIR(FULL_PATH);
  if (!GAME_DIR.exists()) {
    GAME_DIR.mkpath(".");
    qDebug() << "Created game directory.";
  }
  qDebug() << "Found game directory: " << GAME_DIR.absolutePath();

  QUrl const GAME_URL = QUrl::fromLocalFile(FULL_PATH);
  return GAME_URL;
};

QUrl Launcher::getGameDirectory() {
  if (s_gameDirectory.isEmpty() or !s_gameDirectory.isValid()) {
    s_gameDirectory = findGameDirectory();
  }
  return s_gameDirectory;
}

void Launcher::setUsername(const QString& username) {
  if (s_username == username) { return; }
  qDebug() << "Setting username to:" << username;
  s_username = username;
  emit s_instance->usernameChanged();
}

QString Launcher::getUsername() {
  if (s_username.isEmpty() or s_username.isNull()) {
    return "guest";
  }

  return s_username;
}

QString Launcher::getTime(bool def) {
  if (def) { return "1970-01-01T00:00:00.000Z"; }

  QDateTime const DATE_TIME = QDateTime::currentDateTimeUtc();
  return DATE_TIME.toString(Qt::ISODateWithMs);
}

Launcher* Launcher::getInstance() {
  if (!s_instance) {
    s_instance = new Launcher();
  }
  return s_instance;
}

void Launcher::play() {
  const QString CURRENT_VER {Profiles::getCurrentProfileVersion()};

  if (!Versions::isDownloaded(CURRENT_VER)) {
    const QString ONLINE_URL {Versions::getAvailableVersion(CURRENT_VER).value("url").toString()};

    const QString VERSIONS_DIR {Versions::getVersionsDirectory()};

    const QString LOCAL_URL {
      QString("%1/%2/%2.json").arg(VERSIONS_DIR, CURRENT_VER)
    };
    Downloader::addDownload(ONLINE_URL, LOCAL_URL);
  }
}