import subprocess
from xml.dom import InvalidAccessErr

import mc_launcher_utils as mc
import minecraft_launcher_lib

from PySide6.QtCore import QObject, Property, Signal, Slot, QUrl
from PySide6.QtGui import QGuiApplication

from authentication import authentication
from downloader import downloader
from profiles import profiles

class Launcher(QObject):
    userMessageChanged = Signal()
    versionParametersChanged = Signal()
    settingsChanged = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self._dir = mc.launcher.get_game_directory()
        self._java = mc.launcher.get_java_executable()

        authentication.authenticatedChanged.connect(self.userMessageChanged)
        authentication.usernameChanged.connect(self.userMessageChanged)
        profiles.current_profile_changed.connect(self.userMessageChanged)
        profiles.profile_properties_changed.connect(self.userMessageChanged)

    def _generate_options(self):
        return  {
            'username': authentication.username,
            'uuid': authentication.uuid,
            'token': authentication.token
        }

    def _launch_game(self):
        options = self._generate_options()

        command = minecraft_launcher_lib.command.get_minecraft_command(
            profiles.get_profile_version(profiles.current_id),
            self._dir,
            options
        )

        # cwd ensures game logs into game directory
        subprocess.Popen(command, cwd=self._dir)
        QGuiApplication.instance().quit()

    @Property(QUrl, constant=True)
    def game_directory(self):
        return QUrl(str(self._dir))

    @Property(QUrl, constant=True)
    def java_executable(self):
        return QUrl(str(self._java))

    @Property(str, notify=userMessageChanged)
    def user_message(self):
        authenticated = authentication.authenticated
        username = authentication.username

        message = f"Welcome, <b>{username}</b>"

        if not authenticated:
            message += "! Please log in."

        message += "<br>"

        if authenticated:
            download_message = (
                ""
                if mc.versions.is_downloaded(profiles.get_profile_version(profiles.current_id))
                else "download & "
            )

            message += f"Ready to {download_message}play Minecraft {
                profiles.get_profile_version(profiles.current_id)}"

        else:
            # TODO: should wait until versions are actually loaded
            message += "Loading versions..."

        return message

    @Property(str, constant=True)
    def java_args(self):
        return profiles.get_current_profile().get_default_java_args()

    @Property(list, notify=versionParametersChanged)
    def online_versions(self) -> list[dict[str, str]]:
        print("Getting Versions List!")

        get_snapshots = mc.profiles.get_setting("enableSnapshots")
        get_historical = mc.profiles.get_setting("enableHistorical")


        versions = mc.versions.query_online(
            snapshots=get_snapshots,
            historical=get_historical
        )


        available_versions = [
            {"id": "latest-release", "name": "Use Latest Release"},
        ]

        if get_snapshots:
            available_versions.append({"id": "latest-snapshot", "name": "Use Latest Snapshot"})

        for version in versions:
            type = version.get_type()

            if type in ["old_alpha", "old_beta"]:
                type = type.replace("_", "-")

            name = f"{type} {version.get_id()}"

            available_versions.append(
                {"id": version.get_id(), "name": name}
            )

        return available_versions

    @Slot()
    def play(self):
        if not mc.versions.is_downloaded(profiles.get_profile_version(profiles.current_id)):
            downloader.downloadFinished.connect(self._launch_game)
            downloader.download(profiles.get_profile_version(profiles.current_id), self._dir)
            return

        self._launch_game()


    @Property(dict, notify=settingsChanged)
    def settings(self):
        return mc.profiles.get_settings()

    @Slot(str, str)
    def set_setting(self, key, value):
        if value.lower() == 'true':
            value = True
        elif value.lower() == 'false':
            value = False

        if mc.profiles.get_settings()[key] == value:
            return

        mc.profiles.edit_setting(key, value)

        print(f"Setting {key} to {value}")

        if key in ["enableHistorical", "enableSnapshots"]:
            self.versionParametersChanged.emit()

launcher = Launcher()