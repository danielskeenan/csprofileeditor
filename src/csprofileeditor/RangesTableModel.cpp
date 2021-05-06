/**
 * @file RangesTableModel.cpp
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "RangesTableModel.h"
#include <QColor>
#include "EtcCsPersEditBridge.h"
#include "util.h"
#include <QIcon>
#include "Settings.h"
#include <QBrush>

namespace csprofileeditor {

RangesTableModel::RangesTableModel(std::unique_ptr<csprofile::parameter::Parameter> &parameter, QObject *parent) :
    QAbstractTableModel(parent),
    parameter_(parameter),
    disc_db_(EtcCsPersEditBridge::GetDiscDb()),
    effect_db_(EtcCsPersEditBridge::GetEffectDb()),
    gobo_db_(EtcCsPersEditBridge::GetGoboDb()) {
  UpdateImageCache();
}

int RangesTableModel::rowCount(const QModelIndex &parent) const {
  return parameter_->ranges_.size();
}

int RangesTableModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant RangesTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal) {
    return {};
  }

  const auto column = static_cast<Column>(section);
  if (role == Qt::DisplayRole) {
    switch (column) {
      case Column::kBegin:return tr("Begin");
      case Column::kEnd:return tr("End");
      case Column::kDefault:return tr("Default");
      case Column::kLabel:return tr("Label");
      case Column::kMedia:return tr("Gel/Gobo");
    }
  }

  return {};
}

QVariant RangesTableModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const auto &range = parameter_->ranges_.at(index.row());

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (column == Column::kBegin) {
      return range.GetBeginValue();
    } else if (column == Column::kEnd) {
      return range.GetEndValue();
    } else if (column == Column::kDefault) {
      return range.GetDefaultValue();
    } else if (column == Column::kLabel) {
      return QString::fromStdString(range.GetLabel());
    } else if (column == Column::kMedia) {
      if (role == Qt::DisplayRole) {
        return range.GetMedia().has_value() ? QString::fromStdString(range.GetMedia()->GetName()) : QVariant();
      }
    }
  } else if (role == Qt::DecorationRole) {
    if (column == Column::kMedia) {
      const auto &media = range.GetMedia();
      if (media.has_value()) {
        const auto &color = media->GetRgb();
        const auto &gobo_dcid = media->GetGoboDcid();
        if (color.has_value()) {
          return QColor(color.value());
        } else if (gobo_dcid.has_value() && dcid_images_.find(*gobo_dcid) != dcid_images_.end()) {
          return dcid_images_.at(*gobo_dcid);
        }
      }
    }
  } else if (role == Qt::ForegroundRole || role == Qt::ToolTipRole) {
    const auto invalid_reason = range.IsInvalid();
    if (invalid_reason != csprofile::parameter::Range::InvalidReason::kIsValid) {
      if (role == Qt::ForegroundRole) {
        return QBrush(Settings::GetErrorColor());
      } else {
        switch (invalid_reason) {
          case csprofile::parameter::Range::InvalidReason::kEndBeforeBegin:return tr("Range ends before it begins.");
          case csprofile::parameter::Range::InvalidReason::kDefaultOutOfRange:return tr("Default is not inside range.");
          case csprofile::parameter::Range::InvalidReason::kOutOfDmxRange:return tr("Value is not acceptable DMX.");
          case csprofile::parameter::Range::InvalidReason::kMissingLabel:return tr("Missing label.");
          case csprofile::parameter::Range::InvalidReason::kIsValid:break;
        }
      }
    }
  }

  return {};
}

bool RangesTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());
  auto &range = parameter_->ranges_.at(index.row());

  bool success = false;
  if (role == Qt::EditRole) {
    if (column == Column::kBegin) {
      const auto int_val = value.toUInt(&success);
      if (success) {
        range.SetBeginValue(int_val);
      }
    } else if (column == Column::kEnd) {
      const auto int_val = value.toUInt(&success);
      if (success) {
        range.SetEndValue(int_val);
      }
    } else if (column == Column::kDefault) {
      const auto int_val = value.toUInt(&success);
      if (success) {
        range.SetDefaultValue(int_val);
      }
    } else if (column == Column::kLabel) {
      range.SetLabel(value.toString().toStdString());
      success = true;
    }
  }

  if (success) {
    Q_EMIT(dataChanged(index, index));
  }

  return success;
}

bool RangesTableModel::SetMedia(const QModelIndex &index, const std::optional<csprofile::parameter::Media> &media) {
  auto &range = parameter_->ranges_.at(index.row());
  range.SetMedia(media);
  UpdateImageCache();
  const QModelIndex media_index = createIndex(index.row(), static_cast<int>(Column::kMedia));
  Q_EMIT(dataChanged(media_index, media_index));
  return true;
}

Qt::ItemFlags RangesTableModel::flags(const QModelIndex &index) const {
  const auto column = static_cast<Column>(index.column());
  const auto flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;

  switch (column) {
    case Column::kBegin:
    case Column::kEnd:
    case Column::kDefault:
    case Column::kLabel:
    case Column::kMedia:return flags | Qt::ItemIsEditable;
  }

  return flags;
}

bool RangesTableModel::insertRows(int startRow, int count, const QModelIndex &parent) {
  beginInsertRows(parent, startRow, startRow + count - 1);
  parameter_->ranges_.reserve(parameter_->ranges_.size() + count);
  for (int row = startRow; row < startRow + count; ++row) {
    const unsigned int next_val = parameter_->ranges_.empty() ? 0 : (parameter_->ranges_.back().GetEndValue() + 1);
    parameter_->ranges_.emplace_back(next_val, next_val, next_val);
  }
  endInsertRows();

  return true;
}

bool RangesTableModel::removeRows(int startRow, int count, const QModelIndex &parent) {
  if (count == 0) {
    return false;
  }

  const int endRow = startRow + count - 1;
  beginRemoveRows(parent, startRow, endRow);
  parameter_->ranges_.erase(parameter_->ranges_.begin() + startRow, parameter_->ranges_.begin() + (endRow + 1));
  endRemoveRows();

  return true;
}

bool RangesTableModel::removeRows(std::vector<int> rows, const QModelIndex &parent) {
  return util::remove_model_rows(std::move(rows), parent, this);
}

std::optional<QIcon> RangesTableModel::GetImageForDcid(const std::string &dcid) {
  std::optional<std::vector<char>> image;
  if ((disc_db_ && (image = disc_db_->GetImageForDcid(dcid)))
      || (effect_db_ && (image = effect_db_->GetImageForDcid(dcid)))
      || (gobo_db_ && (image = gobo_db_->GetImageForDcid(dcid)))) {
    QPixmap pixmap;
    if (pixmap.loadFromData(reinterpret_cast<const unsigned char *>(image->data()), image->size())) {
      return QIcon(pixmap);
    }
  }
  return {};
}

void RangesTableModel::UpdateImageCache() {
  // Remove unused images
  std::unordered_set<std::string> used_dcids;
  for (const auto &range : parameter_->ranges_) {
    if (range.GetMedia().has_value() && range.GetMedia()->GetGoboDcid().has_value()) {
      used_dcids.insert(range.GetMedia()->GetGoboDcid().value());
    }
  }
  for (auto dcid_image = dcid_images_.begin(); dcid_image != dcid_images_.end();) {
    if (used_dcids.find(dcid_image->first) == used_dcids.end()) {
      dcid_images_.erase(dcid_image++);
    } else {
      ++dcid_image;
    }
  }

  // Add new images
  for (const auto &dcid : used_dcids) {
    if (dcid_images_.find(dcid) == dcid_images_.end()) {
      if (const std::optional<QIcon> icon = GetImageForDcid(dcid)) {
        dcid_images_.insert({dcid, *icon});
      }
    }
  }
}

} // csprofileeditor
