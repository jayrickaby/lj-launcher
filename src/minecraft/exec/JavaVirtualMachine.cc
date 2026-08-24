//
// Created by jay on 24/08/2026.
//

#include "JavaVirtualMachine.h"

QProcessEnvironment JavaVirtualMachine::s_variables {};

QString JavaVirtualMachine::getVariable(const QString& name) {
  if (!s_variables.contains(name)) {
    qDebug() << "Unknown variable name:" << name;
  }
  return s_variables.value(name);
}

void JavaVirtualMachine::setVariable(const QString& name,
                                     const QString& value) {
  if (value.isEmpty() or name.isEmpty()) {
    return;
  }
  s_variables.insert(name, value);
}

QStringList JavaVirtualMachine::resolveArguments(const QStringList& args) {
  QStringList result;

  for (auto arg : args) {
    QRegularExpressionMatchIterator iterator = REGEX.globalMatch(arg);

    QList<QRegularExpressionMatch> matches;
    while (iterator.hasNext()) {
      matches.append(iterator.next());
    }

    for (const auto& match : matches) {
      QString name = match.captured(1);
      QString value = getVariable(name);

      arg.replace(match.capturedStart(0), match.capturedLength(), value);
    }

    result.append(arg);
  }
  return result;
}

void JavaVirtualMachine::appendVariable(const QString& name,
                                           const QString& value,
                                           const QString& seperator) {
  if (name.isEmpty() or value.isEmpty()) {
    return;
  }

  if (!s_variables.contains(name)) {
    setVariable(name, value);
  }

  QStringList items {s_variables.value(name), value};
  s_variables.insert(name, items.join(seperator));
}