/**
 * @file Parameter.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/parameter/Parameter.h"
#include "csprofile/except.h"
#include "csprofile/logging.h"

namespace csprofile::parameter {

std::unique_ptr<Parameter> Parameter::CreateForType(Type type_id) {
  switch (type_id) {
    case Type::kNone:return std::make_unique<Parameter>();
    case Type::kIntensity:return std::make_unique<IntensityParameter>();
    case Type::kPosition:return std::make_unique<PositionParameter>();
    case Type::kBeam:return std::make_unique<BeamParameter>();
    case Type::kColor:return std::make_unique<ColorParameter>();
  }

  throw except::ParseError("Bad parameter type id");
}

std::unique_ptr<Parameter> Parameter::Clone() {
  switch (this->GetType()) {
    case Type::kNone:return std::make_unique<Parameter>(*dynamic_cast<Parameter *>(this));
    case Type::kIntensity:return std::make_unique<IntensityParameter>(*dynamic_cast<IntensityParameter *>(this));
    case Type::kPosition:return std::make_unique<PositionParameter>(*dynamic_cast<PositionParameter *>(this));
    case Type::kBeam:return std::make_unique<BeamParameter>(*dynamic_cast<BeamParameter *>(this));
    case Type::kColor:return std::make_unique<ColorParameter>(*dynamic_cast<ColorParameter *>(this));
  }
  logging::critical("Tried to clone invalid parameter type {}", this->GetType());
  assert(false);
}

std::unique_ptr<Parameter> Parameter::ConvertTo(Type type_id) {
  std::unique_ptr<Parameter> new_parameter = CreateForType(type_id);
  new_parameter->SetAddressCourse(address_course_);
  new_parameter->SetAddressFine(address_fine_);
  new_parameter->SetFadeWithIntensity(fade_with_intensity_);
  new_parameter->SetInvert(invert_);
  new_parameter->SetSnap(snap_);
  new_parameter->SetName(name_);
  new_parameter->ranges_ = ranges_;

  return new_parameter;
}

void from_json(const nlohmann::json &json, std::unique_ptr<Parameter> &parameter) {
  try {
    parameter = Parameter::CreateForType(static_cast<Type>(json.at("type").get<int>()));
    parameter->FromJson(json);
  } catch (const nlohmann::json::exception &e) {
    csprofile::logging::error("Missing parameter type id");
    throw except::ParseError("Missing parameter type id");
  }
}

void to_json(nlohmann::json &json, const std::unique_ptr<Parameter> &parameter) {
  // Addresses are zero-based
  json["coarse"] = parameter->GetAddressCourse() - 1;
  json["fadeWithIntensity"] = parameter->GetFadeWithIntensity();
  if (parameter->Is16Bit()) {
    json["fine"] = parameter->GetAddressFine() - 1;
  } else {
    json.erase("fine");
  }
  json["highlight"] = parameter->GetHighlightValue();
  json["home"] = parameter->GetHomeValue();
  json["invert"] = parameter->GetInvert();
  json["name"] = parameter->GetName();
  if (!parameter->ranges_.empty()) {
    json["ranges"] = parameter->ranges_;
  } else {
    json.erase("ranges");
  }
  json["size"] = parameter->Is16Bit() ? 16 : 8;
  json["snap"] = parameter->GetSnap();
  json["type"] = static_cast<unsigned int>(parameter->GetType());
}

void Parameter::FromJson(const nlohmann::json &json) {
  try {
    // Addresses are zero-based
    json.at("coarse").get_to(address_course_);
    ++address_course_;
    if (json.contains("fine") && json.at("size").get<int>() == 16) {
      json.at("fine").get_to(address_fine_);
      ++address_fine_;
    } else {
      address_fine_ = 0;
    }

    json.at("fadeWithIntensity").get_to(fade_with_intensity_);
    json.at("home").get_to(home_value_);
    json.at("invert").get_to(invert_);
    json.at("name").get_to(name_);
    json.at("snap").get_to(snap_);

    // Ranges
    if (json.contains("ranges")) {
      decltype(Parameter::ranges_) new_ranges;
      for (const auto &range_json : json.at("ranges")) {
        new_ranges.push_back(range_json.get<Range>());
      }
      ranges_ = std::move(new_ranges);
    }
  } catch (const nlohmann::json::exception &e) {
    csprofile::logging::error(fmt::format("Error parsing parameter: {}", e.what()));
    throw except::ParseError("Error parsing parameter");
  }
}

bool Parameter::operator==(const Parameter &rhs) const {
  return GetType() == rhs.GetType() &&
      address_course_ == rhs.address_course_ &&
      address_fine_ == rhs.address_fine_ &&
      home_value_ == rhs.home_value_ &&
      fade_with_intensity_ == rhs.fade_with_intensity_ &&
      invert_ == rhs.invert_ &&
      snap_ == rhs.snap_ &&
      name_ == rhs.name_ &&
      ranges_ == rhs.ranges_;
}

bool Parameter::operator!=(const Parameter &rhs) const {
  return !(rhs == *this);
}

std::optional<uint32_t> ColorParameter::GetColor() const {
  if (!color_param_.has_value()) {
    return {};
  }
  return ColorTable::GetColorRgb(color_param_.value());
}

void ColorParameter::FromJson(const nlohmann::json &json) {
  Parameter::FromJson(json);
  SetColorParam(ColorTable::GetColorFromName(name_));
}

bool ColorParameter::operator==(const ColorParameter &rhs) const {
  return static_cast<const csprofile::parameter::Parameter &>(*this)
      == static_cast<const csprofile::parameter::Parameter &>(rhs) &&
      color_param_ == rhs.color_param_;
}

bool ColorParameter::operator!=(const ColorParameter &rhs) const {
  return !(rhs == *this);
}

std::vector<std::string> ColorParameter::GetAllowedNames() const {
  std::vector<std::string> names;
  names.reserve(ColorTable::kColorCount);
  for (unsigned int color = 0; color < ColorTable::kColorCount; ++color) {
    names.push_back(ColorTable::GetColorName(static_cast<ColorTable::Color>(color)));
  }
  return names;
}

bool ColorParameter::IsAllowedName(const std::string &name) const {
  return ColorTable::GetColorFromName(name).has_value();
}

void ColorParameter::SetName(const std::string &name) {
  if (name.empty()) {
    color_param_.reset();
  } else {
    const auto color_param = ColorTable::GetColorFromName(name);
    if (color_param.has_value()) {
      name_ = name;
      color_param_ = color_param.value();
    }
  }
}

} // csprofile::parameter
