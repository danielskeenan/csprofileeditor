/**
 * @file Parameter.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_PARAMETER_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_PARAMETER_H_

#include <string>
#include <vector>
#include <optional>
#include "../ColorTable.h"
#include <nlohmann/json.hpp>
#include "Range.h"

namespace csprofile::parameter {

enum class Type {
  /** Invalid parameter */
  kNone = 0,
  /** Intensity parameter, mapped to channel fader on console */
  kIntensity = 1,
  /** Position parameter */
  kPosition = 2,
  /** Beam parameter, used for other misc parameters that don't fall into a different category */
  kBeam = 4,
  /** Color parameter */
  kColor = 5,
};

/**
 * Personality parameter
 */
class Parameter {
  friend void from_json(const nlohmann::json &json, std::unique_ptr<Parameter> &parameter);
  friend void to_json(nlohmann::json &json, const std::unique_ptr<Parameter> &parameter);

 public:
  enum class InvalidReason {
    kIsValid = 0,
    /** Missing name. */
    kMissingName,
    /** Home is outside of allowed values. */
    kHomeOutOfRange,
    /** Address is out of DMX range. */
    kOutOfDmxRange,
    /** Addresses overlap. */
    kOverlappingAddresses,
    /** One or more ranges is invalid. */
    kInvalidRange,
    /** Range is outside of allowed values (8-bit/16-bit). */
    kRangeOutOfRange,
  };

  [[nodiscard]] static std::unique_ptr<Parameter> CreateForType(Type type_id);
  [[nodiscard]] std::unique_ptr<Parameter> Clone();
  [[nodiscard]] std::unique_ptr<Parameter> ConvertTo(Type type_id);
  [[nodiscard]] InvalidReason IsInvalid() const;

  [[nodiscard]] virtual Type GetType() const {
    return Type::kNone;
  };

  [[nodiscard]] virtual bool AllowCustomName() const {
    return true;
  };

  [[nodiscard]] virtual std::vector<std::string> GetAllowedNames() const {
    return {};
  }

  [[nodiscard]] virtual bool IsAllowedName(const std::string &name) const {
    return true;
  }

  /**
   * Get the color to decorate this parameter with.
   * @return
   */
  [[nodiscard]] virtual std::optional<uint32_t> GetColor() const {
    return {};
  }

  virtual void FromJson(const nlohmann::json &json);

  [[nodiscard]] unsigned int GetAddressCourse() const {
    return address_course_;
  }

  void SetAddressCourse(unsigned int address_course) {
    address_course_ = address_course;
  }

  [[nodiscard]] unsigned int GetAddressFine() const {
    return address_fine_;
  }

  void SetAddressFine(unsigned int address_fine) {
    address_fine_ = address_fine;
  }

  [[nodiscard]] bool Is8Bit() const {
    return address_fine_ == 0;
  }

  [[nodiscard]] bool Is16Bit() const {
    return address_fine_ > 0;
  }

  [[nodiscard]] unsigned int GetHomeValue() const {
    return home_value_;
  }

  void SetHomeValue(unsigned int home_value) {
    home_value_ = home_value;
  }

  [[nodiscard]] virtual unsigned int GetHighlightValue() const {
    return 0;
  };

  [[nodiscard]] bool GetFadeWithIntensity() const {
    return fade_with_intensity_;
  }

  void SetFadeWithIntensity(bool fade_with_intensity) {
    fade_with_intensity_ = fade_with_intensity;
  }

  [[nodiscard]] bool GetInvert() const {
    return invert_;
  }

  void SetInvert(bool invert) {
    invert_ = invert;
  }

  [[nodiscard]] bool GetSnap() const {
    return snap_;
  }

  void SetSnap(bool snap) {
    snap_ = snap;
  }

  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  virtual void SetName(const std::string &name) {
    name_ = name;
  }

  bool operator==(const Parameter &rhs) const;
  bool operator!=(const Parameter &rhs) const;

  std::vector<Range> ranges_;

 protected:
  unsigned int address_course_ = 1;
  unsigned int address_fine_ = 0;
  unsigned int home_value_ = 0;
  bool fade_with_intensity_ = false;
  bool invert_ = false;
  bool snap_ = false;
  std::string name_;
};

/**
 * Intensity parameter
 */
class IntensityParameter final : public Parameter {
 public:
  IntensityParameter() {
    name_ = "Intensity";
    fade_with_intensity_ = true;
  }

  [[nodiscard]] Type GetType() const final {
    return Type::kIntensity;
  }

  [[nodiscard]] unsigned int GetHighlightValue() const final {
    return 65535;
  }
};

/**
 * Position parameter
 */
class PositionParameter final : public Parameter {
 public:
  [[nodiscard]] Type GetType() const final {
    return Type::kPosition;
  }

  [[nodiscard]] unsigned int GetHighlightValue() const final {
    return 0;
  }
};

/**
 * Beam parameter
 */
class BeamParameter final : public Parameter {
 public:
  [[nodiscard]] Type GetType() const final {
    return Type::kBeam;
  }

  [[nodiscard]] unsigned int GetHighlightValue() const final {
    return 0;
  }
};

/**
 * Color parameter
 */
class ColorParameter final : public Parameter {
 public:
  [[nodiscard]] Type GetType() const final {
    return Type::kColor;
  }

  [[nodiscard]] bool AllowCustomName() const final {
    return false;
  }

  [[nodiscard]] unsigned int GetHighlightValue() const final {
    return 0;
  }

  [[nodiscard]] std::optional<uint32_t> GetColor() const final;

  void FromJson(const nlohmann::json &json) final;

  void SetName(const std::string &name) final;

  [[nodiscard]] std::vector<std::string> GetAllowedNames() const final;

  [[nodiscard]] bool IsAllowedName(const std::string &name) const final;

  [[nodiscard]] const std::optional<ColorTable::Color> &GetColorParam() const {
    return color_param_;
  }

  void SetColorParam(const std::optional<ColorTable::Color> &color_param) {
    color_param_ = color_param;
    if (color_param_.has_value()) {
      name_ = ColorTable::GetColorName(color_param.value());
    } else {
      name_.clear();
    }
  }

  bool operator==(const ColorParameter &rhs) const;
  bool operator!=(const ColorParameter &rhs) const;

 private:
  std::optional<ColorTable::Color> color_param_;
};

} // csprofile::parameter

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_PARAMETER_PARAMETER_H_
