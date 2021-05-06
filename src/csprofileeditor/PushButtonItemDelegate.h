/**
 * @file PushButtonItemDelegate.h
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PUSHBUTTONITEMDELEGATE_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PUSHBUTTONITEMDELEGATE_H_

#include <QStyledItemDelegate>
#include <QPushButton>

namespace csprofileeditor {

/**
 * Show a push button as the editor.
 *
 * The button is NOT re-parented upon construction.  The button IS re-parented
 * when shown.  Depending on when/where the button is created, the parent may
 * not actually change.
 *
 * When editing is requested, the button will have a property (kModelIndexProperty)
 * with the QModelIndex.
 */
class PushButtonItemDelegate : public QStyledItemDelegate {
 Q_OBJECT
 public:
  explicit PushButtonItemDelegate(QPushButton *button, QObject *parent = nullptr);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
  void destroyEditor(QWidget *editor, const QModelIndex &index) const override;

  static const inline auto kModelIndexProperty = "model_index";

 private:
  QPushButton *button_;

};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PUSHBUTTONITEMDELEGATE_H_
