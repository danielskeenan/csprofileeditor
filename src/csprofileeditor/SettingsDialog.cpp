/**
 * @file SettingsDialog.cpp
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "SettingsDialog.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include "Settings.h"
#include "EtcCsPersEditBridge.h"

namespace csprofileeditor {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("Settings"));
  resize(640, 480);
  InitUi();
}

void SettingsDialog::InitUi() {
  auto *layout = new QVBoxLayout(this);
  layout->addStretch();
  auto *form = new QFormLayout;
  layout->addLayout(form);
  layout->addStretch();

  // ETC Pers Edit path
  widgets_.etcCsPersEditPath = new QLineEdit(Settings::GetEtcCsPersEditorPath(), this);
  connect(widgets_.etcCsPersEditPath, &QLineEdit::textChanged, this, &SettingsDialog::SEtcPersEditPathChanged);
  auto *chooseEtcCsPersEditPath = new QPushButton(QIcon::fromTheme("document-open"), tr("Choose"), this);
  connect(chooseEtcCsPersEditPath, &QPushButton::clicked, this, &SettingsDialog::SChooseEtcPersEditPath);
  auto *etcCsPersEditPath = new QHBoxLayout;
  etcCsPersEditPath->addWidget(widgets_.etcCsPersEditPath);
  etcCsPersEditPath->addWidget(chooseEtcCsPersEditPath);
  // Download link
  form->addRow(tr("Path to ETC ColorSource Personality Editor"), etcCsPersEditPath);
  auto *officialEditorDownloadLink =
      new QLabel(QString("<a href=\"https://www.etcconnect.com/Products/Consoles/ColorSource/Software.aspx\">%1</a>")
                     .arg(tr("Download the official editor from ETC")), this);
  officialEditorDownloadLink->setTextFormat(Qt::RichText);
  officialEditorDownloadLink->setOpenExternalLinks(true);
  form->addWidget(officialEditorDownloadLink);

  // Actions
  widgets_.actions = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  widgets_.actions->button(QDialogButtonBox::Ok)->setEnabled(AllowOk());
  connect(widgets_.actions, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
  connect(widgets_.actions, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
  layout->addWidget(widgets_.actions);
}

void SettingsDialog::accept() {
  // Validate path
  if (widgets_.etcCsPersEditPath->text().isEmpty()) {
    QMessageBox::critical(this,
                          tr("Directory required"),
                          tr("An installation directory is required."));
    return;
  }
  const QDir etcCsPersEditPath(widgets_.etcCsPersEditPath->text());
  if (!etcCsPersEditPath.exists()) {
    QMessageBox::critical(this,
                          tr("Non-existent directory"),
                          tr("The directory %1 does not exist.").arg(etcCsPersEditPath.path()));
    return;
  } else if (!EtcCsPersEditBridge::ValidateInstallationDirectory(etcCsPersEditPath)) {
    QMessageBox::critical(this,
                          tr("Bad installation directory"),
                          tr("The directory %1 does not contain a valid installation of the official personality editor.")
                              .arg(etcCsPersEditPath.path()));
    return;
  }

  Settings::SetEtcCsPersEditorPath(etcCsPersEditPath.path());
  QDialog::accept();
}

bool SettingsDialog::AllowOk() const {
  // Require a path to the official editor
  return !widgets_.etcCsPersEditPath->text().isEmpty();
}

void SettingsDialog::SEtcPersEditPathChanged() const {
  widgets_.actions->button(QDialogButtonBox::Ok)->setEnabled(AllowOk());
}

void SettingsDialog::SChooseEtcPersEditPath() {
  auto *file_dialog = new QFileDialog(this);
  file_dialog->setAcceptMode(QFileDialog::AcceptOpen);
  file_dialog->setFileMode(QFileDialog::Directory);
  file_dialog->setOption(QFileDialog::ShowDirsOnly);
  if (widgets_.etcCsPersEditPath->text().isEmpty()) {
    file_dialog->setDirectory(QDir::rootPath());
  } else {
    file_dialog->setDirectory(widgets_.etcCsPersEditPath->text());
  }
  if (file_dialog->exec() == QFileDialog::Accepted) {
    widgets_.etcCsPersEditPath->setText(file_dialog->selectedFiles().first());
  }
}

} // csprofileeditor
