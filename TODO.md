# TODO

## Debug
- Chunk outline renderer
- Single-vao immediate mode debug renderer (renders AABBs, vectors, etc)

## Features
- Remote server for multiplayer
- Infinite world
- Procedural world generation

## Optimizations
- Multithreaded chunk mesh generation
- Proper player physics (dont simply apply velocity, check if each block inbetween this and next position is not solid)
- Frustum culling
- Greedy meshing far away chunks
- Maybe use Sparse Voxel Octrees or Bricks
