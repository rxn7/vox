# VOX
C++ Voxel Engine (possibly a game in the future) made with modern OpenGL (core 4.3).

# Building
#### Only linux is supported (may work on windows, but not tested).

## Requirements
- libopenssl 
- libprotobuf

## Commands
```bash
mkdir build
cd build
cmake ..
cmake --build .
./vox
```
or you can just use my configuration:
```bash
./scripts/configure.sh 
./scripts/debug.sh
```
