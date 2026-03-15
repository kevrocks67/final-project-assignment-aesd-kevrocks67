#!/bin/bash
set -e

# Usage: ./build.sh [debug|release]
# Default to debug (CI/Badge mode)
BUILD_TYPE=${1:-debug}

if [ "$BUILD_TYPE" == "release" ]; then
  echo "📦 BUILDING CLEAN PRODUCTION BINARY"

  rm -rf build bin/*
  mkdir -p build && cd build

  # Build ARM image for BBB
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
        -DBUILD_DOCS=OFF \
        -DBUILD_TESTING=OFF \
        -DENABLE_COVERAGE=OFF ..
  make -j$(nproc)

  echo "Stripping door_security_daemon"
  # Strip symbols to make it even leaner for production
  if [ -f "../bin/door_security_daemon" ]; then
    arm-linux-gnueabihf-strip ../bin/door_security_daemon
    echo "✅ Release binary stripped and ready."
  else
    echo "⚠️ Warning: Binary not found at ../bin/door_security_daemon"
  fi

else
  echo "🧪 BUILDING FOR CI, TESTS, AND COVERAGE"

  rm -rf build tests/mocks/mocks bin/*
  mkdir -p build && cd build

  # Enable everything for the developer/CI workflow
  cmake -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_DOCS=ON \
    -DBUILD_TESTING=ON \
    -DENABLE_COVERAGE=ON ..

  make -j$(nproc)

  # Test, coverage test, and generate doxygen docs
  make coverage
  make docs

  # Generate the .info file specifically for the GitHub Badge
  lcov --capture --directory . --output-file coverage.info
  lcov --remove coverage.info \
       '/usr/*' \
       '*/build/*' \
       '*/tests/*' \
       '*/pkg/*' \
       '*/external/*' \
       '*/src/main.c' \
       '*/src/fsm_table.c' \
       --output-file coverage.info --ignore-errors unused 2>/dev/null
  COVERAGE_VAL=$(lcov --summary coverage.info | grep 'lines' | awk '{print $2}' | sed 's/\..*//;s/%//')
  echo $COVERAGE_VAL > coverage_pct.txt

  if [ -z "$COVERAGE_VAL" ]; then
    echo "⚠️ Warning: Could not extract coverage percentage."
    echo "0" > coverage_pct.txt
  else
    echo "$COVERAGE_VAL" > coverage_pct.txt
    echo "✅ Coverage report ready: $COVERAGE_VAL%"
  fi
fi
