/**
 * @file MainWindow.cpp
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "MainWindow.h"
#include "Settings.h"
#include "SettingsDialog.h"
#include <QCloseEvent>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <csprofile/except.h>
#include "EtcCsPersEditBridge.h"
#include "PersonalityEditDialog.h"
#include "ExportDialog.h"
#include "CsLibUpdater.h"
#include <csprofile/logging.h>

namespace csprofileeditor {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      library_(new csprofile::Library),
      kFileNameFilter(tr("Fixture Library (*.%1)").arg(kFileSuffix)) {
  InitActions();
  InitMenu();
  InitToolbar();
  InitUi();

  // Ensure a path to the official editor is set before continuing.
  if (!EtcCsPersEditBridge::ValidateInstallationDirectory()) {
    QMessageBox::information(this,
                             tr("ETC ColorSource Personality Editor required"),
                             tr("This program uses data files included with the official editor.  Choose the installation directory in Settings."));
    auto *settings_dialog = new SettingsDialog(this);
    connect(settings_dialog, &SettingsDialog::rejected, qApp, &QApplication::quit, Qt::QueuedConnection);
    settings_dialog->exec();
  }
  csprofile::logging::info("Found ETC CS Editor at {}", Settings::GetEtcCsPersEditorPath().toStdString());

  if (!CsLibUpdater::UpToDate()) {
    auto *updater = new CsLibUpdater(this);
    connect(updater, &CsLibUpdater::rejected, qApp, &QApplication::quit, Qt::QueuedConnection);
    connect(updater, &CsLibUpdater::finished, updater, &CsLibUpdater::deleteLater);
    updater->setModal(true);
    updater->open();
  }
}

void MainWindow::InitActions() {
  // File
  // New
  actions_.act_file_new = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
  actions_.act_file_new->setShortcut(QKeySequence::New);
  connect(actions_.act_file_new, &QAction::triggered, this, &MainWindow::SFileNew);

  // Open
  actions_.act_file_open = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
  actions_.act_file_open->setShortcut(QKeySequence::Open);
  connect(actions_.act_file_open, &QAction::triggered, this, &MainWindow::SFileOpen);

  // Save
  actions_.act_file_save = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
  actions_.act_file_save->setShortcut(QKeySequence::Save);
  connect(actions_.act_file_save, &QAction::triggered, this, &MainWindow::SFileSave);

  // Save as
  actions_.act_file_saveas = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
  actions_.act_file_saveas->setShortcut(QKeySequence::SaveAs);
  connect(actions_.act_file_saveas, &QAction::triggered, this, &MainWindow::SFileSaveAs);

  // Export
  actions_.act_file_export = new QAction(QIcon::fromTheme("document-export"), tr("E&xport for Console..."), this);
  actions_.act_file_export->setEnabled(ExportingAllowed());
  connect(actions_.act_file_export, &QAction::triggered, this, &MainWindow::SFileExport);

  // Edit personality
  actions_.act_edit_personality = new QAction(QIcon::fromTheme("document-edit"), tr("&Edit personality..."), this);
  actions_.act_edit_personality->setEnabled(PersonalityActionsAllowed());
  connect(actions_.act_edit_personality, &QAction::triggered, this, &MainWindow::SEditPersonality);

  // Delete personality
  actions_.act_delete_personality = new QAction(QIcon::fromTheme("list-remove"), tr("&Delete personality"), this);
  actions_.act_delete_personality->setEnabled(PersonalityActionsAllowed());
  actions_.act_delete_personality->setShortcut(QKeySequence::Delete);
  connect(actions_.act_delete_personality, &QAction::triggered, this, &MainWindow::SDeletePersonality);

  // Add personality
  actions_.act_add_personality = new QAction(QIcon::fromTheme("list-add"), tr("&Add personality"), this);
  connect(actions_.act_add_personality, &QAction::triggered, this, &MainWindow::SAddPersonality);

  // Settings
  actions_.act_settings = new QAction(QIcon::fromTheme("configure"), tr("Se&ttings"), this);
  connect(actions_.act_settings, &QAction::triggered, this, &MainWindow::SSettings);

  // Quit
  actions_.act_quit = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
  actions_.act_quit->setShortcut(QKeySequence::Quit);
  connect(actions_.act_quit, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::InitMenu() {
  // File
  QMenu *menu_file = menuBar()->addMenu(tr("&File"));
  menu_file->addAction(actions_.act_file_new);
  menu_file->addAction(actions_.act_file_open);
  menu_file->addAction(actions_.act_file_save);
  menu_file->addAction(actions_.act_file_saveas);
  menu_file->addAction(actions_.act_file_export);
  // Recent documents
  actions_.menu_recent_documents = menu_file->addMenu(tr("&Recent Documents"));
  UpdateRecentDocuments();
  menu_file->addSeparator();
  menu_file->addAction(actions_.act_quit);

  // Edit
  QMenu *menu_edit = menuBar()->addMenu(tr("&Edit"));
  menu_edit->addAction(actions_.act_edit_personality);
  menu_edit->addAction(actions_.act_delete_personality);
  menu_edit->addAction(actions_.act_add_personality);
  menu_edit->addSeparator();
  menu_edit->addAction(actions_.act_settings);
}

void MainWindow::InitToolbar() {
  QToolBar *toolbar_main = addToolBar(tr("Main"));
  toolbar_main->addAction(actions_.act_file_new);
  toolbar_main->addAction(actions_.act_file_open);
  toolbar_main->addAction(actions_.act_file_save);

  QToolBar *toolbar_actions = addToolBar(tr("Actions"));
  toolbar_actions->addAction(actions_.act_add_personality);
  toolbar_actions->addAction(actions_.act_delete_personality);
  toolbar_actions->addAction(actions_.act_edit_personality);
}

void MainWindow::InitUi() {
  if (!restoreGeometry(Settings::GetMainWindowGeometry())) {
    resize(600, 600);
  }

  widgets_.personality_table = new QTableView(this);
  setCentralWidget(widgets_.personality_table);
  personality_table_model_ = new PersonalityTableModel(library_, this);
  widgets_.personality_table->setModel(personality_table_model_);
  widgets_.personality_table->setContextMenuPolicy(Qt::ActionsContextMenu);
  widgets_.personality_table->addAction(actions_.act_edit_personality);
  widgets_.personality_table->addAction(actions_.act_delete_personality);
  widgets_.personality_table->addAction(actions_.act_add_personality);
  connect(personality_table_model_, &PersonalityTableModel::dataChanged, this, &MainWindow::SPersonalityChanged);
  connect(personality_table_model_, &PersonalityTableModel::rowsInserted, this, &MainWindow::SPersonalityChanged);
  connect(personality_table_model_, &PersonalityTableModel::rowsRemoved, this, &MainWindow::SPersonalityChanged);
  connect(widgets_.personality_table->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &MainWindow::SSelectedTableRowChanged);
}

bool MainWindow::AskAboutUnsavedData() {
  if (isWindowModified()) {
    const QMessageBox::StandardButton save_choice =
        QMessageBox::question(this,
                              tr("Unsaved data"),
                              tr("This file has been modified.  Do you want to save?"),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (save_choice == QMessageBox::Save) {
      SFileSave();
      if (isWindowModified()) {
        // Window still modified means user cancelled save
        return false;
      }
    } else if (save_choice == QMessageBox::Cancel) {
      return false;
    }
  }

  return true;
}

void MainWindow::OpenFrom(const QString &path) {
  try {
    library_.reset(new csprofile::Library(path.toStdString()));
    setWindowFilePath(path);
    personality_table_model_->SetLibrary(library_);
    widgets_.personality_table->resizeColumnsToContents();
    setWindowModified(false);
    AddPathToRecentDocuments(path);
    actions_.act_file_export->setEnabled(ExportingAllowed());
  } catch (const csprofile::except::ParseError &e) {
    QMessageBox::critical(this,
                          tr("Error opening file"),
                          tr("The file %1 is not valid.  Either it is not a library file or is corrupted.").arg(path));
  } catch (const std::runtime_error &e) {
    QMessageBox::critical(this,
                          tr("Error opening file"),
                          tr("The file %1 cannot be read.  Be sure the file is still accessible.").arg(path));
  }
}

void MainWindow::SaveTo(const QString &path) {
  try {
    library_->Save(path.toStdString());
    setWindowFilePath(path);
    setWindowModified(false);
    AddPathToRecentDocuments(path);
  } catch (const std::runtime_error &e) {
    QMessageBox::critical(this,
                          tr("Error saving file"),
                          tr("The file %1 cannot be written.  Be sure the location is accessible and not read-only.")
                              .arg(path));
  }
}

void MainWindow::AddPathToRecentDocuments(const QString &path) {
  QStringList recent_documents = Settings::GetRecentDocuments();
  if (!recent_documents.contains(path)) {
    // Add to the list, capping the number of document retained.
    recent_documents.push_front(path);
    while (recent_documents.size() > Settings::GetRecentDocumentsCount()) {
      recent_documents.removeLast();
    }
  } else {
    // Move this document to the top.
    std::sort(recent_documents.begin(), recent_documents.end(), [&path](const QString &a, const QString &b) {
      return a == path && b != path;
    });
  }
  Settings::SetRecentDocuments(recent_documents);
  UpdateRecentDocuments();
}

void MainWindow::UpdateRecentDocuments() {
  actions_.menu_recent_documents->clear();
  if (Settings::GetRecentDocuments().empty()) {
    QAction *open_recent_document = actions_.menu_recent_documents->addAction(tr("No recent documents"));
    open_recent_document->setEnabled(false);
  } else {
    for (const auto &path : Settings::GetRecentDocuments()) {
      QAction *open_recent_document = actions_.menu_recent_documents->addAction(path);
      connect(open_recent_document, &QAction::triggered, [path, this]() {
        if (!AskAboutUnsavedData()) {
          return;
        }
        OpenFrom(path);
      });
    }
  }
}

bool MainWindow::PersonalityActionsAllowed() const {
  return widgets_.personality_table != nullptr && widgets_.personality_table->selectionModel()->hasSelection();
}

bool MainWindow::ExportingAllowed() const {
  return !library_->personalities.empty();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (!AskAboutUnsavedData()) {
    event->ignore();
    return;
  }
  Settings::SetMainWindowGeometry(saveGeometry());
  event->accept();
}

void MainWindow::SFileNew() {
  if (!AskAboutUnsavedData()) {
    return;
  }
  library_.reset(new csprofile::Library);
  setWindowFilePath({});
}

void MainWindow::SFileOpen() {
  if (!AskAboutUnsavedData()) {
    return;
  }
  auto *file_dialog = new QFileDialog(this);
  file_dialog->setAcceptMode(QFileDialog::AcceptOpen);
  file_dialog->setFileMode(QFileDialog::ExistingFile);
  file_dialog->setDefaultSuffix(kFileSuffix);
  file_dialog->setNameFilter(kFileNameFilter);
  file_dialog->setDirectory(Settings::GetLastFileDialogPath());
  if (file_dialog->exec() == QFileDialog::Accepted) {
    const QStringList &selected_files = file_dialog->selectedFiles();
    const QString &path = selected_files.front();
    OpenFrom(path);
    QDir dir(path);
    dir.cdUp();
    Settings::SetLastFileDialogPath(dir.absolutePath());
  }
}

void MainWindow::SFileSave() {
  if (windowFilePath().isEmpty()) {
    SFileSaveAs();
  } else {
    SaveTo(windowFilePath());
  }
}

void MainWindow::SFileSaveAs() {
  auto *file_dialog = new QFileDialog(this);
  file_dialog->setAcceptMode(QFileDialog::AcceptSave);
  file_dialog->setFileMode(QFileDialog::AnyFile);
  file_dialog->setDefaultSuffix(kFileSuffix);
  file_dialog->setNameFilter(kFileNameFilter);
  file_dialog->setDirectory(Settings::GetLastFileDialogPath());
  if (!windowFilePath().isEmpty()) {
    file_dialog->selectFile(windowFilePath());
  }
  if (file_dialog->exec() == QFileDialog::Accepted) {
    const QStringList &selected_files = file_dialog->selectedFiles();
    const QString &path = selected_files.front();
    SaveTo(path);
    QDir dir(path);
    dir.cdUp();
    Settings::SetLastFileDialogPath(dir.absolutePath());
  }
}

void MainWindow::SFileExport() {
  auto *dialog = new ExportDialog(library_, this);
  dialog->exec();
}

void MainWindow::SEditPersonality() {
  if (!PersonalityActionsAllowed()) {
    return;
  }
  const QModelIndex selection = widgets_.personality_table->selectionModel()->currentIndex();
  if (!selection.isValid()) {
    return;
  }
  const unsigned int row = selection.row();
  const csprofile::Personality &personality = library_->personalities.at(row);

  // Edit dialog creates a copy of the personality for editing.
  auto *edit_dialog = new PersonalityEditDialog(personality, this);
  if (edit_dialog->exec() == PersonalityEditDialog::Accepted) {
    const bool modified = personality != edit_dialog->GetPersonality();
    if (modified) {
      personality_table_model_->UpdatePersonality(row, edit_dialog->GetPersonality());
      setWindowModified(modified);
    }
  }
}

void MainWindow::SDeletePersonality() {
  if (!PersonalityActionsAllowed()) {
    return;
  }

  std::unordered_set<int> rows;
  for (const auto &row : widgets_.personality_table->selectionModel()->selectedIndexes()) {
    rows.insert(row.row());
  }
  if (rows.empty()) {
    return;
  }
  QString delete_message;
  if (rows.size() == 1) {
    const int row = *rows.cbegin();
    const QString &manufacturer = personality_table_model_->data(
        personality_table_model_->index(row, static_cast<int>(PersonalityTableModel::Column::kManufacturer)),
        Qt::DisplayRole).toString();
    const QString &model = personality_table_model_->data(
        personality_table_model_->index(row, static_cast<int>(PersonalityTableModel::Column::kModel)),
        Qt::DisplayRole).toString();
    const QString &mode = personality_table_model_->data(
        personality_table_model_->index(row, static_cast<int>(PersonalityTableModel::Column::kMode)),
        Qt::DisplayRole).toString();
    //: %1 = Manufacturer; %2 = Model; %3 = Mode
    delete_message = tr("Are you sure you want to delete %1 %2 (%3)?").arg(manufacturer, model, mode);
  } else {
    delete_message = tr("Are you sure you want to delete %n personalities?", "", rows.size());
  }
  const auto really_delete = QMessageBox::question(this, tr("Delete personalities", "", rows.size()), delete_message);
  if (really_delete == QMessageBox::Yes) {
    personality_table_model_->removeRows(std::vector<int>(rows.cbegin(), rows.cend()), QModelIndex());
    actions_.act_file_export->setEnabled(ExportingAllowed());
  }
}

void MainWindow::SAddPersonality() {
  personality_table_model_->insertRows(personality_table_model_->rowCount(QModelIndex()), 1, QModelIndex());
  actions_.act_file_export->setEnabled(ExportingAllowed());
}

void MainWindow::SSettings() {
  auto settings_dialog = new SettingsDialog(this);
  settings_dialog->exec();
}

void MainWindow::SPersonalityChanged() {
  setWindowModified(true);
}

void MainWindow::SSelectedTableRowChanged() const {
  const bool allow_personality_actions = PersonalityActionsAllowed();
  actions_.act_edit_personality->setEnabled(allow_personality_actions);
  actions_.act_delete_personality->setEnabled(allow_personality_actions);
}

} // csprofileeditor
