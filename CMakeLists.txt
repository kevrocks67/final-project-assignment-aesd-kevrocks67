cmake_minimum_required(VERSION 3.10)
project(door-security-daemon C)

# 1. Standard C Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# 2. Include Directories
# This tells the compiler where to look for your .h files
include_directories(include)

# 3. Main Application Modules
# Add logic.c and main.c to the executable
add_executable(door_security_daemon
    src/main.c
    src/lock.c
)

# 4. External Dependencies (Unity)
# This adds the Unity library to the project via the submodule folder
add_subdirectory(external/Unity)

# 5. Testing Infrastructure
# We only want to build tests if requested (Buildroot will usually turn this OFF)
option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING)
    enable_testing()

    # Create the test runner
    add_executable(unit_tests tests/test_lock.c src/main.c src/lock.c)

    # Link against the unity library provided by add_subdirectory
    # Unity's own CMake handles its header paths automatically
    target_link_libraries(unit_tests PRIVATE unity)

    # Register the test with CTest
    add_test(NAME LockTest COMMAND unit_tests)
endif()

find_package(Doxygen)

if (DOXYGEN_FOUND)
    # By NOT using the 'ALL' keyword here, the docs won't build
    # unless you explicitly run 'make docs'
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
        COMMENT "Generating API documentation"
        VERBATIM)

    # If the option is ON, we make the docs a dependency of the main build
    if(BUILD_DOCS)
        add_dependencies(door_security_daemon docs)
    endif()
endif()

# 6. Buildroot Installation Rules
# This tells Buildroot where to put the binary on the BBB (usually /usr/bin)
install(TARGETS door_security_daemon DESTINATION bin)
