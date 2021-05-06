add_library(cslibs
    Db.cpp
    DefsFile.cpp
    )
add_subdirectory(disc)
add_subdirectory(effect)
add_subdirectory(gel)
add_subdirectory(gobo)

find_package(Boost COMPONENTS headers REQUIRED)
find_package(fmt REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(stduuid REQUIRED)
target_link_libraries(cslibs PUBLIC
    Boost::headers
    fmt::fmt
    SQLiteCpp::SQLiteCpp
    stduuid::stduuid
    )
