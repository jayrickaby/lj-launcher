from PySide6.QtGui import QStandardItem

import mc_launcher_utils as mc
from PySide6.QtCore import QObject, Property, Signal, Slot, QRangeModel

from authentication import authentication

class Profile(QObject):
    current_profile_changed = Signal()
    profile_properties_changed = Signal()
    profile_list_changed = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)

        profiles = mc.profiles.get_profiles()
        self._current_id = next(iter(profiles))

        self._model = QRangeModel([])

        self.profile_properties_changed.connect(self.profile_list_changed)
        authentication.authenticatedChanged.connect(self._rename_default)
        self._mode: str | None = None

    def get_profile_version(self, id: str, *, raw=False):
        profile = mc.profiles.get_profile(id)
        if raw:
            return profile["lastVersionId"]

        elif profile["type"] == "latest-release":
            return mc.versions.get_latest()

        elif profile["type"] == "latest-snapshot":
            return mc.versions.get_latest(snapshot=True)

        return profile["lastVersionId"]

    def edit_profile(self, json: dict):
        mc.profiles.edit_profile(
            id=self._current_id,
            params=json
        )

        # for updated names
        self.profile_properties_changed.emit()

    def new_profile(self, json: dict):
        id = mc.profiles.create_profile(
            params=json,
        )

        self.set_current_profile(id)
        self.profile_list_changed.emit()

    def get_profile_names(self) -> list[str]:
        profiles = mc.profiles.get_profiles()

        names = []
        for id, profile in profiles.items():
            if id == self._current_id:
                continue

            names.append(profile["name"])

        return names

    def _get_unique_name(self, new_name: str):
        names = self.get_profile_names()
        base_name = new_name

        i = 2
        while new_name in names:
            new_name = f"{base_name} {i}"
            i += 1

        return new_name

    def _rename_default(self):
        if not authentication.authenticated:
            return

        profile = self.get_current_profile()

        if profile["name"] != "(Default)":
            return

        username = authentication.username

        profile_name = self._get_unique_name(username)

        self.edit_profile({"name": profile_name})

    def _refresh_model(self):
        rows = []
        profiles = mc.profiles.get_profiles()

        i = 0

        for id, profile in profiles.items():
            rows.append([profile["name"], profile["lastVersionId"], i])
            i += 1

        self._model = QRangeModel(rows)

    def get_current_profile(self):
        return mc.profiles.get_profile(id=self._current_id)

    def set_current_profile(self, id: str):
        if mc.profiles.is_profile(id):
            print(f"Setting current profile to: {id}")
            self._current_id = id
            self.current_profile_changed.emit()

    @Property("QVariant", notify=current_profile_changed)
    def current_profile(self):
        profile = self.get_current_profile().copy()

        if self._mode == "new":
            name = self._get_unique_name(f"Copy of {profile["name"]}")
            profile["name"] = name

        return profile

    @current_profile.setter
    def current_profile(self, id: str):
        self.set_current_profile(id)

    @Property(str, notify=current_profile_changed)
    def current_id(self):
        return self._current_id

    @Property(QObject, notify=profile_list_changed)
    def model(self):
        self._refresh_model()
        return self._model

    @Property(list, notify=profile_list_changed)
    def profile_list(self):
        names = []
        profiles = mc.profiles.get_profiles()

        for id, profile in profiles.items():
            names.append({"id": id, "name": profile["name"]})

        return names

    @Property(dict, constant=True)
    def default_resolution(self):
        return mc.profiles.get_default_resolution()

    @Property(str, constant=True)
    def default_java_args(self):
        return mc.profiles.get_default_java_args()

    @Property(str, constant=True)
    def current_version(self):
        return self.get_profile_version(self.current_id,raw=True)

    @Slot(str)
    def set_mode(self, mode: str):
        if mode in ["edit", "new"]:
            self._mode = mode

    @Slot(dict)
    def save(self, json: dict):
        if self._mode == "edit":
            print(f"Editing profile {self._current_id}")
            self.edit_profile(json)
        elif self._mode == "new":
            print(f"Creating new profile {json["name"]}")
            self.new_profile(json)

        self._mode = None

profiles = Profile()