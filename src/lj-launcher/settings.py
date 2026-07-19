from PySide6.QtCore import QObject, Property, Slot, QUrl, QSettings
from PySide6.QtQml import QmlElement

from application import application, ORG_NAME, APP_NAME

QML_IMPORT_NAME = "jayrickaby.lj_launcher.settings"
QML_IMPORT_MAJOR_VERSION = 1

@QmlElement
class Settings(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._settings = QSettings(ORG_NAME, APP_NAME)

        self._settings.beginGroup("Account")
        self._id = self._settings.value(
            "id", None
        )
        self._name = self._settings.value(
            "name", None
        )
        self._refreshToken = self._settings.value(
            "refreshToken", None
        )
        self._settings.endGroup()

    @Property(str)
    def id(self):
        return self._id

    @Property(str)
    def name(self):
        return self._name

    @Property(str)
    def refresh_token(self):
        return self._refreshToken

    @Slot()
    def clear_refresh_token(self):
        self._refreshToken = None
        self._settings.remove("Account/refreshToken")

    @Slot(str)
    def set_id(self, id):
        self._id = id
        self._settings.setValue("Account/id", id)

    @Slot(str)
    def set_name(self, name):
        self._name = name
        self._settings.setValue("Account/name", name)

    @Slot(str)
    def set_refresh_token(self, token):
        self._refreshToken = token
        self._settings.setValue("Account/refreshToken", token)

settings = Settings()