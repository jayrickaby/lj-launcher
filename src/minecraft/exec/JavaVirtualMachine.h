//
// Created by jay on 24/08/2026.
//

#ifndef LJ_LAUNCHER_JAVAVIRTUALMACHINE_H_
#define LJ_LAUNCHER_JAVAVIRTUALMACHINE_H_
#include <QProcessEnvironment>
#include <QRegularExpression>

#include <QString>

class JavaVirtualMachine {
public:
  static QString getVariable(const QString& name);
  static void appendVariable(const QString& name, const QString& value, const QString& seperator);
  static void setVariable(const QString& name, const QString& value);

  static QStringList resolveArguments(const QStringList& args);

private:
  static QProcessEnvironment s_variables;
  inline static const QRegularExpression REGEX{"\\$\\{([^}]+)\\}"};


};

#endif  // LJ_LAUNCHER_JAVAVIRTUALMACHINE_H_
