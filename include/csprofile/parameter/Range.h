/**
 * @file Range.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_RANGE_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_RANGE_H_

#include <nlohmann/json.hpp>
#include "Media.h"

namespace csprofile::parameter {

/**
 * Selectable range
 */
class Range final {
  friend void from_json(const nlohmann::json &json, Range &range);
  friend void to_json(nlohmann::json &json, const Range &range);

 public:
  explicit Range() = default;

  explicit Range(unsigned int begin_value, unsigned int end_value, unsigned int default_value) :
      begin_value_(begin_value), end_value_(end_value), default_value_(default_value) {}

  [[nodiscard]] unsigned int GetBeginValue() const {
    return begin_value_;
  }

  void SetBeginValue(unsigned int begin_value) {
    begin_value_ = begin_value;
  }

  [[nodiscard]] unsigned int GetEndValue() const {
    return end_value_;
  }

  void SetEndValue(unsigned int end_value) {
    end_value_ = end_value;
  }

  [[nodiscard]] unsigned int GetDefaultValue() const {
    return default_value_;
  }

  void SetDefaultValue(unsigned int default_value) {
    default_value_ = default_value;
  }

  [[nodiscard]] const std::string &GetLabel() const {
    return label_;
  }

  void SetLabel(const std::string &label) {
    label_ = label;
  }

  [[nodiscard]] const std::optional<Media> &GetMedia() const {
    return media_;
  }

  void SetMedia(const std::optional<Media> &media) {
    media_ = media;
  }

  bool operator==(const Range &rhs) const;
  bool operator!=(const Range &rhs) const;

 private:
  unsigned int begin_value_ = 0;
  unsigned int end_value_ = 0;
  unsigned int default_value_ = 0;
  std::string label_;
  std::optional<Media> media_;
};

} // csprofile::parameter

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_RANGE_H_
