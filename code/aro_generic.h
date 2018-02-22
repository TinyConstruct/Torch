#define ASSERTIONS_ENABLED 1

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

void reportAssertionFailure(char* expr, char* file, int line) {
  //TODO: actually OutputDebugString() this? Not really needed as of writing
  return;
}

#if ASSERTIONS_ENABLED
#define assert(expr)  if(expr){} else { reportAssertionFailure(#expr,  __FILE__, __LINE__);  __debugbreak();  } 
#else
#define assert(expr)
#endif

#define file_internal static 
#define local_persist static 
#define global_variable static

#define InvalidCodePath assert(!1==1)

uint32 safeTruncateUInt64(uint64 value)
{
    assert(value <= 0xFFFFFFFF);
    uint32 result = (uint32)value;
    return(result);
}