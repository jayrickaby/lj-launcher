import subprocess

import minecraft_launcher_lib

from PySide6.QtCore import QObject, Property, Signal, Slot
from PySide6.QtGui import QGuiApplication

from authentication import authentication
from downloader import downloader

class Launcher(QObject):
    currentProfileChanged = Signal()
    profileNamesChanged = Signal()
    userMessageChanged = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self._dir = self._get_minecraft_dir()

        self._profiles = self._get_profiles(self._dir)

        self._currentProfile = None

        authentication.authenticatedChanged.connect(self.userMessageChanged)
        authentication.usernameChanged.connect(self.userMessageChanged)

        self.currentProfileChanged.connect(self.userMessageChanged)

    def _generate_options(self):
        return  {
            'username': authentication.username,
            'uuid': authentication.uuid,
            'token': authentication.token
        }

    def _get_available_ver(self, mc_dir):
        return minecraft_launcher_lib.utils.get_available_versions(mc_dir)

    def _get_profiles(self, mc_dir):
        return minecraft_launcher_lib.vanilla_launcher.load_vanilla_launcher_profiles(mc_dir)

    def _get_current_profile_version(self):
        if self._currentProfile["versionType"] == "latest-release":
            return self._get_latest_version()["release"]

        if self._currentProfile["versionType"] == "latest-snapshot":
            return self._get_latest_version()["snapshot"]

        if self._currentProfile["version"] == "latest-version":
            return self._get_latest_version()["release"]

        return self._currentProfile["version"]

    def _get_installed_versions(self, mc_dir):
        return minecraft_launcher_lib.utils.get_installed_versions(mc_dir)

    def _get_latest_version(self):
        return minecraft_launcher_lib.utils.get_latest_version()

    def _get_minecraft_dir(self):
        return minecraft_launcher_lib.utils.get_minecraft_directory()

    def _is_current_profile_version_downloaded(self):
        ver = self._get_current_profile_version()
        installed = self._get_installed_versions(self._dir)

        for install in installed:
            if install["id"] == ver:
                return True

        return False

    def _launch_game(self):
        options = self._generate_options()

        command = minecraft_launcher_lib.command.get_minecraft_command(
            self._get_current_profile_version(), self._dir, options
        )

        # cwd ensures game logs into game directory
        subprocess.Popen(command, cwd=self._dir)
        QGuiApplication.instance().quit()

    @Property("QVariant", notify=currentProfileChanged)
    def current_profile(self):
        return self._currentProfile

    @current_profile.setter
    def current_profile(self, index):
        self._currentProfile = self._profiles[index]
        self.currentProfileChanged.emit()

    @Property(list, notify=profileNamesChanged)
    def profile_names(self):
        names = []

        for profile in self._profiles:
            names.append(profile["name"])

        return names

    @Property(str, notify=userMessageChanged)
    def user_message(self):
        authenticated = authentication.authenticated
        username = authentication.username

        message = f"Welcome <b>{username}</b>"

        if not authenticated:
            message += "! Please log in."

        message += "<br>"

        if authenticated:
            download_message = (
                ""
                if self._is_current_profile_version_downloaded()
                else "download & "
            )

            message += f"Ready to {download_message}play Minecraft {
                self._get_current_profile_version()}"

        else:
            message += "Loading versions..."

        return message

    @Slot()
    def play(self):
        if not self._is_current_profile_version_downloaded():
            downloader.downloadFinished.connect(self._launch_game)
            downloader.download(self._get_current_profile_version(), self._dir)
            return

        self._launch_game()

launcher = Launcher()