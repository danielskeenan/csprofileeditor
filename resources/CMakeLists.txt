# Generate the third-party software notice
find_program(PANDOC_PATH NAMES pandoc)
if (NOT PANDOC_PATH)
    message(FATAL_ERROR "Could not find pandoc, required for building installers")
endif ()
message(STATUS "Creating third-party software notice")
list(APPEND CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party.md)
execute_process(COMMAND ${PANDOC_PATH}
    "--metadata" "pagetitle='Third-Party Software'" "--from" "commonmark" "--to" "html" "${CMAKE_CURRENT_SOURCE_DIR}/3rd-party.md"
    OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/3rd-party.html"
    ENCODING "UTF8"
    )
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/3rd-party.qrc"
    "<!DOCTYPE RCC>"
    "<RCC version='1.0'>"
    "<qresource><file>3rd-party.html</file></qresource>"
    "</RCC>"
    )

target_sources(${PROJECT_NAME} PRIVATE
    resources.qrc
    ${CMAKE_CURRENT_BINARY_DIR}/3rd-party.qrc
    )
