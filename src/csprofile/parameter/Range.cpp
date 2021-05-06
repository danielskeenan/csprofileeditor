/**
 * @file Range.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/parameter/Range.h"
#include "csprofile/logging.h"
#include "csprofile/except.h"

namespace csprofile::parameter {

void from_json(const nlohmann::json &json, Range &range) {
  try {
    json.at("begin").get_to(range.begin_value_);
    json.at("default").get_to(range.default_value_);
    json.at("end").get_to(range.end_value_);
    json.at("label").get_to(range.label_);
    if (json.contains("media")) {
      range.media_ = json.at("media").get<Media>();
    }
  } catch (const nlohmann::json::exception &e) {
    logging::error(fmt::format("Error parsing range: {}", e.what()));
    throw except::ParseError("Error parsing range");
  }
}

void to_json(nlohmann::json &json, const Range &range) {
  json["begin"] = range.begin_value_;
  json["default"] = range.default_value_;
  json["end"] = range.end_value_;
  json["label"] = range.label_;
  if (range.media_.has_value()) {
    json["media"] = range.media_.value();
  }
}

Range::InvalidReason Range::IsInvalid() const {
  if (label_.empty()) {
    return InvalidReason::kMissingLabel;
  } else if (end_value_ < begin_value_) {
    return InvalidReason::kEndBeforeBegin;
  } else if (default_value_ < begin_value_ || default_value_ > end_value_) {
    return InvalidReason::kDefaultOutOfRange;
  } else if (begin_value_ > 65535 || end_value_ > 65535 || default_value_ > 65535) {
    return InvalidReason::kOutOfDmxRange;
  }
  return InvalidReason::kIsValid;
}

bool Range::Is16Bit() const {
  return begin_value_ > 512 || end_value_ > 512 || default_value_ > 512;
}

bool Range::Is8Bit() const {
  return !Is16Bit();
}

bool Range::operator==(const Range &rhs) const {
  return begin_value_ == rhs.begin_value_ &&
      end_value_ == rhs.end_value_ &&
      default_value_ == rhs.default_value_ &&
      label_ == rhs.label_ &&
      media_ == rhs.media_;
}

bool Range::operator!=(const Range &rhs) const {
  return !(rhs == *this);
}

} // csprofile::parameter
