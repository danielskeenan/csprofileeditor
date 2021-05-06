/**
 * @file RangesTableModel.h
 *
 * @author dankeenan
 * @date 5/1/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESTABLEMODEL_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESTABLEMODEL_H_

#include <QAbstractTableModel>
#include <csprofile/parameter/Parameter.h>
#include <cslibs/disc/DiscDb.h>
#include <cslibs/effect/EffectDb.h>
#include <cslibs/gobo/GoboDb.h>
#include <QIcon>

namespace csprofileeditor {

/**
 * Model class for Ranges table
 */
class RangesTableModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit RangesTableModel(std::unique_ptr<csprofile::parameter::Parameter> &parameter, QObject *parent = nullptr);

  enum class Column {
    kBegin = 0,
    kEnd,
    kDefault,
    kLabel,
    kMedia,
  };
  static inline const auto kColumnCount = static_cast<unsigned int>(Column::kMedia) + 1;

  [[nodiscard]] int rowCount(const QModelIndex &parent) const final;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const final;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const final;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const final;
  bool setData(const QModelIndex &index, const QVariant &value, int role) final;
  Qt::ItemFlags flags(const QModelIndex &index) const final;
  bool insertRows(int startRow, int count, const QModelIndex &parent) final;
  bool removeRows(int startRow, int count, const QModelIndex &parent) final;
  bool removeRows(std::vector<int> rows, const QModelIndex &parent);
  bool SetMedia(const QModelIndex &index, const std::optional<csprofile::parameter::Media>& media);

 private:
  std::unique_ptr<csprofile::parameter::Parameter> &parameter_;
  std::unordered_map<std::string, QIcon> dcid_images_;
  std::shared_ptr<cslibs::disc::DiscDb> disc_db_;
  std::shared_ptr<cslibs::effect::EffectDb> effect_db_;
  std::shared_ptr<cslibs::gobo::GoboDb> gobo_db_;

  /**
   * Loads required images from the library and removes unused ones.
   */
  void UpdateImageCache();
  /**
   * Lookup the image that corresponds to the given @p dcid.
   * @param dcid
   * @return The icon, if found
   */
  std::optional<QIcon> GetImageForDcid(const std::string &dcid);
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_RANGESTABLEMODEL_H_
