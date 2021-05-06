/**
 * @file util.h
 *
 * @author dankeenan
 * @date 4/30/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_UTIL_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_UTIL_H_

#include <utility>

namespace csprofileeditor::util {

/**
 * Remove a list of rows from a model
 * @param rows
 * @param parent
 * @param model
 * @return
 */
inline bool remove_model_rows(std::vector<int> rows, const QModelIndex &parent, QAbstractItemModel *model) {
  if (rows.empty()) {
    return false;
  }

  // Sort the list backwards to remove starting at the end.
  std::stable_sort(rows.begin(), rows.end(), std::greater());
  int start_row = rows.front();
  int count = 0;
  for (const auto row : rows) {
    if (start_row - row > 1) {
      if (!model->removeRows(start_row, count, parent)) {
        return false;
      }
      start_row = row;
      count = 1;

    } else {
      start_row = row;
      ++count;
    }
  }
  if (count > 0) {
    if (!model->removeRows(start_row, count, parent)) {
      return false;
    }
  }

  return true;
}

} // csprofileeditor::util

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_UTIL_H_
