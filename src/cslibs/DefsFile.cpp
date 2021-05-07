/**
 * @file DefsFile.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "cslibs/DefsFile.h"
#include "cslibs/except.h"
#include <regex>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using boost::algorithm::trim_right;

namespace cslibs {

bool DefsFile::Version::operator==(const DefsFile::Version &rhs) const {
  return major == rhs.major &&
      minor == rhs.minor &&
      patch == rhs.patch;
}

bool DefsFile::Version::operator!=(const DefsFile::Version &rhs) const {
  return !(rhs == *this);
}

bool DefsFile::Version::operator<(const DefsFile::Version &rhs) const {
  if (major < rhs.major)
    return true;
  if (rhs.major < major)
    return false;
  if (minor < rhs.minor)
    return true;
  if (rhs.minor < minor)
    return false;
  return patch < rhs.patch;
}

bool DefsFile::Version::operator>(const DefsFile::Version &rhs) const {
  return rhs < *this;
}

bool DefsFile::Version::operator<=(const DefsFile::Version &rhs) const {
  return !(rhs < *this);
}

bool DefsFile::Version::operator>=(const DefsFile::Version &rhs) const {
  return !(*this < rhs);
}

DefsFile::DefsFile(const std::string &file_path) : file_stream_(file_path, std::ifstream::in | std::ifstream::binary) {
  // Validate
  if (!file_stream_.is_open() || file_stream_.fail()) {
    throw except::DefsError("Could not open file");
  }
  // Never skip whitespace
  file_stream_.unsetf(std::ifstream::skipws);
  std::string line;
  bool valid = false;
  while (std::getline(file_stream_, line)) {
    if (line.substr(0, 6) != "IDENT ") {
      continue;
    }
    valid = line.substr(6, 3) == kIdent;
    break;
  }
  if (!valid) {
    throw except::DefsError("File is not valid");
  }
  file_stream_.seekg(0);

  // Load version
  while (std::getline(file_stream_, line)) {
    if (line.substr(0, 17) != "$CARALLONVERSION ") {
      continue;
    }
    const std::regex version_re(R"(^\$CARALLONVERSION (\d+).(\d+).(\d+)\s?$)", std::regex::ECMAScript);
    std::smatch version_match;
    if (!std::regex_match(line, version_match, version_re)) {
      throw except::DefsError("Version is malformed");
    }
    version_.major = std::stoul(version_match[1]);
    version_.minor = std::stoul(version_match[2]);
    version_.patch = std::stoul(version_match[3]);
    break;
  }
}

std::optional<DefsFile::Def> DefsFile::GetNextRecord() {
  std::optional<Def> record;
  if (!file_stream_) {
    // Already at EOF, do nothing
    return {};
  }

  // Find record start
  std::string line;
  static const std::regex record_name_re(R"(^\$(\w+)\s?$)", std::regex::ECMAScript);
  while (std::getline(file_stream_, line)) {
    std::smatch record_name_match;
    if (!std::regex_match(line, record_name_match, record_name_re)) {
      continue;
    }
    record.emplace(record_name_match[1]);
    break;
  }
  if (!record.has_value()) {
    // Advanced to EOF without finding a new record
    return record;
  }

  // Load contents
  static const std::regex record_content_re(R"(^\$\$(\w+) (.+)\s?$)", std::regex::ECMAScript);
  while (std::getline(file_stream_, line)) {
    std::smatch record_content_match;
    if (std::regex_match(line, record_content_match, record_content_re)) {
      record->contents.insert({record_content_match[1], record_content_match[2]});
    } else if (std::regex_match(line, record_name_re)) {
      // At next record
      // Seek stream to beginning of the next record
      file_stream_.seekg(line.size() * -1 - 1, std::ifstream::cur);
      break;
    }
  }

  return record;
}

ImageDefsFile::ImageDefsFile(const std::string &file_path,
                             const std::string &data_index_path,
                             const std::string &data_path)
    : DefsFile(file_path),
      data_index_stream_(data_index_path, std::ifstream::in | std::ifstream::binary),
      data_stream_(data_path, std::ifstream::in | std::ifstream::binary) {
  // Validate
  if (!data_index_stream_.is_open() || data_index_stream_.fail()) {
    throw except::DefsError("Could not open data index");
  } else if (!data_stream_.is_open() || data_stream_.fail()) {
    throw except::DefsError("Could not open data file");
  }
  // Never skip whitespace
  data_index_stream_.unsetf(std::ifstream::skipws);
}

std::optional<std::vector<char>> ImageDefsFile::GetDataForDcid(const std::string &type, const std::string &dcid) {
  if (type_dcid_offsets_.find(type) == type_dcid_offsets_.end()) {
    LoadOffsetsForType(type);
  }
  DataPosition data_position;
  try {
    data_position = type_dcid_offsets_.at(type).at(dcid);
  } catch (const std::out_of_range &) {
    return {};
  }

  // Each data chunk has an 80 byte header
  data_stream_.seekg(data_position.offset + 80);
  int size = data_position.size - 80;
  if (size < 0) {
    // Size is current position until EOF.  Need to get an actual value so buffer can be created correctly.
    const auto old_pos = data_stream_.tellg();
    data_stream_.seekg(0, std::ifstream::end);
    size = static_cast<int>(data_stream_.tellg() - old_pos);
    data_stream_.seekg(old_pos);
  }
  std::vector<char> data(size, 0);
  data_stream_.read(data.data(), static_cast<long>(data.size()));
  data.resize(data_stream_.gcount());
  return data;
}

void ImageDefsFile::LoadOffsetsForType(const std::string &type) {
  data_index_stream_.seekg(0);
  std::string line;
  auto &dcid_offsets = type_dcid_offsets_[type];
  dcid_offsets.clear();

  // The size is not set in the line, so need to use the next line.  This complicates things a bit.
  bool started = false;
  std::string line_type;
  std::string dcid;
  unsigned int offset = 0;
  std::vector<std::string> parts;
  while (std::getline(data_index_stream_, line)) {
    trim_right(line);
    if (line.empty()) {
      continue;
    }
    split(parts, line, is_any_of(","));
    if (parts.size() != 4) {
      throw except::DefsError("Data index is malformed");
    }
    if (!started) {
      // First pass
      dcid = parts.at(0);
      offset = std::stoul(parts.at(1));
      line_type = parts.at(2);
      started = true;
    } else {
      const unsigned int next_offset = std::stoul(parts.at(1));
      if (line_type == type) {
        dcid_offsets.insert({dcid, {offset, static_cast<int>(next_offset - offset)}});
      }
      line_type = parts.at(2);
      dcid = parts.at(0);
      offset = next_offset;
    }
  }
  // Insert the last line, using -1 to mean "until EOF"
  if (line_type == type) {
    dcid_offsets.insert({dcid, {offset, -1}});
  }
}

unsigned long DefsFile::GetSize() {
  if (!size_.has_value()) {
    const auto old_pos = file_stream_.tellg();
    file_stream_.seekg(0, std::ifstream::end);
    size_ = file_stream_.tellg();
    file_stream_.seekg(old_pos);
  }
  return *size_;
}

unsigned long DefsFile::GetPosition() {
  // End of file means the stream has failed, so position indicators stop working.
  if (file_stream_.eof()) {
    return GetSize();
  } else {
    return file_stream_.tellg();
  }
}

} // cslibs
