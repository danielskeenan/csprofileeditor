/**
 * @file Library.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/Library.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <fmt/chrono.h>
#include "csprofile/logging.h"
#include "csprofile/Personality.h"
#include "csprofile/except.h"
#include "csprofileeditor_config.h"

namespace csprofile {

Library::Library(const std::string &file_path) {
  logging::info("Opening from {}", file_path);
  std::ifstream file(file_path);
  if (!file.is_open() || file.fail()) {
    throw std::runtime_error("Failed to open file for reading");
  }
  file >> *this;
  file.close();
}

void Library::Save(const std::string &file_path) const {
  logging::info("Saving to {}", file_path);
  std::ofstream file(file_path);
  if (!file.is_open() || file.fail()) {
    throw std::runtime_error("Failed to open file for writing");
  }
  file << *this;
  file.close();
}

std::istream &operator>>(std::istream &in, csprofile::Library &library) {
  nlohmann::json json;
  try {
    in >> json;
  } catch (const nlohmann::json::parse_error &e) {
    csprofile::logging::error(fmt::format("Error parsing file (invalid JSON): {}", e.what()));
    throw csprofile::except::ParseError("Error parsing file");
  }

  if (!json.contains("personalities")) {
    csprofile::logging::error("Error parsing file: Missing personalities key");
    throw csprofile::except::ParseError("Missing personalities key");
  }

  decltype(csprofile::Library::personalities) new_personalities;
  for (const auto &personality_json : json.at("personalities")) {
    new_personalities.push_back(personality_json.get<csprofile::Personality>());
  }
  // This won't happen if inner parse function throw exceptions
  library.personalities = std::move(new_personalities);

  return in;
}

std::ostream &operator<<(std::ostream &out, const Library &library) {
  nlohmann::json json;

  // Always uses UTC time
  // Allow an override here to permit consistent testing
  json["date"] = fmt::format("{}Z", boost::posix_time::to_iso_extended_string(library.updated_.has_value()
                                                                              ? library.updated_.value()
                                                                              : boost::posix_time::second_clock::universal_time()));
  json["editorVersion"] = csprofileeditor::config::kEtcEditorCompat;
  json["personalities"] = library.personalities;

  out << std::setw(4) << json;

  return out;
}

bool Library::operator==(const Library &rhs) const {
  return personalities == rhs.personalities;
}

bool Library::operator!=(const Library &rhs) const {
  return !(rhs == *this);
}

} // csprofile
