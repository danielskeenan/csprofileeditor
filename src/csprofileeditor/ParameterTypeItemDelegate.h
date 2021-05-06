/**
 * @file ParameterTypeItemDelegate.h
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTYPEITEMDELEGATE_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTYPEITEMDELEGATE_H_

#include <QStyledItemDelegate>

namespace csprofileeditor {

/**
 * Provide a combo box to select parameter types
 */
class ParameterTypeItemDelegate : public QStyledItemDelegate {
 Q_OBJECT
 public:
  explicit ParameterTypeItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const final;
  void setEditorData(QWidget *editor, const QModelIndex &index) const final;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const final;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTYPEITEMDELEGATE_H_
