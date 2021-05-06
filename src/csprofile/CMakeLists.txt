add_library(csprofile
    ColorTable.cpp
    Library.cpp
    logging.cpp
    Personality.cpp
    )
add_subdirectory(parameter)

find_package(Boost COMPONENTS date_time REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(stduuid REQUIRED)
target_link_libraries(csprofile PUBLIC
    Boost::date_time
    fmt::fmt
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    stduuid::stduuid
    )
