
find_program(CLANG_FORMAT clang-format DOC "Clang Format executable")

if( CLANG_FORMAT )
    file(GLOB_RECURSE FORMAT_SRC_FILES
            "${PROJECT_SOURCE_DIR}/include/**.h"
            "${PROJECT_SOURCE_DIR}/src/**.cpp"
            "${PROJECT_SOURCE_DIR}/test/**.h"
            "${PROJECT_SOURCE_DIR}/test/**.cpp"
            )

    add_custom_target(clang-format COMMAND ${CLANG_FORMAT} -i ${FORMAT_SRC_FILES})
endif()

