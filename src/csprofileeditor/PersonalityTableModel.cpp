/**
 * @file PersonalityTableModel.cpp
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "PersonalityTableModel.h"
#include "util.h"
#include <utility>

namespace csprofileeditor {

PersonalityTableModel::PersonalityTableModel(std::shared_ptr<csprofile::Library> library, QObject *parent) :
    QAbstractTableModel(parent), library_(std::move(library)) {
  assert(library_);
}

void PersonalityTableModel::SetLibrary(std::shared_ptr<csprofile::Library> library) {
  assert(library);
  beginResetModel();
  library_ = std::move(library);
  endResetModel();
}

bool PersonalityTableModel::UpdatePersonality(unsigned int row, const csprofile::Personality &personality) {
  if (row >= library_->personalities.size()) {
    return false;
  }
  library_->personalities[row] = personality;
  Q_EMIT(dataChanged(createIndex(row, 0), createIndex(row, kColumnCount - 1)));
  return true;
}

int PersonalityTableModel::rowCount(const QModelIndex &parent) const {
  return library_->personalities.size();
}

int PersonalityTableModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant PersonalityTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::ItemDataRole::DisplayRole || orientation != Qt::Horizontal) {
    return {};
  }
  const auto column = static_cast<Column>(section);
  switch (column) {
    case Column::kManufacturer:return tr("Manufacturer");
    case Column::kModel:return tr("Model");
    case Column::kMode:return tr("Mode");
    case Column::kFootprint:return tr("Footprint");
  }
  return {};
}

QVariant PersonalityTableModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const csprofile::Personality &personality = library_->personalities.at(index.row());

  if (role == Qt::ItemDataRole::DisplayRole || role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kManufacturer) {
      return QString::fromStdString(personality.GetManufacturerName());
    } else if (column == Column::kModel) {
      return QString::fromStdString(personality.GetModelName());
    } else if (column == Column::kFootprint) {
      return personality.GetFootprint();
    } else if (column == Column::kMode) {
      return QString::fromStdString(personality.GetModeName());
    }
  }

  return {};
}

bool PersonalityTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());
  csprofile::Personality &personality = library_->personalities.at(index.row());

  bool success = false;
  if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kManufacturer) {
      personality.SetManufacturerName(value.toString().toStdString());
      success = true;
    } else if (column == Column::kModel) {
      personality.SetModelName(value.toString().toStdString());
      success = true;
    } else if (column == Column::kMode) {
      personality.SetModeName(value.toString().toStdString());
      success = true;
    }
  }

  if (success) {
    Q_EMIT(dataChanged(index, index));
  }

  return success;
}

Qt::ItemFlags PersonalityTableModel::flags(const QModelIndex &index) const {
  const auto column = static_cast<Column>(index.column());
  const Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

  switch (column) {
    case Column::kManufacturer:
    case Column::kModel:
    case Column::kMode:return flags | Qt::ItemIsEditable;
    case Column::kFootprint:return flags;
  }

  return flags;
}

bool PersonalityTableModel::insertRows(int startRow, int count, const QModelIndex &parent) {
  beginInsertRows(parent, startRow, startRow + count - 1);
  library_->personalities.reserve(library_->personalities.size() + count);
  for (int row = startRow; row < startRow + count; ++row) {
    library_->personalities.emplace(library_->personalities.cbegin() + row);
  }
  endInsertRows();

  return true;
}

bool PersonalityTableModel::removeRows(int startRow, int count, const QModelIndex &parent) {
  if (count == 0) {
    return false;
  }

  const int endRow = startRow + count - 1;
  beginRemoveRows(parent, startRow, endRow);
  library_->personalities
      .erase(library_->personalities.begin() + startRow, library_->personalities.begin() + (endRow + 1));
  endRemoveRows();

  return true;
}

bool PersonalityTableModel::removeRows(std::vector<int> rows, const QModelIndex &parent) {
  return util::remove_model_rows(std::move(rows), parent, this);
}

} // csprofileeditor
