/**
 * @file PushButtonItemDelegate.cpp
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "PushButtonItemDelegate.h"
#include <QPushButton>

namespace csprofileeditor {

PushButtonItemDelegate::PushButtonItemDelegate(QPushButton *button, QObject *parent) :
    QStyledItemDelegate(parent), button_(button) {
  button_->setVisible(false);
}

QWidget *PushButtonItemDelegate::createEditor(QWidget *parent,
                                              const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const {
  button_->setParent(parent);
  return button_;
}

void PushButtonItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  editor->setProperty(kModelIndexProperty, index);
}

void PushButtonItemDelegate::updateEditorGeometry(QWidget *editor,
                                                  const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const {
  editor->setVisible(true);
  QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void PushButtonItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  // Do nothing
}

void PushButtonItemDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const {
  // Because the editor is re-used on each edit event, it must not be destroyed.
  editor->setVisible(false);
}

} // csprofileeditor
