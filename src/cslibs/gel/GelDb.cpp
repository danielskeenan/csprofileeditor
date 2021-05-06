/**
 * @file GelDb.cpp
 *
 * @author dankeenan
 * @date 4/25/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "cslibs/DefsFile.h"
#include "cslibs/gel/GelDb.h"
#include <SQLiteCpp/Statement.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <fmt/format.h>
#include "cslibs/except.h"

using boost::algorithm::split;
using boost::algorithm::is_any_of;
using boost::algorithm::ilexicographical_compare;

namespace cslibs::gel {

void GelDb::CreateTables() {
  db_->exec(R"EOF(
    CREATE TABLE IF NOT EXISTS gel
    (
        id        INTEGER
            PRIMARY KEY,
        dcid      TEXT    NOT NULL,
        series_id INTEGER NOT NULL
            REFERENCES series
                ON DELETE CASCADE,
        code      TEXT    NOT NULL,
        name      TEXT    NOT NULL,
        red       INTEGER NOT NULL,
        green     INTEGER NOT NULL,
        blue      INTEGER NOT NULL
    );

    CREATE INDEX IF NOT EXISTS gel_name_index
        ON gel (name);

    -- Allow lookup of selected gel using info saved in the profile
    CREATE INDEX IF NOT EXISTS gel_name_red_green_blue_index
        ON gel (name, red, green, blue);

    CREATE INDEX IF NOT EXISTS gel_red_green_blue_index
        ON gel (red, green, blue);

    CREATE INDEX IF NOT EXISTS gel_series_id_code_index
        ON gel (series_id, code);

    CREATE INDEX IF NOT EXISTS gel_series_id_index
        ON gel (series_id);
  )EOF");
}

void GelDb::ClearRecords() {
  db_->exec("DELETE FROM gel;");
}

void GelDb::LoadFromDefsFile(const ProgressCallback &progress_callback) {
  DefsFile defs_file(defs_file_path_);

  // Cache stored ids
  ManufacturerIdCache manufacturer_ids;
  SeriesIdCache series_ids;
  SQLite::Statement gel_insert_q(*db_, R"EOF(
    INSERT INTO gel(dcid, series_id, code, name, red, green, blue)
    VALUES (:dcid, :series_id, :code, :name, :red, :green, :blue);
  )EOF");

  while (auto record = defs_file.GetNextRecord()) {
    if (record->record_name != "GEL") {
      continue;
    }

    // Extract data from record
    std::string dcid;
    try {
      dcid = record->contents.at("DCID");
    } catch (const std::out_of_range &) {
      throw except::DefsError("Missing DCID");
    }
    std::vector<std::string> series_info;
    try {
      split(series_info, record->contents.at("GELMANUFACTURER"), is_any_of(","));
    } catch (const std::out_of_range &) {
      throw except::DefsError(fmt::format("{} Missing GELMANUFACTURER", dcid));
    }
    if (series_info.size() != 2) {
      throw except::DefsError(fmt::format("{} GELMANUFACTURER is malformed: \"{}\"",
                                          dcid,
                                          record->contents.at("GELMANUFACTURER")));
    }
    const std::string manufacturer_name = series_info.at(0);
    const std::string series_name = series_info.at(1);
    std::vector<std::string> gel_info;
    try {
      split(gel_info, record->contents.at("GELINFO"), is_any_of(","));
    } catch (const std::out_of_range &) {
      throw except::DefsError(fmt::format("{} Missing GELINFO", dcid));
    }
    if (gel_info.size() == 2) {
      // No swatch, not worth adding
      continue;
    } else if (gel_info.size() < 5) {
      throw except::DefsError(fmt::format("{} GELINFO is malformed: \"{}\"",
                                          dcid,
                                          record->contents.at("GELINFO")));
    }
    const std::string code = gel_info.at(0);
    // Accommodate commas in the name: The color values are always the last three values in the info string.
    const std::string name =
        fmt::format("{}", fmt::join(gel_info.cbegin() + 1, gel_info.cbegin() + (gel_info.size() - 3), ","));
    const uint8_t red = std::stoul(gel_info.at(gel_info.size() - 3));
    const uint8_t green = std::stoul(gel_info.at(gel_info.size() - 2));
    const uint8_t blue = std::stoul(gel_info.at(gel_info.size() - 1));

    // Get foreign keys
    const unsigned int manufacturer_id = GetManufacturerIdForName(manufacturer_name, manufacturer_ids);
    const unsigned int series_id = GetSeriesIdForName(series_name, manufacturer_id, series_ids);

    // Add gel
    try {
      gel_insert_q.reset();
      gel_insert_q.bind(":dcid", dcid);
      gel_insert_q.bind(":series_id", series_id);
      gel_insert_q.bind(":code", code);
      gel_insert_q.bind(":name", name);
      gel_insert_q.bind(":red", red);
      gel_insert_q.bind(":green", green);
      gel_insert_q.bind(":blue", blue);
      gel_insert_q.exec();
    } catch (const SQLite::Exception &e) {
      throw except::DbError(fmt::format("{} Error adding gel: {}", dcid, e.what()));
    }
    if (progress_callback) {
      progress_callback(defs_file.GetPosition(), defs_file.GetSize());
    }
  }

  SetUserVersion(defs_file.GetVersion());
}

std::vector<Gel> GelDb::GetGelForSeries(const Series &series, Sort sort_by) {
  std::string order_by;
  std::function<bool(const Gel &, const Gel &)> comp;
  switch (sort_by) {
    case Sort::kName:order_by = "name ASC";
      comp = [](const Gel &a, const Gel &b) {
        return ilexicographical_compare(a.GetName(), b.GetName());
      };
      break;
    case Sort::kCode:order_by = "code ASC";
      comp = [](const Gel &a, const Gel &b) {
        return ilexicographical_compare(a.GetCode(), b.GetCode());
      };
      break;
    case Sort::kColor:order_by = "red ASC, green ASC, blue ASC";
      break;
  }

  SQLite::Statement q(*db_, fmt::format(R"EOF(
    SELECT dcid,
           code,
           name,
           red,
           green,
           blue
    FROM gel
    WHERE gel.series_id = :series_id
    ORDER BY {};
  )EOF", order_by));
  q.bind(":series_id", series.GetId());
  std::vector<Gel> results;
  while (q.executeStep()) {
    const uint8_t red = q.getColumn("red").getUInt();
    const uint8_t green = q.getColumn("green").getUInt();
    const uint8_t blue = q.getColumn("blue").getUInt();
    const uint32_t argb =
        (0xFF << 24) // Alpha
            | (red << 16)
            | (green << 8)
            | (blue << 0);
    results.emplace_back(
        q.getColumn("dcid").getString(),
        q.getColumn("code").getString(),
        q.getColumn("name").getString(),
        argb
    );
  }
  if (comp) {
    std::sort(results.begin(), results.end(), comp);
  }
  return results;
}

} // cslibs::gel
