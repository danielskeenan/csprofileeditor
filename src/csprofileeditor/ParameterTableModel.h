/**
 * @file ParameterTableModel.h
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTABLEMODEL_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTABLEMODEL_H_

#include <QAbstractTableModel>
#include <csprofile/Personality.h>

namespace csprofileeditor {

/**
 * Model for Parameter table
 */
class ParameterTableModel : public QAbstractTableModel {
 Q_OBJECT
 public:
  explicit ParameterTableModel(csprofile::Personality &personality, QObject *parent = nullptr);

  [[nodiscard]] QStringList GetAllowedNames(const QModelIndex &index) const;
  [[nodiscard]] const std::unique_ptr<csprofile::parameter::Parameter> &GetParameter(const QModelIndex &index) const;
  bool SetRanges(const QModelIndex &index, const std::vector<csprofile::parameter::Range> &ranges);

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
    kType = 0,
    kName,
    kCoarse,
    kFine,
    kHome,
    kInvert,
    kRanges,
  };

 private:
  csprofile::Personality &personality_;

  static inline const auto kColumnCount = static_cast<unsigned int>(Column::kRanges) + 1;
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_PARAMETERTABLEMODEL_H_
