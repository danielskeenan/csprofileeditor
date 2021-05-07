/**
 * @file DefsFile.h
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DEFSFILE_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DEFSFILE_H_

#include <iterator>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <optional>

namespace cslibs {

/**
 * Defs file bundled with official editor
 */
class DefsFile {
 public:
  /**
   * File version
   */
  struct Version {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;

    explicit Version(unsigned int major_part = 0, unsigned int minor_part = 0, unsigned int patch_part = 0) :
        major(major_part), minor(minor_part), patch(patch_part) {}

    bool operator==(const Version &rhs) const;
    bool operator!=(const Version &rhs) const;
    bool operator<(const Version &rhs) const;
    bool operator>(const Version &rhs) const;
    bool operator<=(const Version &rhs) const;
    bool operator>=(const Version &rhs) const;

    friend std::ostream &operator<<(std::ostream &out, const Version &version) {
      out << version.major << "." << version.minor << "." << version.patch;
      return out;
    }
  };

  struct Def {
    explicit Def(std::string name) : record_name(std::move(name)) {}

    std::string record_name;
    std::unordered_map<std::string, std::string> contents;

    bool operator==(const Def &rhs) const {
      return record_name == rhs.record_name &&
          contents == rhs.contents;
    }

    bool operator!=(const Def &rhs) const {
      return !(rhs == *this);
    }
  };

  explicit DefsFile(const std::string &file_path);

  /**
   * Get data version.
   * @return
   */
  [[nodiscard]] const Version &GetVersion() const {
    return version_;
  }

  /**
   * Fetch the next record in the file, or none if no records remain.
   * @return
   */
  [[nodiscard]] std::optional<Def> GetNextRecord();

  [[nodiscard]] unsigned long GetSize();
  [[nodiscard]] unsigned long GetPosition();

 protected:
  std::ifstream file_stream_;
  Version version_;
  std::optional<unsigned long> size_;
  static inline const auto kIdent = "3:0";
};

/**
 * Defs file that uses image data
 */
class ImageDefsFile : public DefsFile {
 public:

  explicit ImageDefsFile(const std::string &file_path,
                         const std::string &data_index_path,
                         const std::string &data_path);

  /**
   * Get the data for the given type and dcid, or none if there is no data.
   *
   * @param type
   * @param dcid
   * @return
   */
  [[nodiscard]] std::optional<std::vector<char>> GetDataForDcid(const std::string &type, const std::string &dcid);

 private:
  struct DataPosition {
    unsigned int offset = 0;
    /** -1 == until EOF */
    int size = -1;
  };

  std::ifstream data_index_stream_;
  std::ifstream data_stream_;
  /** Cache: Media type > dcid > offset in data file */
  std::unordered_map<std::string, std::unordered_map<std::string, DataPosition>> type_dcid_offsets_;

  void LoadOffsetsForType(const std::string &type);
};

} // cslibs

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DEFSFILE_H_
