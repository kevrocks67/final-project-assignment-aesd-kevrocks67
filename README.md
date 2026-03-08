# Door Security Daemon

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
