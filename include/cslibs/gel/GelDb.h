/**
 * @file GelDb.h
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GELDB_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GELDB_H_

#include "../Db.h"
#include "Gel.h"
#include <SQLiteCpp/Column.h>

namespace cslibs::gel {

class GelDb final : public Db {
 public:
  using Db::Db;

  enum class Sort {
    kName,
    kCode,
    kColor,
  };

  [[nodiscard]] std::vector<Gel> GetGelForSeries(const Series &series, Sort sort_by = Sort::kCode);

 protected:
  void CreateTables() final;
  void LoadFromDefsFile(const ProgressCallback &progress_callback) final;
  void ClearRecords() final;
};

} // cslibs::gel

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GELDB_H_
