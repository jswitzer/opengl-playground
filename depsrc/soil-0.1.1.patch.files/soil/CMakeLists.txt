project (soil)

cmake_minimum_required (VERSION 3.6)
set (SOIL_LIB_NAME SOIL)
set (SOIL_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set (SOIL_PUBLIC_HEADERS_FILES ${SOIL_DIR}/include/SOIL/SOIL.h)
set (SOIL_SRC_FILES ${SOIL_DIR}/src/image_dxt.c ${SOIL_DIR}/src/image_helper.c ${SOIL_DIR}/src/soil.c ${SOIL_DIR}/src/stb_image_aug.c)

include_directories (${SOIL_DIR}/include)
add_library (soil_s STATIC ${SOIL_PUBLIC_HEADERS_FILES} ${SOIL_SRC_FILES})
set_target_properties (soil_s PROPERTIES COMPILE_DEFINITIONS "SOIL_STATIC" OUTPUT_NAME "${SOIL_LIB_NAME}" PREFIX lib)
