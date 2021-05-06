/**
 * @file ExportDialog.cpp
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "ExportDialog.h"
#include <QVBoxLayout>
#include <utility>
#include <QPushButton>

namespace csprofileeditor {

namespace exportdialog {

PersonalitiesTableModel::PersonalitiesTableModel(std::shared_ptr<csprofile::Library> library,
                                                 QObject *parent) :
    QAbstractTableModel(parent), library_(std::move(library)) {
  include_.reserve(library_->personalities.size());
}

int PersonalitiesTableModel::rowCount(const QModelIndex &parent) const {
  return library_->personalities.size();
}

int PersonalitiesTableModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant PersonalitiesTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal) {
    return {};
  }
  const auto column = static_cast<Column>(section);

  if (role == Qt::DisplayRole) {
    switch (column) {
      case Column::kInclude:return {};
      case Column::kManufacturer:return tr("Manufacturer");
      case Column::kModel:return tr("Model");
      case Column::kMode:return tr("Mode");
    }
  }
  return {};
}

QVariant PersonalitiesTableModel::data(const QModelIndex &index, int role) const {
  const auto column = static_cast<Column>(index.column());
  const auto &personality = library_->personalities.at(index.row());

  if (role == Qt::DisplayRole) {
    if (column == Column::kManufacturer) {
      return QString::fromStdString(personality.GetManufacturerName());
    } else if (column == Column::kModel) {
      return QString::fromStdString(personality.GetModelName());
    } else if (column == Column::kMode) {
      return QString::fromStdString(personality.GetModeName());
    }
  } else if (role == Qt::CheckStateRole) {
    if (column == Column::kInclude) {
      return include_.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
    }
  }

  return {};
}

bool PersonalitiesTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  const auto column = static_cast<Column>(index.column());

  if (role == Qt::CheckStateRole) {
    const bool checked = value.value<Qt::CheckState>() == Qt::Checked;
    if (column == Column::kInclude) {
      if (checked) {
        include_.insert(index.row());
      } else {
        include_.remove(index.row());
      }
      Q_EMIT(ZIncludeChanged(include_));
    }
  }

  Q_EMIT(dataChanged(index, index));

  return true;
}

Qt::ItemFlags PersonalitiesTableModel::flags(const QModelIndex &index) const {
  const auto column = static_cast<Column>(index.column());
  const auto flags = Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemNeverHasChildren;

  switch (column) {
    case Column::kInclude:return flags | Qt::ItemIsUserCheckable;
    case Column::kManufacturer:
    case Column::kModel:
    case Column::kMode:return flags;
  }

  return flags;
}

void PersonalitiesTableModel::SetInclude(const QSet<unsigned int> &include) {
  include_ = include;
  Q_EMIT(dataChanged(createIndex(0, static_cast<int>(Column::kInclude)),
                     createIndex(rowCount(QModelIndex()) - 1, static_cast<int>(Column::kInclude))));
  Q_EMIT(ZIncludeChanged(include_));
}

void PersonalitiesTableModel::IncludeAll() {
  for (unsigned int i = 0; i < rowCount(QModelIndex()); ++i) {
    include_.insert(i);
  }
  Q_EMIT(dataChanged(createIndex(0, static_cast<int>(Column::kInclude)),
                     createIndex(rowCount(QModelIndex()) - 1, static_cast<int>(Column::kInclude))));
  Q_EMIT(ZIncludeChanged(include_));
}

void PersonalitiesTableModel::IncludeNone() {
  include_.clear();
  Q_EMIT(dataChanged(createIndex(0, static_cast<int>(Column::kInclude)),
                     createIndex(rowCount(QModelIndex()) - 1, static_cast<int>(Column::kInclude))));
  Q_EMIT(ZIncludeChanged(include_));
}

PersonalitySelector::PersonalitySelector(std::shared_ptr<csprofile::Library> library, QWidget *parent) :
    QWidget(parent),
    library_(std::move(library)),
    personalities_table(new QTableView(this)),
    personalities_table_model_(new PersonalitiesTableModel(library_, this)) {
  auto *layout = new QVBoxLayout(this);

  // Table
  layout->addWidget(personalities_table);
  personalities_table->setModel(personalities_table_model_);
  personalities_table->resizeColumnsToContents();
  connect(personalities_table_model_,
          &PersonalitiesTableModel::ZIncludeChanged,
          this,
          &PersonalitySelector::SSelectedChanged);

  // Select all/none
  auto *button_layout = new QHBoxLayout;
  layout->addLayout(button_layout);
  auto *select_all = new QPushButton(QIcon::fromTheme("edit-select-all"), tr("Select All"), this);
  connect(select_all, &QPushButton::clicked, this, &PersonalitySelector::SSelectAll);
  button_layout->addWidget(select_all);
  auto *select_none = new QPushButton(QIcon::fromTheme("edit-select-none"), tr("Select None"), this);
  connect(select_none, &QPushButton::clicked, this, &PersonalitySelector::SSelectNone);
  button_layout->addWidget(select_none);
  button_layout->addStretch();
}

QSet<unsigned int> PersonalitySelector::getSelected() const {
  return personalities_table_model_->GetInclude();
}

void PersonalitySelector::setSelected(const QSet<unsigned int> &selected) {
  personalities_table_model_->SetInclude(selected);
}

void PersonalitySelector::resetSelected() {
  personalities_table_model_->SetInclude({});
}

void PersonalitySelector::SSelectedChanged(const QSet<unsigned int> &selected) {
  // Forward signal from model
  Q_EMIT(selectedChanged(selected));
}

void PersonalitySelector::SSelectAll() {
  personalities_table_model_->IncludeAll();
}

void PersonalitySelector::SSelectNone() {
  personalities_table_model_->IncludeNone();
}

SelectPersonalitiesPage::SelectPersonalitiesPage(std::shared_ptr<csprofile::Library> library,
                                                 QWidget *parent)
    : QWizardPage(parent),
      library_(std::move(library)) {
  setTitle(tr("Select Personalities"));
  auto *layout = new QVBoxLayout;
  setLayout(layout);

  widgets_.personality_selector = new PersonalitySelector(library_, this);
  connect(widgets_.personality_selector,
          &PersonalitySelector::selectedChanged,
          this,
          &SelectPersonalitiesPage::SSelectionChanged);
  layout->addWidget(widgets_.personality_selector);
  registerField("selected", widgets_.personality_selector, "selected");
}

bool SelectPersonalitiesPage::isComplete() const {
  return !widgets_.personality_selector->getSelected().empty();
}

void SelectPersonalitiesPage::SSelectionChanged() {
  Q_EMIT(completeChanged());
}

VolumeModel::VolumeModel(QObject *parent) : QAbstractListModel(parent) {
  for (const auto &volume : QStorageInfo::mountedVolumes()) {
    if (volume.isValid() && !volume.isReadOnly() && volume.isReady() && !volume.isRoot()) {
      volumes_.push_back(volume);
    }
  }
}

QStorageInfo VolumeModel::GetStorageInfo(const QModelIndex &index) const {
  return volumes_.at(index.row());
}

int VolumeModel::GetRowForVolume(const QStorageInfo &storage_info) const {
  return volumes_.indexOf(storage_info);
}

int VolumeModel::rowCount(const QModelIndex &parent) const {
  return volumes_.size();
}

QVariant VolumeModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    return volumes_.at(index.row()).displayName();
  }

  return {};
}

VolumeSelector::VolumeSelector(QWidget *parent) :
    QWidget(parent), volume_list_(new QListView(this)), volume_model_(new VolumeModel(this)) {
  auto *layout = new QVBoxLayout(this);
  layout->addWidget(volume_list_);
  volume_list_->setModel(volume_model_);
  volume_list_->setSelectionBehavior(QListView::SelectRows);
  volume_list_->setSelectionMode(QListView::SingleSelection);
  connect(volume_list_->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &VolumeSelector::SSelectedChanged);
}

QVariant VolumeSelector::getSelected() const {
  const auto selection = volume_list_->selectionModel()->selectedIndexes();
  if (selection.empty()) {
    return {};
  }
  return QVariant::fromValue(volume_model_->GetStorageInfo(selection.front()));
}

void VolumeSelector::setSelected(const QVariant &volume) {
  if (!volume.isValid()) {
    volume_list_->clearSelection();
  } else if (volume.canConvert<QStorageInfo>()) {
    const int row = volume_model_->GetRowForVolume(volume.value<QStorageInfo>());
    if (row > 0) {
      volume_list_->selectionModel()->select(volume_model_->index(row, 0),
                                             QItemSelectionModel::Clear
                                                 | QItemSelectionModel::Select
                                                 | QItemSelectionModel::Current
                                                 | QItemSelectionModel::Rows);
    }
  }
}

void VolumeSelector::resetSelected() {
  volume_list_->clearSelection();
}

void VolumeSelector::SSelectedChanged() {
  Q_EMIT(selectedChanged(getSelected()));
}

SelectDrivePage::SelectDrivePage(QWidget *parent) : QWizardPage(parent) {
  setTitle(tr("Select Drive"));
  auto *layout = new QVBoxLayout;
  setLayout(layout);

  widgets_.drive_selector = new VolumeSelector(this);
  connect(widgets_.drive_selector, &VolumeSelector::selectedChanged, this, &SelectDrivePage::SSelectionChanged);
  layout->addWidget(widgets_.drive_selector);
  registerField("drive", widgets_.drive_selector, "selected");
}

bool SelectDrivePage::isComplete() const {
  return widgets_.drive_selector->getSelected().isValid();
}

void SelectDrivePage::SSelectionChanged() {
  Q_EMIT(completeChanged());
}

CompletePage::CompletePage(std::shared_ptr<csprofile::Library> library, QWidget *parent) :
    QWizardPage(parent),
    library_(std::move(library)),
    selected_personalities_(new QListWidget(this)),
    output_volume_(new QLabel(this)) {
  setCommitPage(true);
  setTitle(tr("Save for Console"));
  auto *layout = new QVBoxLayout;
  setLayout(layout);

  auto *personalities_message = new QLabel(this);
  personalities_message->setWordWrap(true);
  personalities_message->setText(tr("The following personalities will be exported for the console:"));
  layout->addWidget(personalities_message);

  selected_personalities_->setSelectionMode(QListWidget::NoSelection);
  layout->addWidget(selected_personalities_);

  auto *output_message = new QLabel(this);
  output_message->setWordWrap(true);
  output_message->setText(tr("They will be saved on:"));
  layout->addWidget(output_message);

  output_volume_->setWordWrap(true);
  output_volume_->setContentsMargins(20, 0, 0, 0);
  layout->addWidget(output_volume_);
}

void CompletePage::initializePage() {
  // Selected personalities
  selected_personalities_->clear();
  for (const auto personality_index : field("selected").value<QSet<unsigned int>>()) {
    const auto &personality = library_->personalities.at(personality_index);
    selected_personalities_->addItem(tr("%1 %2 (%3)").arg(QString::fromStdString(personality.GetManufacturerName()),
                                                          QString::fromStdString(personality.GetModelName()),
                                                          QString::fromStdString(personality.GetModeName())));
  }
  selected_personalities_->sortItems();

  // Output volume
  output_volume_->setText(field("drive").value<QStorageInfo>().displayName());
}

} // exportdialog

ExportDialog::ExportDialog(std::shared_ptr<csprofile::Library> library, QWidget *parent)
    : QWizard(parent), library_(std::move(library)) {
  setWindowTitle(tr("Export for Console"));

  addPage(new exportdialog::SelectPersonalitiesPage(library_));
  addPage(new exportdialog::SelectDrivePage);
  addPage(new exportdialog::CompletePage(library_));
}

void ExportDialog::accept() {
  // Copy the requested personalities into a new library.
  csprofile::Library output;
  for (const auto personality_index : field("selected").value<QSet<unsigned int>>()) {
    output.personalities.push_back(library_->personalities.at(personality_index));
  }

  QDir output_path(field("drive").value<QStorageInfo>().rootPath());
  // ColorSource console looks for a file with this name in the root of the drive.
  output.Save(output_path.absoluteFilePath("userlib.jlib").toStdString());

  QWizard::accept();
}

} // csprofileeditor
