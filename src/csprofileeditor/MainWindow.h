/**
 * @file MainWindow.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_MAINWINDOW_H_
#define CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_MAINWINDOW_H_

#include <QMainWindow>
#include <csprofile/Library.h>
#include <QTableView>
#include "PersonalityTableModel.h"

namespace csprofileeditor {

/**
 * Main Window
 */
class MainWindow : public QMainWindow {
 Q_OBJECT
 public:
  explicit MainWindow(QWidget *parent = nullptr);

 protected:
  void closeEvent(QCloseEvent *event) final;

 private:
  struct Actions {
    // File
    QAction *act_file_new = nullptr;
    QAction *act_file_open = nullptr;
    QAction *act_file_save = nullptr;
    QAction *act_file_saveas = nullptr;
    QAction *act_file_export = nullptr;
    QMenu *menu_recent_documents = nullptr;
    QAction *act_quit = nullptr;

    // Edit
    QAction *act_edit_personality = nullptr;
    QAction *act_delete_personality = nullptr;
    QAction *act_add_personality = nullptr;
    QAction *act_settings = nullptr;
  };
  Actions actions_;
  struct Widgets {
    QTableView *personality_table = nullptr;
  };
  Widgets widgets_;
  PersonalityTableModel *personality_table_model_ = nullptr;

  std::shared_ptr<csprofile::Library> library_;

  static inline const auto kFileSuffix = "jlib";
  // Translation means this can't be static const.
  QString kFileNameFilter;

  void InitUi();
  void InitActions();
  void InitMenu();
  void InitToolbar();

  /**
   * Ask the user to save unsaved data if the file has been modified.
   * @return TRUE if the action should continue, FALSE for cancelled.
   */
  bool AskAboutUnsavedData();
  void OpenFrom(const QString &path);
  void SaveTo(const QString &path);
  void AddPathToRecentDocuments(const QString &path);
  void UpdateRecentDocuments();
  [[nodiscard]] bool PersonalityActionsAllowed() const;
  [[nodiscard]] bool ExportingAllowed() const;

 private Q_SLOTS:
  // Actions
  void SFileNew();
  void SFileOpen();
  void SFileSave();
  void SFileSaveAs();
  void SFileExport();
  void SEditPersonality();
  void SDeletePersonality();
  void SAddPersonality();
  void SSettings();

  void SPersonalityChanged();
  void SSelectedTableRowChanged() const;
};

} // csprofileeditor

#endif //CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_MAINWINDOW_H_
