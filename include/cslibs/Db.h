/**
 * @file Db.h
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DB_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DB_H_

#include <SQLiteCpp/Database.h>
#include <functional>
#include "DefsFile.h"
#include "Entity.h"

namespace cslibs {

/**
 * Base class for databases
 */
class Db {
 public:
  /**
   * Callback function taking arguments:
   * - current step
   * - total steps
   */
  using ProgressCallback = std::function<void(unsigned long, unsigned long)>;

  /**
   * Open the database at @p db_path with defs from @p defs_path
   *
   * @param defs_path
   * @param db_path
   * @param allow_writing
   * @throws except::DbError When the database cannot be opened.
   * @throws except::DefsError When the defs file cannot be read.
   */
  explicit Db(std::string defs_path, const std::string &db_path, bool allow_writing = false);

  /**
   * Returns TRUE if ths database version matches the defs file version.
   *
   * @return
   */
  [[nodiscard]] bool UpToDate();

  /**
   * Initialize the database.
   *
   * @throws except::DefsError When the defs file cannot be parsed.
   */
  void Update(const ProgressCallback &progress_callback = {});

  /**
   * Clear the database
   */
  void Reset();

  [[nodiscard]] std::vector<Manufacturer> GetManufacturers();
  [[nodiscard]] std::vector<Series> GetSeriesForManufacturer(const Manufacturer &manufacturer);

 protected:
  std::string defs_file_path_;
  std::optional<SQLite::Database> db_;

  virtual void CreateTables() = 0;
  virtual void LoadFromDefsFile(const ProgressCallback &progress_callback) = 0;
  virtual void ClearRecords() = 0;
  void SetUserVersion(const DefsFile::Version &version);
  void CreateManufacturerSeriesTables();
  void Optimize();
  void ReOpen(const std::string &db_path, bool allow_writing);
  void UseFreshDatabase(const std::string &db_path, bool allow_writing);
  bool DatabaseIsReadOnly();

  using ManufacturerIdCache = std::unordered_map<std::string, unsigned int>;
  using SeriesIdCache = std::unordered_map<unsigned int, std::unordered_map<std::string, unsigned int>>;
  /**
   * Get the manufacturer id, storing the results in @p cache.
   * @param name
   * @param cache
   * @return
   */
  [[nodiscard]] unsigned int GetManufacturerIdForName(const std::string &name, ManufacturerIdCache &cache);
  /**
   * Get the series id, storing the results in @p cache.
   * @param name
   * @param manufacturer_id
   * @param cache
   * @return
   */
  [[nodiscard]] unsigned int GetSeriesIdForName(const std::string &name,
                                                unsigned int manufacturer_id,
                                                SeriesIdCache &cache);
};

/**
 * Databases that store images
 */
class ImageDb : public Db {
 public:
  enum class Sort {
    kName,
    kCode,
  };

  /**
   * Open the database at @p db_path with defs from @p defs_path
   *
   * @param defs_path
   * @param data_index_path *.idx file
   * @param data_path *.dat file
   * @param db_path
   * @param allow_writing
   * @throws except::DbError When the database cannot be opened.
   * @throws except::DefsError When the defs file cannot be read.
   */
  explicit ImageDb(std::string defs_path,
                   std::string data_index_path,
                   std::string data_path,
                   const std::string &db_path,
                   bool allow_writing = false);

  [[nodiscard]] virtual std::vector<ImageEntity> GetForSeries(const Series &series, Sort sort_by = Sort::kCode);

  /**
   * Get the image associated with the given @p dcid.
   * @param dcid
   * @return
   */
  [[nodiscard]] virtual std::optional<std::vector<char>> GetImageForDcid(const std::string &dcid);

 protected:
  void CreateTables() override;
  void ClearRecords() override;

 protected:
  std::string data_index_path_;
  std::string data_path_;

  [[nodiscard]] virtual const char *GetBaseTable() const = 0;
  virtual void CreateImageTable();
};

} // cslibs

#endif //CS_PROFILE_EDITOR_INCLUDE_CSLIBS_DB_H_
