/**
 * @file logging.cpp
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "csprofile/logging.h"
#include "csprofileeditor_config.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace csprofile::logging {

void init_logging(spdlog::level::level_enum level) {
  std::vector<spdlog::sink_ptr> sinks;

  // Console sink - always create this logger
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(level);
  sinks.push_back(console_sink);

  // Register global logger
  auto logger = std::make_shared<spdlog::logger>(csprofileeditor::config::kProjectName, sinks.begin(), sinks.end());
  spdlog::register_logger(logger);
}

} // csprofile::logging
