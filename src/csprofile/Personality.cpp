/**
 * @file Personality.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/Personality.h"
#include "csprofile/except.h"
#include "csprofile/logging.h"
#include "csprofile/parameter/Parameter.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <algorithm>

using boost::algorithm::to_upper;

namespace csprofile {

void from_json(const nlohmann::json &json, Personality &personality) {
  try {
    // Metadata
    json.at("manufacturerName").get_to(personality.manufacturer_name_);
    json.at("modeName").get_to(personality.mode_name_);
    // The console uses a single hyphen to represent empty modes
    if (personality.mode_name_ == "-") {
      personality.mode_name_.clear();
    }
    json.at("modelName").get_to(personality.model_name_);

    // Parameters
    decltype(Personality::parameters_) new_parameters;
    for (const auto &parameter_json : json.at("parameters")) {
      new_parameters.push_back(parameter_json.get<std::unique_ptr<parameter::Parameter>>());
    }
    personality.parameters_ = std::move(new_parameters);
  } catch (const nlohmann::json::exception &e) {
    csprofile::logging::error(fmt::format("Error loading personality: {}", e.what()));
    throw csprofile::except::ParseError("Error loading personality");
  }
}

void to_json(nlohmann::json &json, const Personality &personality) {
  // Color table
  const std::optional<ColorTable::ColorMixingType> color_table = personality.GetColorMixingType();
  if (color_table.has_value()) {
    json["colortable"] = ColorTable::GetColorTableUuid(color_table.value());
  }

  json["dcid"] = personality.GetDcid();
  json["hasIntensity"] = std::find_if(personality.parameters_.cbegin(), personality.parameters_.cend(),
                                      [](const std::unique_ptr<parameter::Parameter> &parameter) {
                                        return parameter->GetType() == parameter::Type::kIntensity;
                                      }) != personality.parameters_.cend();
  json["manufacturerName"] = personality.manufacturer_name_.empty() ? "Custom" : personality.manufacturer_name_;
  // Addresses are zero-based
  json["maxOffset"] = personality.GetFootprint() - 1;
  json["modeName"] = personality.mode_name_.empty() ? "-" : personality.mode_name_;
  json["modelName"] = personality.model_name_.empty() ? "-" : personality.model_name_;
  json["parameters"] = personality.parameters_;
}

unsigned int Personality::GetFootprint() const {
  if (parameters_.empty()) {
    return 0;
  }
  unsigned int min_address = UINT32_MAX;
  unsigned int max_address = 0;
  for (const auto &parameter : parameters_) {
    min_address = std::min(min_address,
                           parameter->Is16Bit() ? std::min(parameter->GetAddressCourse(), parameter->GetAddressFine())
                                                : parameter->GetAddressCourse());
    max_address = std::max(max_address,
                           parameter->Is16Bit() ? std::max(parameter->GetAddressCourse(), parameter->GetAddressFine())
                                                : parameter->GetAddressCourse());
  }
  return max_address - min_address + 1;
}

Personality::Personality() : dcid_(uuids::uuid_random_generator{}()) {
}

Personality::Personality(const std::string &dcid) : dcid_(uuids::uuid::from_string(dcid)) {
}

Personality::Personality(const Personality &other) :
    dcid_(other.dcid_),
    manufacturer_name_(other.manufacturer_name_),
    model_name_(other.model_name_),
    mode_name_(other.mode_name_) {
  for (const auto &parameter : other.parameters_) {
    parameters_.push_back(parameter->Clone());
  }
}

Personality &Personality::operator=(const Personality &other) {
  dcid_ = other.dcid_;
  manufacturer_name_ = other.manufacturer_name_;
  model_name_ = other.model_name_;
  mode_name_ = other.mode_name_;
  parameters_.resize(other.parameters_.size());
  for (unsigned int i = 0; i < other.parameters_.size(); ++i) {
    parameters_[i] = other.parameters_.at(i)->Clone();
  }
  return *this;
}

Personality::InvalidReason Personality::IsInvalid() const {
  if (manufacturer_name_.empty()) {
    return InvalidReason::kMissingManufacturerName;
  } else if (model_name_.empty()) {
    return InvalidReason::kMissingModelName;
  } else if (parameters_.empty()) {
    return InvalidReason::kNoParameters;
  }
  for (const auto &parameter : parameters_) {
    if (parameter->IsInvalid() != parameter::Parameter::InvalidReason::kIsValid) {
      return InvalidReason::kInvalidParameter;
    }
  }

  return InvalidReason::kIsValid;
}

std::string Personality::GetDcid() const {
  // Convert to upper case to match official formatting
  std::string dcid_str = uuids::to_string(dcid_);
  to_upper(dcid_str);
  return dcid_str;
}

unsigned int Personality::GetNextAddress() const {
  unsigned int next = 0;
  for (const auto &parameter : parameters_) {
    next = std::max(next, std::max(parameter->GetAddressCourse(), parameter->GetAddressFine()));
  }
  return next + 1;
}

std::optional<ColorTable::ColorMixingType> Personality::GetColorMixingType() const {
  // Find color parameters
  ColorTable::ColorList colors;
  for (const auto &parameter : parameters_) {
    if (parameter->GetType() != parameter::Type::kColor) {
      continue;
    }
    const std::optional<ColorTable::Color> &color_param =
        dynamic_cast<parameter::ColorParameter *>(parameter.get())->GetColorParam();
    if (color_param.has_value()) {
      colors.push_back(color_param.value());
    }
  }

  if (colors.empty()) {
    return {};
  }
  return ColorTable::GetColorMixingType(colors);
}

bool Personality::operator==(const Personality &rhs) const {
  return dcid_ == rhs.dcid_ &&
      manufacturer_name_ == rhs.manufacturer_name_ &&
      model_name_ == rhs.model_name_ &&
      mode_name_ == rhs.mode_name_ &&
      // Compare more than just pointers
      parameters_.size() == rhs.parameters_.size() &&
      std::equal(parameters_.cbegin(), parameters_.cend(), rhs.parameters_.cbegin(), rhs.parameters_.cend(),
                 [](const std::unique_ptr<parameter::Parameter> &a, const std::unique_ptr<parameter::Parameter> &b) {
                   return *a == *b;
                 });
}

bool Personality::operator!=(const Personality &rhs) const {
  return !(rhs == *this);
}

} // csprofile
