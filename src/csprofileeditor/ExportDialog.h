/**
 * @file ExportDialog.h
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_EXPORTDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_EXPORTDIALOG_H_

#include <QWizard>
#include <QWizardPage>
#include <csprofile/Library.h>
#include <QTableView>
#include <QListView>
#include <QStorageInfo>
#include <QListWidget>
#include <QLabel>

namespace csprofileeditor {

/** @internal */
namespace exportdialog {

/**
 * Model class for Personalities selector.
 */
class PersonalitiesTableModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit PersonalitiesTableModel(std::shared_ptr<csprofile::Library> library, QObject *parent = nullptr);

  enum class Column {
    kInclude = 0,
    kManufacturer,
    kModel,
    kMode,
  };
  static inline const auto kColumnCount = static_cast<unsigned int>(Column::kMode) + 1;

  [[nodiscard]] int rowCount(const QModelIndex &parent) const final;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const final;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const final;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const final;
  bool setData(const QModelIndex &index, const QVariant &value, int role) final;
  [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const final;

  [[nodiscard]] const QSet<unsigned int> &GetInclude() const {
    return include_;
  }

  void SetInclude(const QSet<unsigned int> &include);
  void IncludeAll();
  void IncludeNone();

 Q_SIGNALS:
  void ZIncludeChanged(const QSet<unsigned int> &include);

 private:
  std::shared_ptr<csprofile::Library> library_;
  QSet<unsigned int> include_;
};

/**
 * Personality selector.
 *
 * This exists only to provide a widget that be used with QWizard::registerField().
 */
class PersonalitySelector : public QWidget {
 Q_OBJECT
 public:
  explicit PersonalitySelector(std::shared_ptr<csprofile::Library> library, QWidget *parent = nullptr);

  Q_PROPERTY(QSet<unsigned int> selected
                 READ getSelected
                 WRITE setSelected
                 RESET resetSelected
                 NOTIFY selectedChanged
                 USER true
  )

  [[nodiscard]] QSet<unsigned int> getSelected() const;
  void setSelected(const QSet<unsigned int> &selected);
  void resetSelected();

 Q_SIGNALS:
  void selectedChanged(const QSet<unsigned int> &selected);

 private:
  std::shared_ptr<csprofile::Library> library_;
  QTableView *personalities_table;
  PersonalitiesTableModel *personalities_table_model_;

 private Q_SLOTS:
  void SSelectedChanged(const QSet<unsigned int> &selected);
  void SSelectAll();
  void SSelectNone();
};

/**
 * Select personality page.
 */
class SelectPersonalitiesPage : public QWizardPage {
 Q_OBJECT
 public:
  explicit SelectPersonalitiesPage(std::shared_ptr<csprofile::Library> library, QWidget *parent = nullptr);
  [[nodiscard]] bool isComplete() const final;

 private:
  struct Widgets {
    PersonalitySelector *personality_selector = nullptr;
  };
  Widgets widgets_;
  std::shared_ptr<csprofile::Library> library_;

 private Q_SLOTS:
  void SSelectionChanged();
};

/**
 * Model class for drive selector.
 */
class VolumeModel : public QAbstractListModel {
 Q_OBJECT
 public:
  explicit VolumeModel(QObject *parent = nullptr);

  [[nodiscard]] QStorageInfo GetStorageInfo(const QModelIndex &index) const;
  [[nodiscard]] int GetRowForVolume(const QStorageInfo &storage_info) const;

  [[nodiscard]] int rowCount(const QModelIndex &parent) const final;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const final;

 private:
  QList<QStorageInfo> volumes_;
};

/**
 * Volume selector.
 *
 * Allows the user to select from available volumes.
 */
class VolumeSelector : public QWidget {
 Q_OBJECT
 public:
  Q_PROPERTY(QVariant selected
                 READ getSelected
                 WRITE setSelected
                 RESET resetSelected
                 NOTIFY selectedChanged
                 USER true
  )

  explicit VolumeSelector(QWidget *parent = nullptr);

  [[nodiscard]] QVariant getSelected() const;
  void setSelected(const QVariant &volume);
  void resetSelected();

 Q_SIGNALS:
  void selectedChanged(QVariant selected);

 private:
  QListView *volume_list_;
  VolumeModel *volume_model_;

 private Q_SLOTS:
  void SSelectedChanged();
};

/**
 * Select drive page.
 */
class SelectDrivePage : public QWizardPage {
 Q_OBJECT
 public:
  explicit SelectDrivePage(QWidget *parent = nullptr);
  [[nodiscard]] bool isComplete() const final;

 private:
  struct Widgets {
    VolumeSelector *drive_selector = nullptr;
  };
  Widgets widgets_;

 private Q_SLOTS:
  void SSelectionChanged();

};

/**
 * Final page
 */
class CompletePage : public QWizardPage {
 Q_OBJECT
 public:
  explicit CompletePage(std::shared_ptr<csprofile::Library> library, QWidget *parent = nullptr);
  void initializePage() override;

 private:
  std::shared_ptr<csprofile::Library> library_;
  QListWidget *selected_personalities_;
  QLabel *output_volume_;
};

} // exportdialog

/**
 * Export for console dialog.
 */
class ExportDialog : public QWizard {
 Q_OBJECT
 public:
  explicit ExportDialog(std::shared_ptr<csprofile::Library> library, QWidget *parent = nullptr);
  void accept() override;

 private:
  std::shared_ptr<csprofile::Library> library_;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_EXPORTDIALOG_H_
