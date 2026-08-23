//
// Created by jay on 23/08/2026.
//

#include "Profiles.h"
#include "ProfilesTable.h"

ProfilesTable::ProfilesTable(QObject* parent)
  : QAbstractTableModel(parent) {

  connect(Profiles::getInstance(), &Profiles::profilesChanged,
    this, &ProfilesTable::refresh);

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
  m_uuidList = Profiles::getProfiles().keys();
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
    const QVariantMap profile {Profiles::getProfile(UUID)};

    switch (index.column()) {
      case 0: return profile["name"].toString();
      case 1: return profile["lastVersionId"].toString();
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
