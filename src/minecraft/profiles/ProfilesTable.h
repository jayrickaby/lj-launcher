//
// Created by jay on 23/08/2026.
//

#ifndef LJ_LAUNCHER_PROFILESTABLE_H_
#define LJ_LAUNCHER_PROFILESTABLE_H_

#include <qqml.h>
#include <QAbstractItemModel>

class ProfilesTable : public QAbstractTableModel {
  Q_OBJECT;

public:
  enum ProfileRole {
    UUID_ROLE = Qt::UserRole,
  };
  Q_ENUM(ProfileRole)

  explicit ProfilesTable(QObject* parent = nullptr);

  void refresh();

  int rowCount(const QModelIndex& parent=QModelIndex()) const override;
  int columnCount(const QModelIndex& parent=QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const override;

private:
  QStringList m_uuidList;
};

#endif  // LJ_LAUNCHER_PROFILESTABLE_H_
