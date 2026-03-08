#!/bin/bash
set -e

# Usage: ./build.sh [debug|release]
# Default to debug (CI/Badge mode)
BUILD_TYPE=${1:-debug}

mkdir -p build && cd build

if [ "$BUILD_TYPE" == "release" ]; then
  echo "📦 BUILDING CLEAN PRODUCTION BINARY"
  # Disable testing and coverage at the CMake level
  cmake -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DENABLE_COVERAGE=OFF ..
  make
  # Optional: strip symbols to make it even leaner for production
  strip ../bin/door_security_daemon
  echo "✅ Release binary stripped and ready."

else
  echo "🧪 BUILDING FOR CI, TESTS, AND COVERAGE"
  # Enable everything for the developer/CI workflow
  cmake -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DENABLE_COVERAGE=ON ..

  make
  # Use your custom target that handles both
  make coverage
  make docs

  # Generate the .info file specifically for the GitHub Badge
  # (Assuming your 'make coverage' creates the raw .gcda files)
  lcov --capture --directory . --output-file coverage.info
  lcov --remove coverage.info '/usr/*' '*/tests/*' '*/build/*' --output-file coverage.info
  echo "✅ Coverage report generated for GitHub Badges."
fi
