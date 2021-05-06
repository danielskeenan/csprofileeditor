/**
 * @file Db.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "cslibs/Db.h"
#include "cslibs/except.h"
#include <utility>
#include <fmt/format.h>
#include <csprofileeditor_config.h>
#include <sqlite3.h>
#include <filesystem>
#include <boost/algorithm/string/predicate.hpp>

using boost::algorithm::ilexicographical_compare;

namespace cslibs {

Db::Db(std::string defs_path, const std::string &db_path, bool allow_writing)
    : defs_file_path_(std::move(defs_path)) {
  // Open the database
  // Lots of error handling because failure can stop the application completely.
  try {
    db_.emplace(db_path, allow_writing ? (SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) : SQLite::OPEN_READONLY);
  } catch (const SQLite::Exception &e) {
    // Try opening/closing in read only to fix the WAL log.
    try {
      ReOpen(db_path, allow_writing);
    } catch (const SQLite::Exception &e) {
      // Database is corrupt.  Remove file and start over.
      UseFreshDatabase(db_path, allow_writing);
    }
  }
  if (allow_writing && DatabaseIsReadOnly()) {
    // Db was not opened properly.  Try to close and re-open again.
    ReOpen(db_path, allow_writing);
    if (DatabaseIsReadOnly()) {
      // Database is corrupt.  Remove file and start over.
      UseFreshDatabase(db_path, allow_writing);
    }
  }
  // Use WAL mode
  db_->exec("PRAGMA journal_mode=WAL;");

  const unsigned int application_id = db_->execAndGet("PRAGMA application_id;").getUInt();
  if (application_id == 0 && allow_writing) {
    // New database; set the application id
    db_->exec(fmt::format("PRAGMA application_id = {};", csprofileeditor::config::kApplicationId));
  } else if (application_id != csprofileeditor::config::kApplicationId) {
    throw except::DbError("Not a library database");
  }
}

bool Db::UpToDate() {
  const unsigned int user_version = db_->execAndGet("PRAGMA user_version;").getUInt();
  const DefsFile defs_file(defs_file_path_);
  const DefsFile::Version defs_version = defs_file.GetVersion();
  return defs_version.major == ((user_version & (0xFF << 16)) >> 16)
      && defs_version.minor == ((user_version & (0xFF << 8)) >> 8)
      && defs_version.patch == ((user_version & (0xFF << 0)) >> 0);
}

void Db::SetUserVersion(const DefsFile::Version &version) {
  if (DatabaseIsReadOnly()) {
    throw except::ReadOnlyDbError();
  }
  const unsigned int user_version = (version.major << 16) | (version.minor << 8) | (version.patch << 0);
  db_->exec(fmt::format("PRAGMA user_version = {};", user_version));
}

void Db::Update(const ProgressCallback &progress_callback) {
  if (DatabaseIsReadOnly()) {
    throw except::ReadOnlyDbError();
  }
  CreateManufacturerSeriesTables();
  this->CreateTables();
  Reset();
  this->LoadFromDefsFile(progress_callback);
  Optimize();
}

void Db::Reset() {
  this->ClearRecords();
  db_->exec(R"EOF(
    DELETE FROM series;
    DELETE FROM manufacturer;
  )EOF");
}

std::vector<Manufacturer> Db::GetManufacturers() {
  SQLite::Statement q(*db_, R"EOF(
    SELECT id, name
    FROM manufacturer
    ORDER BY name ASC;
  )EOF");
  std::vector<Manufacturer> results;
  while (q.executeStep()) {
    results.emplace_back(
        q.getColumn("id").getUInt(),
        q.getColumn("name").getString()
    );
  }
  return results;
}

std::vector<Series> Db::GetSeriesForManufacturer(const Manufacturer &manufacturer) {
  SQLite::Statement q(*db_, R"EOF(
    SELECT id, name
    FROM series
    WHERE manufacturer_id = :manufacturer_id
    ORDER BY name ASC;
  )EOF");
  q.bind(":manufacturer_id", manufacturer.GetId());
  std::vector<Series> results;
  while (q.executeStep()) {
    results.emplace_back(
        q.getColumn("id").getUInt(),
        q.getColumn("name").getString()
    );
  }
  return results;
}

void Db::CreateManufacturerSeriesTables() {
  db_->exec(R"EOF(
    --- Manufacturer
    CREATE TABLE IF NOT EXISTS manufacturer
    (
        id   INTEGER
            PRIMARY KEY,
        name TEXT NOT NULL
    );

    CREATE INDEX IF NOT EXISTS manufacturer_name_index
        ON manufacturer (name);

    --- Series
    CREATE TABLE IF NOT EXISTS series
    (
        id              INTEGER
            PRIMARY KEY,
        manufacturer_id INTEGER NOT NULL
            REFERENCES manufacturer
                ON DELETE CASCADE,
        name            TEXT    NOT NULL
    );

    CREATE INDEX IF NOT EXISTS series_manufacturer_id_index
        ON series (manufacturer_id);

    CREATE INDEX IF NOT EXISTS series_name_index
        ON series (name);
  )EOF");
}

void Db::Optimize() {
  db_->exec("PRAGMA OPTIMIZE; VACUUM;");
}

void Db::ReOpen(const std::string &db_path, bool allow_writing) {
  db_.reset();
  db_.emplace(db_path, SQLite::OPEN_READONLY);
  db_.reset();
  db_.emplace(db_path, allow_writing ? (SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) : SQLite::OPEN_READONLY);
}

void Db::UseFreshDatabase(const std::string &db_path, bool allow_writing) {
  db_.reset();
  for (const auto &suffix : {"-journal", "-wal", "-shm", ""}) {
    std::filesystem::remove(fmt::format("{}{}", db_path, suffix));
  }
  db_.emplace(db_path, allow_writing ? (SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) : SQLite::OPEN_READONLY);
}

bool Db::DatabaseIsReadOnly() {
  return sqlite3_db_readonly(db_->getHandle(), "main") != 0;
}

unsigned int Db::GetManufacturerIdForName(const std::string &name, ManufacturerIdCache &cache) {
  SQLite::Statement manufacturer_get_q(*db_, "SELECT id FROM manufacturer WHERE name = :name;");
  SQLite::Statement manufacturer_insert_q(*db_, "INSERT INTO manufacturer(name) VALUES (:name);");

  unsigned int manufacturer_id;
  try {
    manufacturer_id = cache.at(name);
  } catch (const std::out_of_range &) {
    // Fetch from the database
    try {
      manufacturer_get_q.reset();
      manufacturer_get_q.bind(":name", name);
      manufacturer_get_q.executeStep();
    } catch (const SQLite::Exception &e) {
      throw except::DbError(fmt::format("Error fetching manufacturer id: {}", e.what()));
    }
    if (manufacturer_get_q.hasRow()) {
      // Db has manufacturer
      manufacturer_id = manufacturer_get_q.getColumn("id").getUInt();
    } else {
      // Db doesn't have manufacturer
      try {
        manufacturer_insert_q.reset();
        manufacturer_insert_q.bind(":name", name);
        manufacturer_insert_q.exec();
      } catch (const SQLite::Exception &e) {
        throw except::DbError(fmt::format("Error adding manufacturer id: {}", e.what()));
      }
      manufacturer_id = db_->getLastInsertRowid();
    }
    cache.insert({name, manufacturer_id});
  }

  return manufacturer_id;
}

unsigned int Db::GetSeriesIdForName(const std::string &name, unsigned int manufacturer_id, SeriesIdCache &cache) {
  SQLite::Statement series_get_q
      (*db_, "SELECT id FROM series WHERE manufacturer_id = :manufacturer_id AND name = :name;");
  SQLite::Statement series_insert_q
      (*db_, "INSERT INTO series(manufacturer_id, name) VALUES (:manufacturer_id, :name);");

  unsigned int series_id;
  try {
    series_id = cache.at(manufacturer_id).at(name);
  } catch (const std::out_of_range &) {
    // Fetch from the database
    try {
      series_get_q.reset();
      series_get_q.bind(":manufacturer_id", manufacturer_id);
      series_get_q.bind(":name", name);
      series_get_q.executeStep();
    } catch (const SQLite::Exception &e) {
      throw except::DbError(fmt::format("Error fetching series id: {}", e.what()));
    }
    if (series_get_q.hasRow()) {
      // Db has series
      series_id = series_get_q.getColumn("id").getUInt();
    } else {
      // Db doesn't have series
      try {
        series_insert_q.reset();
        series_insert_q.bind(":manufacturer_id", manufacturer_id);
        series_insert_q.bind(":name", name);
        series_insert_q.exec();
      } catch (const SQLite::Exception &e) {
        throw except::DbError(fmt::format("Error adding series id: {}", e.what()));
      }
      series_id = db_->getLastInsertRowid();
    }
    cache[manufacturer_id].insert({name, series_id});
  }

  return series_id;
}

ImageDb::ImageDb(std::string defs_path,
                 std::string data_index_path,
                 std::string data_path,
                 const std::string &db_path,
                 bool allow_writing) : Db(std::move(defs_path), db_path, allow_writing),
                                       data_index_path_(std::move(data_index_path)),
                                       data_path_(std::move(data_path)) {
}

void ImageDb::CreateTables() {
  CreateImageTable();
}

void ImageDb::CreateImageTable() {
  db_->exec(fmt::format(R"EOF(
    CREATE TABLE IF NOT EXISTS {base_table}
    (
        id        INTEGER
            PRIMARY KEY,
        dcid      TEXT    NOT NULL,
        series_id INTEGER NOT NULL
            REFERENCES series
                ON DELETE CASCADE,
        code      TEXT    NOT NULL,
        name      TEXT    NOT NULL,
        image     BLOB    NOT NULL
    );

    CREATE INDEX IF NOT EXISTS {base_table}_name_index
        ON {base_table} (name);

    CREATE INDEX IF NOT EXISTS {base_table}_series_id_code_index
        ON {base_table} (series_id, code);

    CREATE INDEX IF NOT EXISTS {base_table}_series_id_index
        ON {base_table} (series_id);
  )EOF", fmt::arg("base_table", GetBaseTable())));
}

void ImageDb::ClearRecords() {
  db_->exec(fmt::format("DELETE FROM {base_table};", fmt::arg("base_table", GetBaseTable())));
}

std::vector<ImageEntity> ImageDb::GetForSeries(const Series &series, ImageDb::Sort sort_by) {
  std::string order_by;
  std::function<bool(const ImageEntity &, const ImageEntity &)> comp;
  switch (sort_by) {
    case Sort::kName:order_by = "name ASC";
      comp = [](const ImageEntity &a, const ImageEntity &b) {
        return ilexicographical_compare(a.GetName(), b.GetName());
      };
      break;
    case Sort::kCode:order_by = "code ASC";
      comp = [](const ImageEntity &a, const ImageEntity &b) {
        return ilexicographical_compare(a.GetCode(), b.GetCode());
      };
      break;
  }

  SQLite::Statement q(*db_, fmt::format(R"EOF(
    SELECT dcid,
           code,
           name,
           image
    FROM {base_table}
    WHERE {base_table}.series_id = :series_id
    ORDER BY {order_by};
  )EOF", fmt::arg("base_table", GetBaseTable()), fmt::arg("order_by", order_by)));
  q.bind(":series_id", series.GetId());
  std::vector<ImageEntity> results;
  while (q.executeStep()) {
    results.emplace_back(
        q.getColumn("dcid").getString(),
        q.getColumn("code").getString(),
        q.getColumn("name").getString(),
        q.getColumn("image").getBlob(),
        q.getColumn("image").size()
    );
  }
  if (comp) {
    std::sort(results.begin(), results.end(), comp);
  }
  return results;
}

std::optional<std::vector<char>> ImageDb::GetImageForDcid(const std::string &dcid) {
  SQLite::Statement q(*db_, fmt::format(R"EOF(
    SELECT image
    FROM {base_table}
    WHERE dcid = :dcid
    LIMIT 1;
  )EOF", fmt::arg("base_table", GetBaseTable())));
  q.bind(":dcid", dcid);
  q.executeStep();
  if (!q.hasRow()) {
    return {};
  }
  const auto image_size = q.getColumn(0).size();
  std::vector<char> result(image_size, 0);
  memcpy(result.data(), q.getColumn(0).getBlob(), result.size());

  return result;
}

} // cslibs
