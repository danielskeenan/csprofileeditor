configure_file(config.h.in ${PROJECT_BINARY_DIR}/include/csprofileeditor_config.h)
include_directories(${PROJECT_BINARY_DIR}/include)

add_subdirectory(cslibs)
add_subdirectory(csprofile)
add_subdirectory(csprofileeditor)
