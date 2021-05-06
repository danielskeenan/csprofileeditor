/**
 * @file except.h
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EXCEPT_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EXCEPT_H_

#include <stdexcept>

namespace cslibs::except {

/**
 * Thrown on database errors.
 */
class DbError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

/**
 * Thrown when writing to a read-only database.
 */
class ReadOnlyDbError : public DbError {
 public:
  explicit ReadOnlyDbError() : DbError("Cannot perform a write operation on a read-only database") {}
};

/**
 * Thrown on errors reading defs files.
 */
class DefsError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

} // cslibs::except

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_EXCEPT_H_
