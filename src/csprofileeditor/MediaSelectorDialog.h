/**
 * @file MediaSelectorDialog.h
 *
 * @author dankeenan
 * @date 5/2/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_MEDIASELECTORDIALOG_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_MEDIASELECTORDIALOG_H_

#include <QDialog>
#include <cslibs/Db.h>
#include <csprofile/parameter/Media.h>
#include <QAbstractTableModel>
#include <cslibs/gel/GelDb.h>
#include <QTableView>
#include <utility>

namespace csprofileeditor {

class MediaSelectorWidget;

/**
 * Select media for a range
 */
class MediaSelectorDialog : public QDialog {
 Q_OBJECT
 public:
  explicit MediaSelectorDialog(const std::shared_ptr<cslibs::Db> &db, QWidget *parent = nullptr);
  void Init();

  [[nodiscard]] const std::optional<csprofile::parameter::Media> &GetMedia() const {
    return media_;
  }

  void SetMedia(const std::optional<csprofile::parameter::Media> &media) {
    media_ = media;
  }

 protected:
  std::shared_ptr<cslibs::Db> db_;
  std::optional<csprofile::parameter::Media> media_;

 private:
  QWidget *CreateSeriesWidget(const cslibs::Manufacturer &manufacturer);
  virtual MediaSelectorWidget *CreateSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                                    const cslibs::Series &series) = 0;

 private Q_SLOTS:
  void SSelectionChanged(std::optional<csprofile::parameter::Media> media);
};

/**
 * Base class for media selector models.
 */
class MediaSelectorModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit MediaSelectorModel(const std::shared_ptr<cslibs::Db> &db,
                              const cslibs::Manufacturer &manufacturer,
                              const cslibs::Series &series,
                              QObject *parent = nullptr);
  void Init();

  enum class Column {
    kDecoration = 0,
    kCode,
    kName,
  };
  static inline const auto kColumnCount = static_cast<unsigned int>(Column::kName) + 1;

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override = 0;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
  [[nodiscard]] virtual csprofile::parameter::Media GetMedia(int row) const = 0;

 protected:
  bool ready_ = false;
  std::shared_ptr<cslibs::Db> db_;
  cslibs::Manufacturer manufacturer_;
  cslibs::Series series_;
  struct Filters {
    std::string code;
    std::string name;
  };
  Filters filters_;

  virtual void LoadEntitiesWithFilters() = 0;
  [[nodiscard]] virtual QVariant GetEntityDecoration(int row) const = 0;
  [[nodiscard]] virtual QVariant GetEntityCode(int row) const = 0;
  [[nodiscard]] virtual QVariant GetEntityName(int row) const = 0;

 private:

};

/**
 * Model class for ImageMediaSelectorDialog.
 */
class ImageMediaSelectorModel : public MediaSelectorModel {
 Q_OBJECT
 public:
  explicit ImageMediaSelectorModel(const std::shared_ptr<cslibs::ImageDb> &db,
                                   const cslibs::Manufacturer &manufacturer,
                                   const cslibs::Series &series,
                                   QObject *parent = nullptr);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] csprofile::parameter::Media GetMedia(int row) const override;

 protected:
  void LoadEntitiesWithFilters() override;
  [[nodiscard]] QVariant GetEntityDecoration(int row) const override;
  [[nodiscard]] QVariant GetEntityCode(int row) const override;
  [[nodiscard]] QVariant GetEntityName(int row) const override;

 private:
  std::shared_ptr<cslibs::ImageDb> db_;
  std::vector<cslibs::ImageEntity> entities_;
};

/**
 * Model class for GelMediaSelectorDialog
 */
class GelMediaSelectorModel final : public MediaSelectorModel {
 Q_OBJECT
 public:
  explicit GelMediaSelectorModel(const std::shared_ptr<cslibs::gel::GelDb> &db,
                                 const cslibs::Manufacturer &manufacturer,
                                 const cslibs::Series &series,
                                 QObject *parent = nullptr);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const final;
  [[nodiscard]] csprofile::parameter::Media GetMedia(int row) const override;

 protected:
  void LoadEntitiesWithFilters() override;
  [[nodiscard]] QVariant GetEntityDecoration(int row) const final;
  [[nodiscard]] QVariant GetEntityCode(int row) const final;
  [[nodiscard]] QVariant GetEntityName(int row) const final;

 private:
  std::shared_ptr<cslibs::gel::GelDb> db_;
  std::vector<cslibs::gel::Gel> entities_;
};

/**
 * Select image-based media (e.g. Gobos)
 */
class ImageMediaSelectorDialog : public MediaSelectorDialog {
 Q_OBJECT
 public:
  explicit ImageMediaSelectorDialog(const std::shared_ptr<cslibs::ImageDb> &db, QWidget *parent = nullptr);

 protected:
  std::shared_ptr<cslibs::ImageDb> db_;

 private:
  MediaSelectorWidget *CreateSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                            const cslibs::Series &series) override;
};

/**
 * Select gels
 */
class GelSelectorDialog final : public MediaSelectorDialog {
 Q_OBJECT
 public:
  explicit GelSelectorDialog(const std::shared_ptr<cslibs::gel::GelDb> &db, QWidget *parent = nullptr);

 private:
  std::shared_ptr<cslibs::gel::GelDb> db_;

  MediaSelectorWidget *CreateSelectorWidget(const cslibs::Manufacturer &manufacturer,
                                            const cslibs::Series &series) final;
};

/**
 * Base class for media selector widgets
 */
class MediaSelectorWidget : public QWidget {
 Q_OBJECT
 public:
  explicit MediaSelectorWidget(const cslibs::Manufacturer &manufacturer,
                               const cslibs::Series &series,
                               QWidget *parent = nullptr);

  [[nodiscard]] virtual std::optional<csprofile::parameter::Media> GetMedia() const;

 Q_SIGNALS:
  void ZSelectionChanged(std::optional<csprofile::parameter::Media> media);

 protected:
  cslibs::Manufacturer manufacturer_;
  cslibs::Series series_;
  QTableView *table_;

  [[nodiscard]] virtual MediaSelectorModel *GetModel() const = 0;

 protected Q_SLOTS:
  void SSelectionChanged();
};

/**
 * Image media selector widget
 */
class ImageMediaSelectorWidget final : public MediaSelectorWidget {
 Q_OBJECT
 public:
  explicit ImageMediaSelectorWidget(const std::shared_ptr<cslibs::ImageDb> &db,
                                    const cslibs::Manufacturer &manufacturer,
                                    const cslibs::Series &series,
                                    QWidget *parent = nullptr);

 protected:
  [[nodiscard]] MediaSelectorModel *GetModel() const final {
    return model_;
  }

 private:
  ImageMediaSelectorModel *model_;
};

/**
 * Gel selector widget
 */
class GelSelectorWidget final : public MediaSelectorWidget {
 Q_OBJECT
 public:
  explicit GelSelectorWidget(const std::shared_ptr<cslibs::gel::GelDb> &db,
                             const cslibs::Manufacturer &manufacturer,
                             const cslibs::Series &series,
                             QWidget *parent = nullptr);

 protected:
  [[nodiscard]] MediaSelectorModel *GetModel() const final {
    return model_;
  }

 private:
  GelMediaSelectorModel *model_;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_MEDIASELECTORDIALOG_H_
