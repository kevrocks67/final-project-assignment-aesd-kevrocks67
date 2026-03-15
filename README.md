# Door Security Daemon

![Build Status](https://github.com/kevrocks67/final-project-assignment-aesd-kevrocks67/actions/workflows/github-actions/badge.svg)
![Coverage](https://img.shields.io/endpoint?url=https://kevrocks67.github.io/final-project-assignment-aesd-kevrocks67/coverage-badge.json)
![License](https://img.shields.io/badge/License-GPLv3-blue.svg)

This is part of my final project for the CU Boulder Advanced Embedded Linux
Development course.

[Project Overview]("https://github.com/cu-ecen-aeld/final-project-kevrocks67/wiki/Project-Overview")

The Door Security Daemon is an embedded C application designed for the BeagleBone
Black (BBB). It handles pin input from an adafruit 3x4 keypad and a magnetic contact
switch. The "lock" is a SG90 servo. This daemon supports secure, signed OTA
updates via SWUpdate.

This is meant to be deployed through a buildroot image which can be found in
[this]("https://github.com/cu-ecen-aeld/final-project-kevrocks67") other repo.

## Building

1. Build the environment image

```bash
docker build -t aesd-build-env .
```

2. Build the software

```bash
# Dev
docker run -it --rm -v $(pwd):/workspace aesd-build-env ./build.sh debug
# Release
docker run -it --rm -v $(pwd):/workspace aesd-build-env ./build.sh release

```
