/**
 * @file Gel.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GEL_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GEL_H_

#include <iostream>
#include "../Entity.h"

namespace cslibs::gel {

class Gel {
  friend std::ostream &operator<<(std::ostream &out, const Gel &gel) {
    out << "<Gel " << gel.dcid_ << ": " << gel.code_ << ", " << gel.name_ << ">";
    return out;
  }

 public:
  explicit Gel(std::string dcid,
               std::string code,
               std::string name,
               unsigned int argb) :
      dcid_(std::move(dcid)), code_(std::move(code)), name_(std::move(name)), argb_(argb) {}

  [[nodiscard]] const std::string &GetDcid() const {
    return dcid_;
  }

  [[nodiscard]] const std::string &GetCode() const {
    return code_;
  }

  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  [[nodiscard]] unsigned int GetArgb() const {
    return argb_;
  }

  bool operator==(const Gel &rhs) const {
    return dcid_ == rhs.dcid_ &&
        code_ == rhs.code_ &&
        name_ == rhs.name_ &&
        argb_ == rhs.argb_;
  }

  bool operator!=(const Gel &rhs) const {
    return !(rhs == *this);
  }

 private:
  std::string dcid_;
  std::string code_;
  std::string name_;
  unsigned int argb_;
};

} // cslibs::gel

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_GEL_GEL_H_
