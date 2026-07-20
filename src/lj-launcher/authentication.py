import minecraft_launcher_lib
from PySide6.QtCore import Property, QObject, QUrl, Signal, Slot, QUrlQuery
from PySide6.QtQml import QmlElement

from settings import settings

CLIENT_ID = "478514ce-2d7e-4e71-9301-29eb2241e2d6"
REDIRECT_URI = "http://localhost"

class Authentication(QObject):

    authenticated = Signal(bool)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._url, self._state, self._verifier = self._get_login_data()

    def _get_auth_code(self, url):
        code = minecraft_launcher_lib.microsoft_account.parse_auth_code_url(
            url.toString(), self._state
        )

        return code

    def _get_auth_result(self, code):
        result = minecraft_launcher_lib.microsoft_account.complete_login(
            client_id=CLIENT_ID,
            client_secret=None,
            redirect_uri=REDIRECT_URI,
            auth_code=code,
            code_verifier=self._verifier
        )

        return result

    def _get_login_data(self):
        data = minecraft_launcher_lib.microsoft_account.get_secure_login_data(
            client_id=CLIENT_ID,
            redirect_uri=REDIRECT_URI
        )

        return data

    def _get_refresh(self):
        result = minecraft_launcher_lib.microsoft_account.complete_refresh(
                client_id=CLIENT_ID,
                client_secret=None,
                redirect_uri=REDIRECT_URI,
                refresh_token=settings.refresh_token
        )

        return result

    def _save_auth_data(self, result):
        if "refresh_token" in result:
            settings.set_refresh_token(result["refresh_token"])

    @Property(QUrl, constant=True)
    def login_url(self):
        return self._url

    @Slot(QUrl, result=bool)
    def complete_auth(self, url):
        try:
            code = self._get_auth_code(url)

        except AssertionError:
            print("Assertion Error!")
            self.authenticated.emit(False)
            return False

        try:
            result = self._get_auth_result(code)

        except Exception as e:
            print(e)
            self.authenticated.emit(False)
            return False

        self._save_auth_data(result)
        self.authenticated.emit(True)
        return True

    @Slot(QUrl, result=bool)
    def is_url_localhost(self, url):
        return url.host() == "localhost"

    @Slot(QUrl, result=dict)
    def parse_localhost_url(self, url):
        query = QUrlQuery(url.query())

        data = {}

        for key, value in query.queryItems():
            data[key] = value

        return data

    @Slot(result=bool)
    def try_stored_refresh(self):
        if settings.refresh_token is None:
            self.authenticated.emit(False)
            return False

        try:
            result = self._get_refresh()

        except minecraft_launcher_lib.exceptions.InvalidRefreshToken:
            settings.clear_refresh_token()
            self.authenticated.emit(False)
            return False

        self._save_auth_data(result)
        self.authenticated.emit(True)
        return True


authentication = Authentication()