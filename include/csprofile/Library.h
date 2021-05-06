/**
 * @file Library.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LIBRARY_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LIBRARY_H_

#include <vector>
#include <stdexcept>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Personality.h"

namespace csprofile {

/**
 * A collection of personalities
 */
class Library final {
  friend std::istream &operator>>(std::istream &in, Library &library);
  friend std::ostream &operator<<(std::ostream &out, const Library &library);

 public:
  /**
   * Create a new library
   */
  explicit Library() = default;

  /**
   * Load a library file.
   *
   * @throws std::runtime_error when the file cannot be read.
   * @throws except::ParseError when the library file is not valid.
   */
  explicit Library(const std::string &file_path);

  /**
   * Save a library path.
   *
   * @throws std::runtime_error when the file cannot be written.
   */
  void Save(const std::string &file_path) const;

  /**
   * @internal
   * Set a specific updated time.
   *
   * Used for testing to allow for consistent output.
   *
   * @param updated
   */
  void SetUpdated(const boost::posix_time::ptime &updated) {
    updated_ = updated;
  }

  bool operator==(const Library &rhs) const;
  bool operator!=(const Library &rhs) const;

  std::vector<Personality> personalities;

 private:
  std::optional<boost::posix_time::ptime> updated_;
};

} // csprofile

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LIBRARY_H_
