/**
 * @file ParameterTableModel.cpp
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "ParameterTableModel.h"
#include <QColor>
#include "util.h"
#include "EtcCsPersEditBridge.h"
#include "Settings.h"

namespace csprofileeditor {

ParameterTableModel::ParameterTableModel(csprofile::Personality &personality, QObject *parent) :
    QAbstractTableModel(parent), personality_(personality) {
}

QStringList ParameterTableModel::GetAllowedNames(const QModelIndex &index) const {
  QStringList names;
  const auto &parameter = personality_.parameters_.at(index.row());
  for (const auto &name : parameter->GetAllowedNames()) {
    names.push_back(QString::fromStdString(name));
  }
  return names;
}

const std::unique_ptr<csprofile::parameter::Parameter> &ParameterTableModel::GetParameter(const QModelIndex &index) const {
  return personality_.parameters_.at(index.row());
}

bool ParameterTableModel::SetRanges(const QModelIndex &index, const std::vector<csprofile::parameter::Range> &ranges) {
  auto &parameter = personality_.parameters_.at(index.row());
  parameter->ranges_ = ranges;
  const auto ranges_index = createIndex(index.row(), static_cast<int>(Column::kRanges));
  Q_EMIT(dataChanged(ranges_index, ranges_index));
  return true;
}

int ParameterTableModel::rowCount(const QModelIndex &parent) const {
  return personality_.parameters_.size();
}

int ParameterTableModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant ParameterTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
    return {};
  }

  const auto column = static_cast<Column>(section);
  switch (column) {
    case Column::kType:return tr("Type");
    case Column::kName:return tr("Name");
    case Column::kCoarse:return tr("Coarse");
    case Column::kFine:return tr("Fine");
    case Column::kHome:return tr("Home");
    case Column::kInvert:return tr("Invert");
    case Column::kRanges:return tr("Ranges");
  }
  return {};
}

QVariant ParameterTableModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const auto &parameter = personality_.parameters_.at(index.row());

  if (role == Qt::ItemDataRole::DisplayRole || role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kType) {
      if (role == Qt::ItemDataRole::DisplayRole) {
        return EtcCsPersEditBridge::GetParameterTypeName(parameter->GetType());
      } else {
        return static_cast<unsigned int>(parameter->GetType());
      }
    } else if (column == Column::kName) {
      return QString::fromStdString(parameter->GetName());
    } else if (column == Column::kCoarse) {
      return parameter->GetAddressCourse();
    } else if (column == Column::kFine) {
      return parameter->Is16Bit() ? parameter->GetAddressFine() : QVariant();
    } else if (column == Column::kHome) {
      return parameter->GetHomeValue();
    } else if (column == Column::kRanges && role == Qt::ItemDataRole::DisplayRole) {
      return parameter->ranges_.empty() ? QVariant() : static_cast<unsigned int>(parameter->ranges_.size());
    }
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    if (column == Column::kInvert) {
      return parameter->GetInvert() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
    }
  } else if (role == Qt::ItemDataRole::DecorationRole) {
    if (column == Column::kName) {
      const auto &color = parameter->GetColor();
      return color.has_value() ? QColor(color.value()) : QVariant();
    }
  } else if (role == Qt::ForegroundRole || role == Qt::ToolTipRole) {
    const auto invalid_reason = parameter->IsInvalid();
    if (invalid_reason != csprofile::parameter::Parameter::InvalidReason::kIsValid) {
      if (role == Qt::ForegroundRole) {
        return QBrush(Settings::GetErrorColor());
      } else {
        switch (invalid_reason) {
          case csprofile::parameter::Parameter::InvalidReason::kMissingName:return tr("Missing label.");
          case csprofile::parameter::Parameter::InvalidReason::kHomeOutOfRange:return tr("Home is outside of allowed values.");
          case csprofile::parameter::Parameter::InvalidReason::kOutOfDmxRange:return tr("Address is not acceptable DMX.");
          case csprofile::parameter::Parameter::InvalidReason::kOverlappingAddresses:return tr("Coarse and fine addresses overlap.");
          case csprofile::parameter::Parameter::InvalidReason::kInvalidRange:return tr("A range is invalid.");
          case csprofile::parameter::Parameter::InvalidReason::kRangeOutOfRange:return tr("A range's value is out of the possible values.");
          case csprofile::parameter::Parameter::InvalidReason::kIsValid:break;
        }
      }
    }
  }

  return {};
}

bool ParameterTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());
  auto &parameter = personality_.parameters_.at(index.row());

  bool success = false;
  // Sometimes changing one field causes changes in others.
  QModelIndex changedTopLeft = index;
  QModelIndex changedBottomRight = index;
  if (role == Qt::ItemDataRole::EditRole) {
    if (column == Column::kType) {
      const auto new_type = static_cast<csprofile::parameter::Type>(value.toUInt());
      parameter = parameter->ConvertTo(new_type);
      changedTopLeft = createIndex(changedTopLeft.row(), 0);
      changedBottomRight = createIndex(changedBottomRight.row(), kColumnCount - 1);
      success = true;
    } else if (column == Column::kName) {
      success = parameter->IsAllowedName(value.toString().toStdString());
      if (success) {
        parameter->SetName(value.toString().toStdString());
      }
    } else if (column == Column::kCoarse) {
      const unsigned int address = value.toUInt(&success);
      if (success) {
        parameter->SetAddressCourse(address);
      }
    } else if (column == Column::kFine) {
      const unsigned int address = value.toUInt(&success);
      if (success) {
        parameter->SetAddressFine(address);
      }
    } else if (column == Column::kHome) {
      const unsigned int address = value.toUInt(&success);
      if (success) {
        parameter->SetHomeValue(address);
      }
    }
  } else if (role == Qt::ItemDataRole::CheckStateRole) {
    const bool checked = value.value<Qt::CheckState>() == Qt::CheckState::Checked;
    if (column == Column::kInvert) {
      parameter->SetInvert(checked);
      success = true;
    }
  }

  if (success) {
    Q_EMIT(dataChanged(changedTopLeft, changedBottomRight));
  }
  return success;
}

Qt::ItemFlags ParameterTableModel::flags(const QModelIndex &index) const {
  const auto column = static_cast<Column>(index.column());
  const Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

  switch (column) {
    case Column::kType:
    case Column::kName:
    case Column::kCoarse:
    case Column::kFine:
    case Column::kHome:
    case Column::kRanges:return flags | Qt::ItemIsEditable;
    case Column::kInvert:return flags | Qt::ItemIsUserCheckable;
  }

  return flags;
}

bool ParameterTableModel::insertRows(int startRow, int count, const QModelIndex &parent) {
  beginInsertRows(parent, startRow, startRow + count - 1);
  personality_.parameters_.reserve(personality_.parameters_.size() + count);
  for (int row = startRow; row < startRow + count; ++row) {
    personality_.parameters_
        .push_back(csprofile::parameter::Parameter::CreateForType(csprofile::parameter::Type::kNone));
  }
  endInsertRows();

  return true;
}

bool ParameterTableModel::removeRows(int startRow, int count, const QModelIndex &parent) {
  if (count == 0) {
    return false;
  }

  const int endRow = startRow + count - 1;
  beginRemoveRows(parent, startRow, endRow);
  personality_.parameters_
      .erase(personality_.parameters_.begin() + startRow, personality_.parameters_.begin() + (endRow + 1));
  endRemoveRows();

  return true;
}

bool ParameterTableModel::removeRows(std::vector<int> rows, const QModelIndex &parent) {
  return util::remove_model_rows(std::move(rows), parent, this);
}

} // csprofileeditor
