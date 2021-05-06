/**
 * @file except.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_EXCEPT_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_EXCEPT_H_

namespace csprofile::except {

/**
 * Thrown when the library file cannot be parsed
 */
class ParseError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

} // csprofile::except

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_EXCEPT_H_
