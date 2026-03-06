cmake_minimum_required(VERSION 3.10)
project(door-security-daemon C)

# --- 1. Global Options & Functions ---
option(ENABLE_COVERAGE "Enable coverage instrumentation" OFF)
option(BUILD_TESTING "Build unit tests" OFF)
option(BUILD_DOCS "Build API documentation" OFF)

# Helper function to instrument targets for coverage
# Using PUBLIC ensures executables inherit the flags from libraries
function(enable_coverage_for_target target_name)
    if(ENABLE_COVERAGE)
        message(STATUS "Enabling coverage instrumentation for: ${target_name}")
        target_compile_options(${target_name} PUBLIC --coverage)
        target_link_options(${target_name} PUBLIC --coverage)
    endif()
endfunction()

# --- 2. Standard C Settings ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(include)

# --- 3. Libraries ---
add_library(keypad_parser STATIC src/keypad_parser.c)
enable_coverage_for_target(keypad_parser)
target_include_directories(keypad_parser PUBLIC include)

add_library(pin_entry STATIC src/pin_entry.c)
enable_coverage_for_target(pin_entry)
target_include_directories(pin_entry PUBLIC include)
target_link_libraries(pin_entry PRIVATE keypad_parser)

add_library(lock_logic STATIC src/lock.c)
enable_coverage_for_target(lock_logic)
target_include_directories(lock_logic PUBLIC include)

add_library(door_sensor STATIC src/door_sensor.c)
enable_coverage_for_target(door_sensor)
target_include_directories(door_sensor PUBLIC include)

# --- 4. Main Application ---
add_executable(door_security_daemon src/main.c)
target_link_libraries(door_security_daemon PRIVATE keypad_parser lock_logic
    door_sensor pin_entry)

# --- 5. External Dependencies ---
# Ensure you re-init your submodules! (git submodule update --init)
add_subdirectory(external/Unity)

# --- 6. Testing Infrastructure ---
if(BUILD_TESTING)
    enable_testing()
    set(ENV{CTEST_OUTPUT_ON_FAILURE} 1)

    # Keypad Parser Tests
    add_executable(test_keypad_parser tests/test_keypad_parser.c)
    target_link_libraries(test_keypad_parser PRIVATE keypad_parser unity)
    add_test(NAME KeypadParserUnitTests COMMAND test_keypad_parser)

    # Pin Entry Tests
    add_executable(test_pin_entry tests/test_pin_entry.c)
    target_link_libraries(test_pin_entry PRIVATE pin_entry unity)
    add_test(NAME PinEntryUnitTests COMMAND test_pin_entry)

    # Lock Logic Tests
    add_executable(test_lock tests/test_lock.c)
    target_link_libraries(test_lock PRIVATE lock_logic unity)
    add_test(NAME LockUnitTests COMMAND test_lock)

    # Door Sensor Tests
    add_executable(test_door_sensor tests/test_door_sensor.c)
    target_link_libraries(test_door_sensor PRIVATE door_sensor unity)
    add_test(NAME DoorSensorUnitTests COMMAND test_door_sensor)

    # Unified Coverage Report Target
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND ENABLE_COVERAGE)
        add_custom_target(coverage
            # The '-' allows report generation even if a test fails
            COMMAND -${CMAKE_CTEST_COMMAND} --output-on-failure
            # Capture all data, ignore system/path/unused errors
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info --no-external --ignore-errors path,unused --base-directory ${CMAKE_SOURCE_DIR}
            # Extract only our source code
            COMMAND ${LCOV_PATH} --extract coverage.info "${CMAKE_SOURCE_DIR}/src/*" --output-file coverage.info --ignore-errors unused
            # Generate HTML
            COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Report: build/coverage_report/index.html"
        )
        # This target ONLY generates the HTML, it doesn't run tests
        add_custom_target(coverage_report
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info --no-external --ignore-errors path,unused --base-directory ${CMAKE_SOURCE_DIR}
            COMMAND ${LCOV_PATH} --extract coverage.info "${CMAKE_SOURCE_DIR}/src/*" --output-file coverage.info --ignore-errors unused
            COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating HTML report from existing test data..."
        )
    endif()
endif()

# --- 7. Doxygen Documentation ---
find_package(Doxygen)
if(DOXYGEN_FOUND)
    # This creates the 'make docs' command
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
        COMMENT "Generating API documentation..."
    )
    if(BUILD_DOCS)
        add_dependencies(door_security_daemon docs)
    endif()
endif()

# --- 8. Buildroot Installation ---
install(TARGETS door_security_daemon DESTINATION bin)
