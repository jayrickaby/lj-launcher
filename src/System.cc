//
// Created by jay on 13/08/2026.
//

#include "System.h"

bool System::touch(const QString& path, bool existsOk) {
  QFileInfo const FILE_INFO {path};
  if (FILE_INFO.exists()) {
    QString const MSG {"The file %1 already exists!"};
    qDebug() << MSG.arg(path);
    return existsOk;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    QString const MSG {"Failed to create %1!"};
    qDebug() << MSG.arg(path);
    return false;
  }

  // Close early as only creating the file
  file.close();
  QString const MSG {"Successfully created %1!"};
  qDebug() << MSG.arg(path);
  return true;
}

bool System::write(const QString& path, const QString& content) {
  QFileInfo const FILE_INFO {path}; 
  if (!FILE_INFO.exists()) {
    QString const MSG {"The file %1 does not exist! Attempted to write: %2"};
    qDebug() << MSG.arg(path, content);
    return false;
  }

  QFile file {path};
  if (!file.open(QIODevice::WriteOnly)) {
    QString const MSG {"Failed to write to %1!"};
    qDebug() << MSG.arg(path);
    return false;
  }

  QTextStream stream {&file};
  stream << content;
  file.close();

  QString const MSG {"Successfully wrote to %1!"};
  qDebug() << MSG.arg(path);
  return true;
}

QString System::read(const QString& path) {
  QFileInfo const FILE_INFO {path};
  if (!FILE_INFO.exists()) {
    QString const MSG {"The file %1 does not exists!"};
    qDebug() << MSG.arg(path);
    return {};
  }

  QFile file {path};
  if (!file.open(QIODevice::ReadOnly)) {
    QString const MSG {"Failed to read from %1!"};
    qDebug() << MSG.arg(path);
    return {};
  }

  QTextStream stream {&file};
  QString contents {stream.readAll()};
  file.close();

  QString const MSG {"Successfully read from %1!"};
  qDebug() << MSG.arg(path);
  return contents;
}

QString System::which(const QString& path) {
  QString foundPath {QStandardPaths::findExecutable(path)};

  if (foundPath.isEmpty()) {
    const QString MSG {"The executable \"%1\" could not be found!"};
    qDebug() << MSG.arg(path);
    return {};
  }
  const QString MSG {"Found %1 at %2"};
  qDebug() << MSG.arg(path, foundPath);

  return foundPath;
}

QString System::getOs() {
  QString name {QSysInfo::kernelType()};

  if (name == "winnt") { return "windows"; }
  if (name == "darwin") { return "osx"; }

  return name;
}

QString System::getArchitecture() {
  QString arch {QSysInfo::currentCpuArchitecture()};
  if (arch == "i386") { return "x86"; }

  return arch;
}

QString System::getOsVersion() {
  return QSysInfo::kernelVersion();
}

QByteArray System::getSha1Checksum(const QByteArray& data) {
  return QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex();
}