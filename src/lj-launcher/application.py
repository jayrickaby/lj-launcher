from pathlib import Path
import sys
from PySide6.QtCore import Property, QObject, QUrl

ORG_NAME = "JayRickaby"
ORG_DOMAIN = "jayrickaby.com"
APP_NAME = "LJ-Launcher"

class Application(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._default_title = "LJ Launcher"

        if getattr(sys, 'frozen', False):
            _base = Path(sys.executable).parent
        else:
            _base = Path(__file__).parent

        self._parent_path = _base

    @Property(str, constant=True)
    def default_icon(self):
        path = self._parent_path / "qml" / "assets" / "icons" / "icon.png"

        return str(path)

    @Property(str, constant=True)
    def default_title(self):
        return self._default_title

    @Property(QUrl, constant=True)
    def parent_path(self):
        path = self._parent_path.absolute()

        return QUrl.fromLocalFile(path)

application = Application()
