//
// Created by jay on 23/08/2026.
//

#include "Profiles.h"
#include "ProfilesTable.h"

ProfilesTable::ProfilesTable(QObject* parent)
  : QAbstractTableModel(parent) {

  connect(ProfileManager::getInstance(), &ProfileManager::profileUpdated,
    this, [this](const QString &uuid) {
      int row = m_uuidList.indexOf(uuid);
        if (row != -1) {
          auto topLeft {index(row, 0)};
          auto bottomRight {index(row, columnCount() - 1)};
          emit dataChanged(topLeft, bottomRight);
      }
    }
  );

  refresh();
}

int ProfilesTable::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return static_cast<int>(m_uuidList.size());
}

int ProfilesTable::columnCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  // Name, Version
  return 2;
}

void ProfilesTable::refresh() {
  beginResetModel();
  m_uuidList = ProfileManager::getProfiles().keys();

  std::ranges::sort(m_uuidList, [](const QVariant& a, const QVariant& b) {
    const auto PROFILE_A {ProfileManager::getProfile(a.toString())};
    const auto PROFILE_B {ProfileManager::getProfile(b.toString())};

    return PROFILE_A->getCreated() < PROFILE_B->getCreated();
  });

  endResetModel();
}

QVariant ProfilesTable::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
    return {};
  }

  const QString UUID = m_uuidList.at(index.row());

  if (role == ProfileRole::UUID_ROLE) {
    return UUID;
  }

  if (role == Qt::DisplayRole) {
    auto profile {ProfileManager::getProfile(UUID)};

    switch (index.column()) {
      case 0: return profile->getName();
      case 1: return profile->getLastVersionId();
    }
  }
  return {};
}

QVariant ProfilesTable::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (orientation == Qt::Horizontal and role == Qt::DisplayRole) {
    switch (section) {
      case 0: return "Version name";
      case 1: return "Version";
      default:;
    }
  }

  return {};
}

QHash<int, QByteArray> ProfilesTable::roleNames() const {
  QHash<int, QByteArray> roles = QAbstractTableModel::roleNames();
  roles[ProfileRole::UUID_ROLE] = "uuid";
  return roles;
}