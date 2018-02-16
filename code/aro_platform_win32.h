struct FileReadResult {
    uint32 contentsSize;
    void *contents;
};

struct Bitmap {
  int32 width, height;
  void* contents;
};

//This should only be taken as me wanting to know how things work, it's not 
//really a good/safe implementation by any stretch of imagination
//Force byte alignment so header offests stay intact
#pragma pack(push,1)
struct BitmapHeader{
  uint16 bfType;
  uint32 bfSize;
  uint16 bfReserved1;
  uint16 bfReserved2;
  uint32 bfOffBits;
  uint32 biSize;
  int32 biWidth;
  int32  biHeight;
  uint16 biPlanes;
  uint16 biBitCount;
};
#pragma pack(pop)



