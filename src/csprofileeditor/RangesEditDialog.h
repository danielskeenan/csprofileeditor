/**
 * @file RangesEditDialog.h
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESEDITDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESEDITDIALOG_H_

#include <QDialog>
#include <csprofile/parameter/Parameter.h>
#include <QTableView>
#include <QStyledItemDelegate>
#include "RangesTableModel.h"
#include "MediaSelectorDialog.h"

namespace csprofileeditor {

/**
 * Edit parameter ranges dialog
 */
class RangesEditDialog : public QDialog {
 Q_OBJECT
 public:
  explicit RangesEditDialog(const std::unique_ptr<csprofile::parameter::Parameter> &parameter,
                            QWidget *parent = nullptr);

  [[nodiscard]] const std::unique_ptr<csprofile::parameter::Parameter> &GetParameter() const {
    return parameter_;
  }

 private:
  struct Actions {
    QAction *act_range_add = nullptr;
    QAction *act_range_remove = nullptr;
  };
  Actions actions_;
  struct Widgets {
    QTableView *range_table = nullptr;
  };
  Widgets widgets_;
  std::unique_ptr<csprofile::parameter::Parameter> parameter_;
  RangesTableModel *ranges_table_model_;

  void InitActions();
  void InitUi();
  [[nodiscard]] bool RangeActionsAllowed() const;
  [[nodiscard]] QPushButton *CreateRangeMediaButton();

  template<class Dialog_T, class Db_T,
      typename = std::enable_if_t<std::is_base_of_v<MediaSelectorDialog, Dialog_T>>,
      typename = std::enable_if_t<std::is_base_of_v<cslibs::Db, Db_T>>>
  void ShowMediaSelector(const QModelIndex &index, std::shared_ptr<Db_T> db) {
    auto *dialog = new Dialog_T(db, this);
    dialog->Init();
    if (dialog->exec() == Dialog_T::Accepted) {
      ranges_table_model_->SetMedia(index, dialog->GetMedia());
    }
  }

 private Q_SLOTS:
  void SAddRange();
  void SRemoveRange();

  void SSelectedTableRowChanged() const;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESEDITDIALOG_H_
