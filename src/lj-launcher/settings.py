
from application import application, ORG_NAME, APP_NAME
from PySide6.QtCore import QObject, Property, Slot, QUrl, QSettings
from PySide6.QtQml import QmlElement

QML_IMPORT_NAME = "jayrickaby.lj_launcher.settings"
QML_IMPORT_MAJOR_VERSION = 1

@QmlElement
class Settings(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)
        self._settings = QSettings(ORG_NAME, APP_NAME)

        # Account
        self.__refreshToken = self._settings.value("account/refreshToken", None)
        self.__id = self._settings.value("account/id", None)
        self.__name = self._settings.value("account/name", None)

    @Property(str)
    def refreshToken(self):
        return self.__refreshToken

    @Property(str)
    def id(self):
        return self.__id

    @Property(str)
    def name(self):
        return self.__name

    @Slot(str)
    def setRefreshToken(self, token):
        self.__refreshToken = token
        self._settings.setValue("account/refreshToken", token)

    @Slot()
    def clearRefreshToken(self):
        self.__refreshToken = None
        self._settings.remove("account/refreshToken")

    @Slot(str)
    def setId(self, id):
        self.__id = id
        self._settings.setValue("account/id", id)

    @Slot(str)
    def setName(self, name):
        self.__name = name
        self._settings.setValue("account/name", name)

settings = Settings()