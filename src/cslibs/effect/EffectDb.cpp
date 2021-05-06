/**
 * @file EffectDb.cpp
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "cslibs/effect/EffectDb.h"
#include <cslibs/except.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <fmt/format.h>

using boost::algorithm::split;
using boost::algorithm::is_any_of;

namespace cslibs::effect {

void EffectDb::LoadFromDefsFile(const ProgressCallback &progress_callback) {
  ImageDefsFile defs_file(defs_file_path_, data_index_path_, data_path_);

  // Cache stored ids
  ManufacturerIdCache manufacturer_ids;
  SeriesIdCache series_ids;
  SQLite::Statement insert_stmt(*db_, fmt::format(R"EOF(
    INSERT INTO {base_table}(dcid, series_id, code, name, image)
    VALUES (:dcid, :series_id, :code, :name, :image);
  )EOF", fmt::arg("base_table", GetBaseTable())));

  while (auto record = defs_file.GetNextRecord()) {
    if (record->record_name != "FXGLASS") {
      continue;
    }

    // Extract data from record
    std::string dcid;
    try {
      dcid = record->contents.at("IMAGE");
    } catch (const std::out_of_range &) {
      // Gobos without images are not helpful.
      continue;
    }
    std::optional<std::vector<char>> image_data = defs_file.GetDataForDcid("effect", dcid);
    if (!image_data.has_value()) {
      // Gobos without images are not helpful.
      continue;
    }
    std::vector<std::string> series_info;
    try {
      split(series_info, record->contents.at("FXGLASSMANUFACTURER"), is_any_of(","));
    } catch (const std::out_of_range &) {
      throw except::DefsError(fmt::format("{} Missing FXGLASSMANUFACTURER", dcid));
    }
    if (series_info.size() != 2) {
      throw except::DefsError(fmt::format("{} FXGLASSMANUFACTURER is malformed: \"{}\"",
                                          dcid,
                                          record->contents.at("FXGLASSMANUFACTURER")));
    }
    const std::string manufacturer_name = series_info.at(0);
    const std::string series_name = series_info.at(1);
    std::vector<std::string> effect_info;
    try {
      split(effect_info, record->contents.at("FXGLASSINFO"), is_any_of(","));
    } catch (const std::out_of_range &) {
      throw except::DefsError(fmt::format("{} Missing FXGLASSINFO", dcid));
    }
    if (effect_info.size() < 2) {
      throw except::DefsError(fmt::format("{} FXGLASSINFO is malformed: \"{}\"",
                                          dcid,
                                          record->contents.at("FXGLASSINFO")));
    }
    const std::string code = effect_info.at(0);
    const std::string name = fmt::format("{}", fmt::join(effect_info.cbegin() + 1, effect_info.cend(), ","));

    // Get foreign keys
    const unsigned int manufacturer_id = GetManufacturerIdForName(manufacturer_name, manufacturer_ids);
    const unsigned int series_id = GetSeriesIdForName(series_name, manufacturer_id, series_ids);

    // Add effect
    try {
      insert_stmt.reset();
      insert_stmt.bind(":dcid", dcid);
      insert_stmt.bind(":series_id", series_id);
      insert_stmt.bind(":code", code);
      insert_stmt.bind(":name", name);
      insert_stmt.bind(":image", image_data->data(), static_cast<int>(image_data->size()));
      insert_stmt.exec();
    } catch (const SQLite::Exception &e) {
      throw except::DbError(fmt::format("{} Error adding effect: {}", dcid, e.what()));
    }
    if (progress_callback) {
      progress_callback(defs_file.GetPosition(), defs_file.GetSize());
    }
  }

  SetUserVersion(defs_file.GetVersion());
}

} // cslibs::effect
