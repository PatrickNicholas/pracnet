
include_directories("${PROJECT_BINARY_DIR}")

configure_file(
    "${PROJECT_SOURCE_DIR}/include/network/config.h.inc"
    "${PROJECT_BINARY_DIR}/network/config.h"
)