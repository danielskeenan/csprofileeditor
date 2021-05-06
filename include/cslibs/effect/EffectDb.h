/**
 * @file EffectDb.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EFFECT_EFFECTDB_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EFFECT_EFFECTDB_H_

#include "../Db.h"
#include <SQLiteCpp/Column.h>
#include <cslibs/Entity.h>

namespace cslibs::effect {

/**
 * Effect database
 */
class EffectDb final : public ImageDb {
 public:
  using ImageDb::ImageDb;

 protected:
  [[nodiscard]] const char *GetBaseTable() const override {
    return "effect";
  }

  void LoadFromDefsFile(const ProgressCallback &progress_callback) final;
};

}

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EFFECT_EFFECTDB_H_
