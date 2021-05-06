/**
 * @file PersonalityEditDialog.h
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYEDITDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYEDITDIALOG_H_

#include <QDialog>
#include <csprofile/Personality.h>
#include <QTableView>
#include "ParameterTableModel.h"

namespace csprofileeditor {

/**
 * Edit personality
 */
class PersonalityEditDialog : public QDialog {
 Q_OBJECT
 public:
  explicit PersonalityEditDialog(const csprofile::Personality &personality, QWidget *parent = nullptr);

  [[nodiscard]] const csprofile::Personality &GetPersonality() const {
    return personality_;
  }

 private:
  struct Actions {
    QAction *act_add_parameter = nullptr;
    QAction *act_remove_parameter = nullptr;
    QAction* act_edit_ranges = nullptr;
  };
  Actions actions_;
  struct Widgets {
    QTableView *parameter_table = nullptr;
  };
  Widgets widgets_;

  csprofile::Personality personality_;
  ParameterTableModel *parameter_table_model_ = nullptr;

  void InitActions();
  void InitUi();
  [[nodiscard]] bool ParameterActionsAllowed() const;
  void EditRanges(const QModelIndex &index);

 private Q_SLOTS:
  // Actions
  void SAddParameter();
  void SRemoveParameter();
  void SEditRanges();

  void SSelectedTableRowChanged() const;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYEDITDIALOG_H_
