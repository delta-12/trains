set(UNCRUSTIFY_NAME uncrustify)
set(UNCRUSTIFY_DIR ${TOOLS_DIR}/uncrustify)
set(UNCRUSTIFY_SEARCH_PATH
    ${UNCRUSTIFY_DIR}/uncrustify/build
)

find_program(${UNCRUSTIFY_NAME}_BIN
    NAMES ${UNCRUSTIFY_NAME}
    HINTS ${UNCRUSTIFY_SEARCH_PATH}
)
if(${UNCRUSTIFY_NAME}_BIN)
    message(STATUS "Found ${UNCRUSTIFY_NAME} at: ${${UNCRUSTIFY_NAME}_BIN}")
else()
    message(FATAL_ERROR "${UNCRUSTIFY_NAME} not found.")
endif()

message(STATUS "Adding files to Uncrustify")
set(UNCRUSTIFY_SOURCES ${TRAINS_SRCS})
foreach(INC_DIR ${INC_DIRS})
    file(GLOB INC_DIR_H_SRCS LIST_DIRECTORIES false CONFIGURE_DEPENDS
        "${INC_DIR}/*.h"
    )
    list(APPEND UNCRUSTIFY_SOURCES ${INC_DIR_H_SRCS})
endforeach()

add_custom_target(
    uncrustify
    ${${UNCRUSTIFY_NAME}_BIN}
    -c ${UNCRUSTIFY_DIR}/uncrustify.cfg
    --no-backup
    ${UNCRUSTIFY_SOURCES}
)

add_custom_target(
    uncrustify-check
    ${${UNCRUSTIFY_NAME}_BIN}
    -c ${UNCRUSTIFY_DIR}/uncrustify.cfg
    -L 1
    --check
    ${UNCRUSTIFY_SOURCES}
    2> uncrustify-report.txt
)