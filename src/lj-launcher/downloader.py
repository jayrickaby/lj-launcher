import threading

import minecraft_launcher_lib
from PySide6.QtCore import Property, QObject, Signal, Slot


class Downloader(QObject):
    downloadingChanged = Signal()
    downloadFinished = Signal()

    downloadProgressMaxChanged = Signal()
    downloadProgressChanged = Signal()
    downloadStatusChanged = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)

        self._download_progress_max = None
        self._download_progress = None
        self._download_status = None

        self._downloading = False

        self._callback = {
                "setStatus": self._set_download_status,
                "setProgress": self._set_download_progress,
                "setMax": self._set_download_progress_max
            }

    def _download(self, version, directory):
        minecraft_launcher_lib.install.install_minecraft_version(
            version=version,
            minecraft_directory=directory,
            callback=self._callback
        )

    def _set_downloading(self, status: bool):
        if self._downloading == status:
            return

        self._downloading = status
        print(f"Downloading: {self._downloading}")
        self.downloadingChanged.emit()

    def _set_download_status(self, status: str):
        if self._download_status == status:
            return

        print(f"[STATUS]: {self._download_status}")

        self._download_status = status
        self.downloadStatusChanged.emit()

    def _set_download_progress(self, progress: int):
        if self._download_progress_max == 0:
            return

        if self._download_progress == progress:
            return

        print(f"[PROGRESS]: {self._download_progress}/{self._download_progress_max}")

        self._download_progress = progress
        self.downloadProgressChanged.emit()


    def _set_download_progress_max(self, new_max: int):
        if self._download_progress == new_max:
            return

        print(f"[DOWNLOAD_MAX]: {self._download_progress_max}")

        self._download_progress_max = new_max
        self.downloadProgressMaxChanged.emit()

    @Property(bool, notify=downloadingChanged)
    def downloading(self):
        return self._downloading

    @Property(int, notify=downloadProgressChanged)
    def download_progress(self):
        if self._download_progress is None:
            return 0

        return self._download_progress

    @Property(str, notify=downloadStatusChanged)
    def download_status(self):
        if self._download_status is None:
            return ""

        if self._download_status == "Installation complete":
            self._set_downloading(False)
            self.downloadFinished.emit()

        return self._download_status

    @Property(int, notify=downloadProgressMaxChanged)
    def download_progress_max(self):
        if self._download_progress_max is None:
            return 0

        return self._download_progress_max

    @Slot()
    def download(self, version, directory):
        print(f"Downloading {version} in {directory}...")
        self._set_downloading(True)

        thread = threading.Thread(target=self._download, args=(version, directory))
        thread.start()

downloader = Downloader()
