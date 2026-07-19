from pathlib import Path
from PySide6.QtCore import QObject, Property, QUrl
from PySide6.QtQml import QmlElement

QML_IMPORT_NAME = "jayrickaby.lj_launcher.application"
QML_IMPORT_MAJOR_VERSION = 1

ORG_NAME = "JayRickaby"
ORG_DOMAIN = "jayrickaby.com"
APP_NAME = "LJ-Launcher"

@QmlElement
class Application(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._default_title = "LJ Launcher"
        self._parent_path = Path(__file__).parent

    @Property(str, constant=True)
    def default_icon(self):
        path = self._parent_path / "qml" / "assets" / "icons" / "icon.png"

        return str(path)

    @Property(str, constant=True)
    def default_title(self):
        return self._default_title

    @Property(QUrl, constant=True)
    def external_folder(self):
        path = self._parent_path.parent.parent.absolute() / "external"

        return QUrl.fromLocalFile(str(path))

    @Property(QUrl, constant=True)
    def parent_path(self):
        path = self._parent_path.absolute()

        return QUrl.fromLocalFile(path)

    @Property(QUrl, constant=True)
    def project_root_folder(self):
        path = self._parent_path.parent.parent.absolute()

        return QUrl.fromLocalFile(path)


application = Application()
