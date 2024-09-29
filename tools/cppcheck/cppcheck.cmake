set(CPPCHECK_NAME cppcheck)
set(CPPCHECK_DIR ${TOOLS_DIR}/cppcheck)
set(CPPCHECK_SEARCH_PATH
    ${CPPCHECK_DIR}/cppcheck/build/bin/
    /opt/cppcheck/build/bin/
)

find_program(${CPPCHECK_NAME}_BIN
    NAMES ${CPPCHECK_NAME}
    HINTS ${CPPCHECK_SEARCH_PATH}
)
if(${CPPCHECK_NAME}_BIN)
    message(STATUS "Found ${CPPCHECK_NAME} at: ${${CPPCHECK_NAME}_BIN}")
else()
    message(FATAL_ERROR "${CPPCHECK_NAME} not found.")
endif()

message(STATUS "Adding files to Cppcheck")
set(CPPCHECK_SOURCES ${TRAINS_SRCS})

add_custom_target(
    cppcheck
    ${${CPPCHECK_NAME}_BIN}
    --enable=all
    --check-level=exhaustive
    --suppress-xml=${CPPCHECK_DIR}/suppressions.xml
    --error-exitcode=2
    --xml
    --output-file=cppcheck_report.xml
    ${CPPCHECK_SOURCES}
)