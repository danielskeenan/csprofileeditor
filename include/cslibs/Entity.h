/**
 * @file Entity.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_ENTITY_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_ENTITY_H_

#include <string>
#include <iostream>

namespace cslibs {

class Manufacturer {
  friend std::ostream &operator<<(std::ostream &out, const Manufacturer &manufacturer) {
    out << "<Manufacturer " << manufacturer.id_ << ": " << manufacturer.name_ << ">";
    return out;
  }

 public:
  explicit Manufacturer(unsigned int id, std::string name) : id_(id), name_(std::move(name)) {}

  [[nodiscard]] unsigned int GetId() const {
    return id_;
  }

  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  bool operator==(const Manufacturer &rhs) const {
    return id_ == rhs.id_ &&
        name_ == rhs.name_;
  }

  bool operator!=(const Manufacturer &rhs) const {
    return !(rhs == *this);
  }

 private:
  unsigned int id_;
  std::string name_;
};

class Series {
  friend std::ostream &operator<<(std::ostream &out, const Series &series) {
    out << "<Series " << series.id_ << ": " << series.name_ << ">";
    return out;
  }

 public:
  explicit Series(unsigned int id, std::string name) :
      id_(id), name_(std::move(name)) {}

  [[nodiscard]] unsigned int GetId() const {
    return id_;
  }

  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  bool operator==(const Series &rhs) const {
    return id_ == rhs.id_ &&
        name_ == rhs.name_;
  }

  bool operator!=(const Series &rhs) const {
    return !(rhs == *this);
  }

 private:
  unsigned int id_;
  std::string name_;
};

class ImageEntity {
  friend std::ostream &operator<<(std::ostream &out, const ImageEntity &gobo) {
    out << "<ImageEntity " << gobo.dcid_ << ": " << gobo.code_ << ", " << gobo.name_ << ", (" << gobo.image_.size()
        << "-byte image)>";
    return out;
  }

 public:
  explicit ImageEntity(std::string dcid,
                std::string code,
                std::string name,
                std::vector<char> image) :
      dcid_(std::move(dcid)), code_(std::move(code)), name_(std::move(name)), image_(std::move(image)) {}

  explicit ImageEntity(std::string dcid,
                std::string code,
                std::string name,
                const void *image_data,
                unsigned int image_size) :
      dcid_(std::move(dcid)), code_(std::move(code)), name_(std::move(name)), image_(image_size, 0) {
    memcpy(image_.data(), image_data, image_.size());
  }

  [[nodiscard]] const std::string &GetDcid() const {
    return dcid_;
  }

  [[nodiscard]] const std::string &GetCode() const {
    return code_;
  }

  [[nodiscard]] const std::string &GetName() const {
    return name_;
  }

  [[nodiscard]] const std::vector<char> &GetImage() const {
    return image_;
  }

  bool operator==(const ImageEntity &rhs) const {
    return dcid_ == rhs.dcid_ &&
        code_ == rhs.code_ &&
        name_ == rhs.name_ &&
        image_ == rhs.image_;
  }

  bool operator!=(const ImageEntity &rhs) const {
    return !(rhs == *this);
  }

 private:
  std::string dcid_;
  std::string code_;
  std::string name_;
  std::vector<char> image_;
};

} // cslibs

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_ENTITY_H_
