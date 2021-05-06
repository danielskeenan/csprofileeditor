/**
 * @file GoboDb.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GOBO_GOBODB_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GOBO_GOBODB_H_

#include "../Db.h"
#include <SQLiteCpp/Column.h>
#include <cslibs/Entity.h>

namespace cslibs::gobo {

/**
 * Gobo database
 */
class GoboDb final : public ImageDb {
 public:
  using ImageDb::ImageDb;

 protected:
  [[nodiscard]] const char *GetBaseTable() const override {
    return "gobo";
  }

  void LoadFromDefsFile(const ProgressCallback &progress_callback) final;
};

} // cslibs::gel

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GOBO_GOBODB_H_
