/**
 * @file DiscDb.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DISC_DISCDB_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DISC_DISCDB_H_

#include "../Db.h"
#include <SQLiteCpp/Column.h>
#include <cslibs/Entity.h>

namespace cslibs::disc {

/**
 * Disc database
 */
class DiscDb final : public ImageDb {
 public:
  using ImageDb::ImageDb;

 protected:
  [[nodiscard]] const char *GetBaseTable() const final {
    return "disc";
  }

  void LoadFromDefsFile(const ProgressCallback &progress_callback) final;
};

} // cslibs::disc

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DISC_DISCDB_H_
