/**
 * @file Media.h
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_MEDIA_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_MEDIA_H_

#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace csprofile::parameter {

/**
 * Range media (i.e. Gel/Gobo)
 */
class Media {
  friend void from_json(const nlohmann::json &json, Media &media);
  friend void to_json(nlohmann::json &json, const Media &media);

 public:
  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  void SetName(const std::string &name) {
    name_ = name;
  }

  /**
   * Get the color RGB values, packed as AARRGGBB.
   * @return
   */
  [[nodiscard]] std::optional<uint32_t> GetRgb() const {
    return rgb_;
  }

  void SetRgb(const std::optional<uint32_t> &rgb) {
    rgb_ = rgb;
  }

  void SetRgb(uint8_t r, uint8_t g, uint8_t b) {
    const uint8_t a = 0xFF;
    rgb_ = (a << 24) | (r << 16) | (g << 8) | (b << 0);
  }

  /**
   * Get the DCID for the gobo.
   * @return
   */
  [[nodiscard]] std::optional<std::string> GetGoboDcid() const {
    return gobo_dcid_;
  }

  void SetGoboDcid(const std::optional<std::string> &gobo_dcid) {
    gobo_dcid_ = gobo_dcid;
  }

  bool operator==(const Media &rhs) const;
  bool operator!=(const Media &rhs) const;

 private:
  std::string name_;
  std::optional<uint32_t> rgb_;
  std::optional<std::string> gobo_dcid_;
};

} // csprofile::parameter

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_MEDIA_H_
