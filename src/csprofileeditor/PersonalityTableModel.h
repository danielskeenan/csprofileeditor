/**
 * @file PersonalityTableModel.h
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYTABLEMODEL_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYTABLEMODEL_H_

#include <QAbstractTableModel>
#include <csprofile/Library.h>

namespace csprofileeditor {

/**
 * Model for Personality Table
 */
class PersonalityTableModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit PersonalityTableModel(std::shared_ptr<csprofile::Library> library, QObject *parent = nullptr);

  void SetLibrary(std::shared_ptr<csprofile::Library> library);
  bool UpdatePersonality(unsigned int row, const csprofile::Personality &personality);

  [[nodiscard]] int rowCount(const QModelIndex &parent) const final;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const final;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const final;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role) const final;
  bool setData(const QModelIndex &index, const QVariant &value, int role) final;
  [[nodiscard]]Qt::ItemFlags flags(const QModelIndex &index) const final;
  bool insertRows(int startRow, int count, const QModelIndex &parent) final;
  bool removeRows(int startRow, int count, const QModelIndex &parent) final;
  bool removeRows(std::vector<int> rows, const QModelIndex &parent);

  enum class Column {
    kManufacturer = 0,
    kModel,
    kMode,
    kFootprint,
  };

 private:
  std::shared_ptr<csprofile::Library> library_;

  static inline const auto kColumnCount = static_cast<unsigned int>(Column::kFootprint) + 1;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PERSONALITYTABLEMODEL_H_
