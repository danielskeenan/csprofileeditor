/**
 * @file RangesEditDialog.cpp
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "RangesEditDialog.h"
#include "MediaSelectorDialog.h"
#include <QAction>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QMenu>
#include "EtcCsPersEditBridge.h"
#include "PushButtonItemDelegate.h"

namespace csprofileeditor {

RangesEditDialog::RangesEditDialog(const std::unique_ptr<csprofile::parameter::Parameter> &parameter, QWidget *parent) :
    QDialog(parent), parameter_(parameter->Clone()), ranges_table_model_(new RangesTableModel(parameter_, this)) {
  setWindowTitle(QString::fromStdString(parameter->GetName()));

  InitActions();
  InitUi();
}

void RangesEditDialog::InitActions() {
  // Add range
  actions_.act_range_add = new QAction(QIcon::fromTheme("list-add"), tr("Add Range"), this);
  connect(actions_.act_range_add, &QAction::triggered, this, &RangesEditDialog::SAddRange);

  // Remove range
  actions_.act_range_remove = new QAction(QIcon::fromTheme("list-remove"), tr("Remove Range"), this);
  actions_.act_range_remove->setEnabled(RangeActionsAllowed());
  connect(actions_.act_range_remove, &QAction::triggered, this, &RangesEditDialog::SRemoveRange);
}

void RangesEditDialog::InitUi() {
  resize(800, 600);
  auto *layout = new QVBoxLayout(this);

  // Table
  widgets_.range_table = new QTableView(this);
  widgets_.range_table->setModel(ranges_table_model_);
  widgets_.range_table->setContextMenuPolicy(Qt::ActionsContextMenu);
  widgets_.range_table->addAction(actions_.act_range_add);
  widgets_.range_table->addAction(actions_.act_range_remove);
  widgets_.range_table->resizeColumnsToContents();
  QPushButton *media_button = CreateRangeMediaButton();
  auto *media_delegate = new PushButtonItemDelegate(media_button, this);
  widgets_.range_table->setItemDelegateForColumn(static_cast<int>(RangesTableModel::Column::kMedia), media_delegate);
  connect(widgets_.range_table->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &RangesEditDialog::SSelectedTableRowChanged);
  layout->addWidget(widgets_.range_table);

  // Actions
  auto *actions = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout->addWidget(actions);
  connect(actions, &QDialogButtonBox::accepted, this, &RangesEditDialog::accept);
  connect(actions, &QDialogButtonBox::rejected, this, &RangesEditDialog::reject);
}

bool RangesEditDialog::RangeActionsAllowed() const {
  return widgets_.range_table != nullptr && widgets_.range_table->selectionModel()->hasSelection();
}

void RangesEditDialog::SAddRange() {
  ranges_table_model_->insertRows(ranges_table_model_->rowCount(QModelIndex()), 1, QModelIndex());
}

void RangesEditDialog::SRemoveRange() {
  if (!RangeActionsAllowed()) {
    return;
  }

  std::unordered_set<int> rows;
  for (const auto &row : widgets_.range_table->selectionModel()->selectedIndexes()) {
    rows.insert(row.row());
  }
  if (rows.empty()) {
    return;
  }
  QString delete_message;
  if (rows.size() == 1) {
    const int row = *rows.cbegin();
    const QString &parameter_name = ranges_table_model_->data(
        ranges_table_model_->index(row, static_cast<int>(RangesTableModel::Column::kLabel)),
        Qt::DisplayRole).toString();
    if (!parameter_name.isEmpty()) {
      delete_message = tr("Are you sure you want to delete %1?").arg(parameter_name);
    } else {
      delete_message = tr("Are you sure you want to delete this range?");
    }
  } else {
    delete_message = tr("Are you sure you want to delete %n ranges?", "", rows.size());
  }
  const auto really_delete = QMessageBox::question(this, tr("Delete ranges", "", rows.size()), delete_message);
  if (really_delete == QMessageBox::Yes) {
    ranges_table_model_->removeRows(std::vector<int>(rows.cbegin(), rows.cend()), QModelIndex());
  }
}

void RangesEditDialog::SSelectedTableRowChanged() const {
  const bool range_actions_allowed = RangeActionsAllowed();
  actions_.act_range_remove->setEnabled(range_actions_allowed);
}

QPushButton *RangesEditDialog::CreateRangeMediaButton() {
  auto *button = new QPushButton(tr("Choose"), this);
  auto *menu = new QMenu(this);
  button->setMenu(menu);

  // Disc
  auto *disc_action = new QAction(tr("Disc"), this);
  connect(disc_action, &QAction::triggered, [this, button]() {
    ShowMediaSelector<ImageMediaSelectorDialog>(
        button->property(PushButtonItemDelegate::kModelIndexProperty).toModelIndex(),
        EtcCsPersEditBridge::GetDiscDb());
  });
  menu->addAction(disc_action);

  // Effect
  auto *effect_action = new QAction(tr("Effect"), this);
  connect(effect_action, &QAction::triggered, [this, button]() {
    ShowMediaSelector<ImageMediaSelectorDialog>(
        button->property(PushButtonItemDelegate::kModelIndexProperty).toModelIndex(),
        EtcCsPersEditBridge::GetEffectDb());
  });
  menu->addAction(effect_action);

  // Gel
  auto *gel_action = new QAction(tr("Gel"), this);
  connect(gel_action, &QAction::triggered, [this, button]() {
    ShowMediaSelector<GelSelectorDialog>(
        button->property(PushButtonItemDelegate::kModelIndexProperty).toModelIndex(),
        EtcCsPersEditBridge::GetGelDb());
  });
  menu->addAction(gel_action);

  // Gobo
  auto *gobo_action = new QAction(tr("Gobo"), this);
  connect(gobo_action, &QAction::triggered, [this, button]() {
    ShowMediaSelector<ImageMediaSelectorDialog>(
        button->property(PushButtonItemDelegate::kModelIndexProperty).toModelIndex(),
        EtcCsPersEditBridge::GetGoboDb());
  });
  menu->addAction(gobo_action);

  return button;
}
} // csprofileeditor
