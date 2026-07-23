import threading

import minecraft_launcher_lib
import requests
from PySide6.QtCore import Property, QObject, QUrl, Signal, Slot, QUrlQuery

from settings import settings

CLIENT_ID = "478514ce-2d7e-4e71-9301-29eb2241e2d6"
REDIRECT_URI = "http://localhost"

class Authentication(QObject):
    authenticatedChanged = Signal()
    usernameChanged = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)

        self._authenticated = False
        self._url, self._state, self._verifier = self._get_login_data()
        self._username = None
        self._uuid = None
        self._token = None

    def _complete_auth(self, url):
        try:
            code = self._get_auth_code(url)

        except AssertionError as e:
            print(e)
            print("Could not authenticate user!")
            self._set_authenticated(False)
            return

        try:
            result = self._get_auth_result(code)

        except Exception as e:
            print(e)
            print("Could not authenticate user!")
            self._set_authenticated(False)
            return

        self._save_auth_data(result)
        print("User successfully authenticated")
        self._set_authenticated(True)

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
        while True:
            try:
                result = minecraft_launcher_lib.microsoft_account.complete_refresh(
                        client_id=CLIENT_ID,
                        client_secret=None,
                        redirect_uri=REDIRECT_URI,
                        refresh_token=settings.refresh_token
                )
            except requests.exceptions.ConnectionError as e:
                print(e)
                print("Trying again!")
            else:
                break

        return result

    def _save_auth_data(self, result):
        if "id" in result:
            self._uuid = result["id"]

        if "name" in result:
            self._username = result["name"]
            self.usernameChanged.emit()

        if "access_token" in result:
            self._token = result["access_token"]

        if "refresh_token" in result:
            settings.set_refresh_token(result["refresh_token"])

    def _set_authenticated(self, state):
        if self._authenticated == state:
            return

        self._authenticated = state
        self.authenticatedChanged.emit()

    def _try_stored_refresh(self):
        if not self.has_stored_refresh:
            print("Could not authenticate user!")
            self._set_authenticated(False)
            return

        try:
            result = self._get_refresh()

        except minecraft_launcher_lib.exceptions.InvalidRefreshToken:
            settings.clear_refresh_token()
            print("Could not authenticate user!")
            self._set_authenticated(False)
            return

        except (KeyError, Exception) as e:
            print(f"Could not authenticate user! Missing {e}, please try "
                  f"again!")
            self._set_authenticated(False)
            return


        self._save_auth_data(result)
        print("User successfully authenticated")
        self._set_authenticated(True)

    @Property(bool, notify=authenticatedChanged)
    def authenticated(self):
        return self._authenticated

    @Property(QUrl, constant=True)
    def login_url(self):
        return self._url

    @Property(str, notify=usernameChanged)
    def username(self):
        if self._username is None:
            return "guest"

        return self._username

    @Property("QVariant") # allow None to be returned
    def uuid(self):
        return self._uuid

    @Property("QVariant") # allow None to be returned
    def token(self):
        return self._token

    @Slot(QUrl)
    def complete_auth(self, url):
        thread = threading.Thread(target=self._complete_auth, args=(url,))
        thread.start()

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

    @Slot()
    def try_stored_refresh(self):
        thread = threading.Thread(target=self._try_stored_refresh)
        thread.start()

    @Slot(result=bool)
    def has_stored_refresh(self):
       return settings.refresh_token is not None

authentication = Authentication()