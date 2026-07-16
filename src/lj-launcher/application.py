from PySide6.QtCore import QObject, Property, QUrl
from PySide6.QtQml import QmlElement
from pathlib import Path

QML_IMPORT_NAME = "jayrickaby.lj_launcher.application"
QML_IMPORT_MAJOR_VERSION = 1

ORG_NAME = "JayRickaby"
ORG_DOMAIN = "jayrickaby.com"
APP_NAME = "LJ-Launcher"

@QmlElement
class Application(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.__parentPath = Path(__file__).parent
        self.__defaultTitle = "LJ Launcher"

    @Property(str, constant=True)
    def defaultIcon(self):
        return str(self.__parentPath / "qml" / "assets" / "icons" / "icon.png")

    @Property(QUrl, constant=True)
    def parentPath(self):
        return QUrl.fromLocalFile(self.__parentPath)

    @Property(QUrl, constant=True)
    def projectRootFolder(self):
        return QUrl.fromLocalFile((self.__parentPath.parent.parent.absolute()))

    @Property(QUrl, constant=True)
    def externalFolder(self):
        return QUrl.fromLocalFile(str(self.__parentPath.parent.parent.absolute() / "external"))

    @Property(str, constant=True)
    def defaultTitle(self):
        return self.__defaultTitle

application = Application()
