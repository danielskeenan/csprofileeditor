/**
 * @file Media.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/parameter/Media.h"
#include "csprofile/logging.h"
#include "csprofile/except.h"

namespace csprofile::parameter {

void from_json(const nlohmann::json &json, Media &media) {
  try {
    if (json.contains("r") && json.contains("g") && json.contains("b")) {
      const auto r = json.at("r").get<uint8_t>();
      const auto g = json.at("g").get<uint8_t>();
      const auto b = json.at("b").get<uint8_t>();
      const uint8_t a = 0xFF;
      media.rgb_ = (a << 24) | (r << 16) | (g << 8) | (b << 0);
    }
    if (json.contains("dcid")) {
      media.gobo_dcid_ = json.at("dcid").get<std::string>();
    }
    if (!media.rgb_.has_value() && !media.gobo_dcid_.has_value()) {
      // No gel/gobo info defined
      logging::error("Media has neither gel nor gobo data");
      throw except::ParseError("Media missing data");
    }
    json.at("name").get_to(media.name_);
  } catch (const nlohmann::json::exception &e) {
    logging::error(fmt::format("Error parsing media: {}", e.what()));
    throw except::ParseError("Error parsing media");
  }
}

void to_json(nlohmann::json &json, const Media &media) {
  uint8_t r, g, b;
  if (media.rgb_.has_value()) {
    r = (media.rgb_.value() & (0xFF << 16)) >> 16;
    g = (media.rgb_.value() & (0xFF << 8)) >> 8;
    b = (media.rgb_.value() & (0xFF << 0)) >> 0;
  }

  // Store values in the same order as the official editor
  if (media.gobo_dcid_.has_value()) {
    json["dcid"] = media.gobo_dcid_.value();
  }
  if (media.rgb_.has_value()) {
    json["b"] = b;
    json["g"] = g;
  }
  json["name"] = media.name_;
  if (media.rgb_.has_value()) {
    json["r"] = r;
  }
}

bool Media::operator==(const Media &rhs) const {
  return name_ == rhs.name_ &&
      rgb_ == rhs.rgb_ &&
      gobo_dcid_ == rhs.gobo_dcid_;
}

bool Media::operator!=(const Media &rhs) const {
  return !(rhs == *this);
}

} // csprofile::parameter
