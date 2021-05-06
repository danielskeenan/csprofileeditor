/**
 * @file AllowedNamesItemDelegate.cpp
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "AllowedNamesItemDelegate.h"
#include "ParameterTableModel.h"
#include <QComboBox>

namespace csprofileeditor {

QWidget *AllowedNamesItemDelegate::createEditor(QWidget *parent,
                                                const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const {
  const auto *model = dynamic_cast<const ParameterTableModel *>(index.model());
  const QStringList &allowed_names = model->GetAllowedNames(index);
  if (allowed_names.empty()) {
    // No specific allowed names; use default editor.
    return QStyledItemDelegate::createEditor(parent, option, index);
  }
  auto *combo_box = new QComboBox(parent);
  assert(model != nullptr);
  // One entry per parameter
  for (const auto &name : allowed_names) {
    combo_box->addItem(name);
  }

  return combo_box;
}

void AllowedNamesItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  auto *combo_box = dynamic_cast<QComboBox *>(editor);
  if (combo_box == nullptr) {
    QStyledItemDelegate::setEditorData(editor, index);
    return;
  }
  combo_box->setCurrentText(index.data().toString());
}

void AllowedNamesItemDelegate::setModelData(QWidget *editor,
                                            QAbstractItemModel *model,
                                            const QModelIndex &index) const {
  auto *combo_box = dynamic_cast<QComboBox *>(editor);
  if (combo_box == nullptr) {
    QStyledItemDelegate::setModelData(editor, model, index);
    return;
  }
  model->setData(index, combo_box->currentText());
}

} //csprofileeditor
