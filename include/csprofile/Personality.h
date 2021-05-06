/**
 * @file Personality.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PERSONALITY_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PERSONALITY_H_

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <uuid.h>
#include "parameter/Parameter.h"

namespace csprofile {

/**
 * Fixture personality
 */
class Personality final {
  friend void from_json(const nlohmann::json &json, Personality &personality);
  friend void to_json(nlohmann::json &json, const Personality &personality);

 public:
  explicit Personality();
  explicit Personality(const std::string &dcid);
  Personality(const Personality &other);
  Personality &operator=(const Personality &other);

  [[nodiscard]] std::string GetDcid() const;

  [[nodiscard]] const std::string &GetManufacturerName() const {
    return manufacturer_name_;
  }

  void SetManufacturerName(const std::string &manufacturer_name) {
    manufacturer_name_ = manufacturer_name;
  }

  [[nodiscard]] const std::string &GetModelName() const {
    return model_name_;
  }

  void SetModelName(const std::string &model_name) {
    model_name_ = model_name;
  }

  [[nodiscard]] const std::string &GetModeName() const {
    return mode_name_;
  }

  void SetModeName(const std::string &mode_name) {
    mode_name_ = mode_name;
  }

  [[nodiscard]] unsigned int GetFootprint() const;

  [[nodiscard]] unsigned int GetNextAddress() const;

  [[nodiscard]] std::optional<ColorTable::ColorMixingType> GetColorMixingType() const;

  bool operator==(const Personality &rhs) const;
  bool operator!=(const Personality &rhs) const;

  std::vector<std::unique_ptr<parameter::Parameter>> parameters_;

 private:
  uuids::uuid dcid_;
  std::string manufacturer_name_ = "Custom";
  std::string model_name_;
  std::string mode_name_;
};

} // csprofile

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PERSONALITY_H_
