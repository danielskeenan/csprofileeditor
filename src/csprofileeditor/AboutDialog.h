/**
 * @file AboutDialog.h
 *
 * @author dankeenan
 * @date 5/7/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ABOUTDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ABOUTDIALOG_H_

#include <QDialog>

namespace csprofileeditor {

/**
 * About Dialog
 */
class AboutDialog : public QDialog {
  Q_OBJECT
 public:
  explicit AboutDialog(QWidget *parent = nullptr);

 private:
  QWidget *CreateAboutTab();
  QWidget *CreateThirdPartyTab();
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ABOUTDIALOG_H_
