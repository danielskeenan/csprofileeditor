/**
 * @file MediaSelectorDialog.cpp
 *
 * @author dankeenan
 * @date 5/2/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "MediaSelectorDialog.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <utility>
#include <QDialogButtonBox>
#include <QColor>

namespace csprofileeditor {

MediaSelectorDialog::MediaSelectorDialog(const std::shared_ptr<cslibs::Db> &db, QWidget *parent)
    : QDialog(parent), db_(db) {
  resize(640, 480);
}

void MediaSelectorDialog::Init() {
  auto *layout = new QVBoxLayout(this);

  // Tabs
  auto *tabs = new QTabWidget(this);
  layout->addWidget(tabs);
  if (db_) {
    for (const auto &manufacturer : db_->GetManufacturers()) {
      tabs->addTab(CreateSeriesWidget(manufacturer), QString::fromStdString(manufacturer.GetName()));
    }
  }

  // Actions
  auto *actions = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(actions, &QDialogButtonBox::accepted, this, &MediaSelectorDialog::accept);
  connect(actions, &QDialogButtonBox::rejected, this, &MediaSelectorDialog::reject);
  layout->addWidget(actions);
}

QWidget *MediaSelectorDialog::CreateSeriesWidget(const cslibs::Manufacturer &manufacturer) {
  const auto series_for_manufacturer = db_->GetSeriesForManufacturer(manufacturer);
  if (series_for_manufacturer.empty()) {
    return new QWidget(this);
  } else if (series_for_manufacturer.size() == 1) {
    MediaSelectorWidget *widget = this->CreateSelectorWidget(manufacturer, series_for_manufacturer.front());
    connect(widget, &MediaSelectorWidget::ZSelectionChanged, this, &MediaSelectorDialog::SSelectionChanged);
    return widget;
  } else {
    auto *tabs = new QTabWidget(this);
    for (const auto &series : series_for_manufacturer) {
      MediaSelectorWidget *widget = this->CreateSelectorWidget(manufacturer, series);
      connect(widget, &MediaSelectorWidget::ZSelectionChanged, this, &MediaSelectorDialog::SSelectionChanged);
      tabs->addTab(widget, QString::fromStdString(series.GetName()));
    }
    return tabs;
  }
}

void MediaSelectorDialog::SSelectionChanged(std::optional<csprofile::parameter::Media> media) {
  media_ = std::move(media);
}

MediaSelectorModel::MediaSelectorModel(const std::shared_ptr<cslibs::Db> &db,
                                       const cslibs::Manufacturer &manufacturer,
                                       const cslibs::Series &series,
                                       QObject *parent) :
    QAbstractTableModel(parent),
    db_(db),
    manufacturer_(manufacturer),
    series_(series) {
}

void MediaSelectorModel::Init() {
  LoadEntitiesWithFilters();
  ready_ = true;
}

int MediaSelectorModel::columnCount(const QModelIndex &parent) const {
  return kColumnCount;
}

QVariant MediaSelectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
    return {};
  }

  const auto column = static_cast<Column>(section);
  switch (column) {
    case Column::kDecoration:return {};
    case Column::kCode:return tr("Code");
    case Column::kName:return tr("Name");
  }

  return {};
}

QVariant MediaSelectorModel::data(const QModelIndex &index, int role) const {
  if (!ready_) {
    return {};
  }
  const auto column = static_cast<Column>(index.column());

  if (role == Qt::DisplayRole) {
    if (column == Column::kCode) {
      return GetEntityCode(index.row());
    } else if (column == Column::kName) {
      return GetEntityName(index.row());
    }
  } else if (role == Qt::DecorationRole) {
    if (column == Column::kDecoration) {
      return GetEntityDecoration(index.row());
    }
  }

  return {};
}

ImageMediaSelectorModel::ImageMediaSelectorModel(const std::shared_ptr<cslibs::ImageDb> &db,
                                                 const cslibs::Manufacturer &manufacturer,
                                                 const cslibs::Series &series,
                                                 QObject *parent) :
    MediaSelectorModel(db, manufacturer, series, parent), db_(db) {
}

void ImageMediaSelectorModel::LoadEntitiesWithFilters() {
  beginResetModel();
  // TODO: Filtering
  entities_ = db_->GetForSeries(series_);
  endResetModel();
}

QVariant ImageMediaSelectorModel::GetEntityDecoration(int row) const {
  const auto &image_data = entities_.at(row).GetImage();
  QPixmap pixmap;
  if (pixmap.loadFromData(reinterpret_cast<const unsigned char *>(image_data.data()), image_data.size())) {
    return QIcon(pixmap);
  }
  return {};
}

QVariant ImageMediaSelectorModel::GetEntityCode(int row) const {
  return QString::fromStdString(entities_.at(row).GetCode());
}

QVariant ImageMediaSelectorModel::GetEntityName(int row) const {
  return QString::fromStdString(entities_.at(row).GetName());
}

int ImageMediaSelectorModel::rowCount(const QModelIndex &parent) const {
  return entities_.size();
}

csprofile::parameter::Media ImageMediaSelectorModel::GetMedia(int row) const {
  const auto &entity = entities_.at(row);
  csprofile::parameter::Media media;
  media.SetName(entity.GetName());
  media.SetGoboDcid(entity.GetDcid());

  return media;
}

GelMediaSelectorModel::GelMediaSelectorModel(const std::shared_ptr<cslibs::gel::GelDb> &db,
                                             const cslibs::Manufacturer &manufacturer,
                                             const cslibs::Series &series,
                                             QObject *parent) :
    MediaSelectorModel(db, manufacturer, series, parent), db_(db) {
}

int GelMediaSelectorModel::rowCount(const QModelIndex &parent) const {
  return entities_.size();
}

void GelMediaSelectorModel::LoadEntitiesWithFilters() {
  beginResetModel();
  // TODO: Filtering
  entities_ = db_->GetGelForSeries(series_);
  endResetModel();
}

QVariant GelMediaSelectorModel::GetEntityDecoration(int row) const {
  return QColor(entities_.at(row).GetArgb());
}

QVariant GelMediaSelectorModel::GetEntityCode(int row) const {
  return QString::fromStdString(entities_.at(row).GetCode());
}

QVariant GelMediaSelectorModel::GetEntityName(int row) const {
  return QString::fromStdString(entities_.at(row).GetName());
}

csprofile::parameter::Media GelMediaSelectorModel::GetMedia(int row) const {
  const auto &entity = entities_.at(row);
  csprofile::parameter::Media media;
  media.SetName(entity.GetName());
  media.SetRgb(entity.GetArgb());

  return media;
}

ImageMediaSelectorDialog::ImageMediaSelectorDialog(const std::shared_ptr<cslibs::ImageDb> &db, QWidget *parent) :
    MediaSelectorDialog(db, parent), db_(db) {
}

MediaSelectorWidget *ImageMediaSelectorDialog::CreateSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                                                    const cslibs::Series &series) {
  return new ImageMediaSelectorWidget(db_, manufacturer, series, this);
}

GelSelectorDialog::GelSelectorDialog(const std::shared_ptr<cslibs::gel::GelDb> &db, QWidget *parent) :
    MediaSelectorDialog(db, parent), db_(db) {
}

MediaSelectorWidget *GelSelectorDialog::CreateSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                                             const cslibs::Series &series) {
  return new GelSelectorWidget(db_, manufacturer, series, this);
}

MediaSelectorWidget::MediaSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                         const cslibs::Series &series,
                                         QWidget *parent)
    : QWidget(parent),
      manufacturer_(manufacturer),
      series_(series),
      table_(new QTableView(this)) {
  auto *layout = new QVBoxLayout(this);
  layout->addWidget(table_);
  table_->setSelectionMode(QTableView::SingleSelection);
  table_->setSelectionBehavior(QTableView::SelectRows);
}

std::optional<csprofile::parameter::Media> MediaSelectorWidget::GetMedia() const {
  const QItemSelection selection = table_->selectionModel()->selection();
  if (selection.empty()) {
    return {};
  }

  return GetModel()->GetMedia(selection.first().top());
}

void MediaSelectorWidget::SSelectionChanged() {
  Q_EMIT(ZSelectionChanged(GetMedia()));
}

ImageMediaSelectorWidget::ImageMediaSelectorWidget(const std::shared_ptr<cslibs::ImageDb> &db,
                                                   const cslibs::Manufacturer &manufacturer,
                                                   const cslibs::Series &series,
                                                   QWidget *parent) :
    MediaSelectorWidget(manufacturer, series, parent) {
  model_ = new ImageMediaSelectorModel(db, manufacturer_, series_, this);
  model_->Init();
  table_->setModel(model_);
  connect(table_->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &ImageMediaSelectorWidget::SSelectionChanged);
}

GelSelectorWidget::GelSelectorWidget(const std::shared_ptr<cslibs::gel::GelDb> &db,
                                     const cslibs::Manufacturer &manufacturer,
                                     const cslibs::Series &series,
                                     QWidget *parent) :
    MediaSelectorWidget(manufacturer, series, parent) {
  model_ = new GelMediaSelectorModel(db, manufacturer_, series_, this);
  model_->Init();
  table_->setModel(model_);
  connect(table_->selectionModel(),
          &QItemSelectionModel::selectionChanged,
          this,
          &GelSelectorWidget::SSelectionChanged);
}

} // csprofileeditor
