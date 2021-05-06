/**
 * @file PersonalityEditDialog.cpp
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "PersonalityEditDialog.h"
#include "ParameterTypeItemDelegate.h"
#include "AllowedNamesItemDelegate.h"
#include "PushButtonItemDelegate.h"
#include "RangesEditDialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QAction>
#include <QMenu>
#include <QMessageBox>

namespace csprofileeditor {

PersonalityEditDialog::PersonalityEditDialog(const csprofile::Personality &personality, QWidget *parent) :
    QDialog(parent), personality_(personality) {
  QStringList title_parts;
  title_parts.reserve(3);
  if (!personality.GetManufacturerName().empty()) {
    title_parts.push_back(QString::fromStdString(personality.GetManufacturerName()));
  }
  if (!personality.GetModelName().empty()) {
    title_parts.push_back(QString::fromStdString(personality.GetModelName()));
  }
  if (!personality.GetModeName().empty()) {
    title_parts.push_back(QString::fromStdString(personality.GetModeName()));
  }
  setWindowTitle(title_parts.join(" - "));

  InitActions();
  InitUi();
}

void PersonalityEditDialog::InitActions() {
  // Add parameter
  actions_.act_add_parameter = new QAction(QIcon::fromTheme("list-add"), tr("Add Parameter"), this);
  connect(actions_.act_add_parameter, &QAction::triggered, this, &PersonalityEditDialog::SAddParameter);

  // Remove parameter
  actions_.act_remove_parameter = new QAction(QIcon::fromTheme("list-remove"), tr("Remove Parameter"), this);
  actions_.act_remove_parameter->setEnabled(ParameterActionsAllowed());
  connect(actions_.act_remove_parameter, &QAction::triggered, this, &PersonalityEditDialog::SRemoveParameter);

  // Edit ranges
  actions_.act_edit_ranges = new QAction(QIcon::fromTheme("document-edit"), tr("Edit Ranges..."), this);
  actions_.act_edit_ranges->setEnabled(ParameterActionsAllowed());
  connect(actions_.act_edit_ranges, &QAction::triggered, this, &PersonalityEditDialog::SEditRanges);
}

void PersonalityEditDialog::InitUi() {
  resize(800, 600);
  auto *layout = new QVBoxLayout(this);

  // Table
  widgets_.parameter_table = new QTableView(this);
  parameter_table_model_ = new ParameterTableModel(personality_, this);
  widgets_.parameter_table->setModel(parameter_table_model_);
  widgets_.parameter_table->setContextMenuPolicy(Qt::ActionsContextMenu);
  widgets_.parameter_table->addAction(actions_.act_add_parameter);
  widgets_.parameter_table->addAction(actions_.act_remove_parameter);
  widgets_.parameter_table->addAction(actions_.act_edit_ranges);
  widgets_.parameter_table->resizeColumnsToContents();
  auto *parameter_type_delegate = new ParameterTypeItemDelegate(this);
  widgets_.parameter_table->setItemDelegateForColumn(static_cast<int>(ParameterTableModel::Column::kType),
                                                     parameter_type_delegate);
  auto *parameter_name_delegate = new AllowedNamesItemDelegate(this);
  widgets_.parameter_table->setItemDelegateForColumn(static_cast<int>(ParameterTableModel::Column::kName),
                                                     parameter_name_delegate);
  auto *edit_ranges_button = new QPushButton(tr("Edit..."), this);
  connect(edit_ranges_button, &QPushButton::clicked, [this, edit_ranges_button]() {
    EditRanges(edit_ranges_button->property(PushButtonItemDelegate::kModelIndexProperty).toModelIndex());
  });
  auto *ranges_delegate = new PushButtonItemDelegate(edit_ranges_button, this);
  widgets_.parameter_table->setItemDelegateForColumn(static_cast<int>(ParameterTableModel::Column::kRanges),
                                                     ranges_delegate);
  connect(widgets_.parameter_table->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &PersonalityEditDialog::SSelectedTableRowChanged);
  layout->addWidget(widgets_.parameter_table);

  // Actions
  auto *actions = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout->addWidget(actions);
  connect(actions, &QDialogButtonBox::accepted, this, &PersonalityEditDialog::accept);
  connect(actions, &QDialogButtonBox::rejected, this, &PersonalityEditDialog::reject);
}

bool PersonalityEditDialog::ParameterActionsAllowed() const {
  return widgets_.parameter_table != nullptr && widgets_.parameter_table->selectionModel()->hasSelection();
}

void PersonalityEditDialog::EditRanges(const QModelIndex &index) {
  const auto &parameter = parameter_table_model_->GetParameter(index);
  RangesEditDialog dialog(parameter, this);
  if (dialog.exec() == RangesEditDialog::Accepted) {
    parameter_table_model_->SetRanges(index, dialog.GetParameter()->ranges_);
  }
}

void PersonalityEditDialog::SAddParameter() {
  parameter_table_model_->insertRows(parameter_table_model_->rowCount(QModelIndex()), 1, QModelIndex());
}

void PersonalityEditDialog::SRemoveParameter() {
  if (!ParameterActionsAllowed()) {
    return;
  }

  std::unordered_set<int> rows;
  for (const auto &row : widgets_.parameter_table->selectionModel()->selectedIndexes()) {
    rows.insert(row.row());
  }
  if (rows.empty()) {
    return;
  }
  QString delete_message;
  if (rows.size() == 1) {
    const int row = *rows.cbegin();
    const QString &parameter_name = parameter_table_model_->data(
        parameter_table_model_->index(row, static_cast<int>(ParameterTableModel::Column::kName)),
        Qt::DisplayRole).toString();
    if (!parameter_name.isEmpty()) {
      delete_message = tr("Are you sure you want to delete %1?").arg(parameter_name);
    } else {
      delete_message = tr("Are you sure you want to delete this parameter?");
    }
  } else {
    delete_message = tr("Are you sure you want to delete %n parameters?", "", rows.size());
  }
  const auto really_delete = QMessageBox::question(this, tr("Delete parameters", "", rows.size()), delete_message);
  if (really_delete == QMessageBox::Yes) {
    parameter_table_model_->removeRows(std::vector<int>(rows.cbegin(), rows.cend()), QModelIndex());
  }
}

void PersonalityEditDialog::SEditRanges() {
  if (!ParameterActionsAllowed()) {
    return;
  }
  const auto selected = widgets_.parameter_table->selectionModel()->selectedIndexes().first();
  EditRanges(selected);
}

void PersonalityEditDialog::SSelectedTableRowChanged() const {
  const bool parameter_actions_allowed = ParameterActionsAllowed();
  actions_.act_remove_parameter->setEnabled(parameter_actions_allowed);
  actions_.act_edit_ranges->setEnabled(parameter_actions_allowed);
}

} // csprofileeditor
