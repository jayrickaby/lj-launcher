from contextlib import redirect_stderr

from PySide6.QtCore import Property, QObject, QUrl, Signal, Slot, QUrlQuery
from PySide6.QtQml import QmlElement
from settings import settings

import minecraft_launcher_lib

QML_IMPORT_NAME = "jayrickaby.lj_launcher.auth"
QML_IMPORT_MAJOR_VERSION = 1

CLIENT_ID = "478514ce-2d7e-4e71-9301-29eb2241e2d6"
REDIRECT_URI = "http://localhost"

@QmlElement
class Auth(QObject):

    authenticated = Signal(bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self.__result = None
        self.__loginUrl, self.__loginState, self.__loginCodeVerifier = minecraft_launcher_lib.microsoft_account.get_secure_login_data(
                                                                            client_id=CLIENT_ID,
                                                                            redirect_uri=REDIRECT_URI
                                                                        )

    @Property(QUrl, constant=True)
    def loginUrl(self):
        return(self.__loginUrl)

    @Slot(QUrl, result=bool)
    def isUrlLocalHost(self, url):
        return url.host() == "localhost"

    @Slot(QUrl, result=dict)
    def parseLocalHostUrl(self, url):
        query = QUrlQuery(url.query())

        data = {}

        for key, value in query.queryItems():
            data[key] = value

        return data

    @Slot(QUrl, result=bool)
    def completeAuth(self, url):
        try:
            code = minecraft_launcher_lib.microsoft_account.parse_auth_code_url(url.toString(), self.__loginState)

        except AssertionError:
            print("Assertion Error!")
            self.authenticated.emit(False)
            return False

        try:
            self.__result = minecraft_launcher_lib.microsoft_account.complete_login(
                client_id=CLIENT_ID,
                client_secret=None,
                redirect_uri=REDIRECT_URI,
                auth_code=code,
                code_verifier=self.__loginCodeVerifier
            )

        except Exception as e:
            print(e)
            self.authenticated.emit(False)
            return False

        self.__saveAuthData(self.__result)
        self.authenticated.emit(True)
        return True

    def __saveAuthData(self, result):
        settings.setRefreshToken(result["refresh_token"])
        settings.setId(result["id"])
        settings.setName(result["name"])

    @Slot(result=bool)
    def tryStoredRefresh(self):
        if settings.refreshToken is None:
            self.authenticated.emit(False)
            return False

        try:
            self.__result = minecraft_launcher_lib.microsoft_account.complete_refresh(
                client_id=CLIENT_ID,
                client_secret=None,
                redirect_uri=REDIRECT_URI,
                refresh_token=settings.refreshToken
            )

        except minecraft_launcher_lib.exceptions.InvalidRefreshToken:
            settings.clearRefreshToken()
            self.authenticated.emit(False)
            return False

        self.__saveAuthData(self.__result)
        self.authenticated.emit(True)
        return True


auth = Auth()