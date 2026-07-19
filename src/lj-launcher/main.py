# This Python file uses the following encoding: utf-8
import sys

from PySide6.QtGui import QGuiApplication, QIcon
from PySide6.QtQml import QQmlApplicationEngine, qmlRegisterSingletonInstance
from PySide6.QtWebEngineQuick import QtWebEngineQuick

from application import application, ORG_NAME, ORG_DOMAIN, APP_NAME
from authentication import authentication

if __name__ == "__main__":
    QtWebEngineQuick.initialize()

    app = QGuiApplication(sys.argv)
    app.setOrganizationName(ORG_NAME)
    app.setOrganizationDomain(ORG_DOMAIN)
    app.setApplicationName(APP_NAME)

    app.setWindowIcon(QIcon(application.default_icon))

    app.setDesktopFileName("lj-launcher")

    qmlRegisterSingletonInstance(
        type(application), "jayrickaby.lj_launcher.application",
        1, 0,
        "Application", application
     )

    qmlRegisterSingletonInstance(
        type(authentication), "jayrickaby.lj_launcher.authentication",
        1, 0,
        "Authentication", authentication
    )

    engine = QQmlApplicationEngine()
    engine.addImportPath(application.parent_path.toLocalFile())
    engine.loadFromModule("qml", "Main")

    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec())
