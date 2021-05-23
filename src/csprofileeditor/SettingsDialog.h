/**
 * @file SettingsDialog.h
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_SETTINGSDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_SETTINGSDIALOG_H_

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include "csprofileeditor_config.h"

namespace csprofileeditor {

/**
 * Settings dialog
 */
class SettingsDialog : public QDialog {
 Q_OBJECT
 public:
  explicit SettingsDialog(QWidget *parent = nullptr);
  void accept() final;

 private:
  struct Widgets {
    QLineEdit *etcCsPersEditPath = nullptr;
    QDialogButtonBox *actions = nullptr;
  };
  Widgets widgets_;

  void InitUi();
  [[nodiscard]] bool AllowOk() const;

 private Q_SLOTS:
  void SEtcPersEditPathChanged() const;
  void SChooseEtcPersEditPath();
#ifdef APP_ETC_EDITOR_DETECTABLE_PLATFORM
  void SAutodetectEtcPersEditPath();
#endif
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_SETTINGSDIALOG_H_
