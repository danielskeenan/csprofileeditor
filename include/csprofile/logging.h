/**
 * @file logging.h
 *
 * @author dankeenan
 * @date 4/24/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LOGGING_H_
#define CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LOGGING_H_

#include <spdlog/spdlog.h>

namespace csprofile::logging {

// Re-export these functions in our own namespace
using spdlog::trace, spdlog::debug, spdlog::info, spdlog::warn, spdlog::error, spdlog::critical;

/**
 * Start logging
 *
 * @param level
 */
void init_logging(spdlog::level::level_enum level);

} // csprofile::logging

#endif //CS_PROFILE_EDITOR_INCLUDE_CSPROFILE_LOGGING_H_
