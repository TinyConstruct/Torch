struct SwapSpace {
  int sizeInBytes;
  void* space;
};

global_variable SwapSpace simulationSwapSpace;
global_variable TileMinHeap* simMinHeap;