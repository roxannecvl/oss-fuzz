
#include <turbojpeg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define NUMTESTS 7

// Pixel formats to test with
struct test {
  enum TJPF pf;
  const char* extension;  // File extension to use
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 16) return 0;  // Need at least some data to work with
  
  tjhandle handle = NULL;
  unsigned char *imgBuf = NULL;
  int width = 0, height = 0;
  char filename[FILENAME_MAX] = { 0 };
  int fd = -1;
  int retval = 0;
  
  // Different pixel formats and file extensions to test
  struct test tests[NUMTESTS] = {
    { TJPF_RGB, ".ppm" },
    { TJPF_BGR, ".bmp" },
    { TJPF_RGBX, ".ppm" },
    { TJPF_BGRA, ".bmp" },
    { TJPF_XRGB, ".ppm" },
    { TJPF_GRAY, ".pgm" },
    { TJPF_CMYK, ".ppm" }
  };

  // Initialize TurboJPEG
  // if ((handle = tj3Init(TJINIT_DECOMPRESS)) == NULL)
  //   goto bailout;
  
  // Use first 4 bytes for width (limit to reasonable values)
  width = (data[0] | (data[1] << 8)) % 1024;
  if (width < 1) width = 1;
  
  // Use next 4 bytes for height (limit to reasonable values)
  height = (data[2] | (data[3] << 8)) % 1024;
  if (height < 1) height = 1;
  
  // Use the next byte to determine which test to run
  int testIdx = data[4] % NUMTESTS;
  enum TJPF pixelFormat = tests[testIdx].pf;
  
  // Calculate pitch (0 means tightly packed)
  int pitch = 0;  // Use 0 half the time
  if (data[5] & 1) {
    pitch = width * tjPixelSize[pixelFormat];
    // Sometimes add padding
    if (data[5] & 2)
      pitch += (data[5] & 0x0F);
  }
  
  // Set some TurboJPEG parameters based on fuzzer data
  tj3Set(handle, TJPARAM_BOTTOMUP, (data[6] & 1));
  tj3Set(handle, TJPARAM_MAXMEMORY, ((data[7] & 0x0F) + 1) * 10);  // 10-160MB
  
  // Create a temporary file for output
  snprintf(filename, FILENAME_MAX, "/tmp/libjpeg-turbo_saveimage_fuzz%s", 
           tests[testIdx].extension);
  
  // Create and fill an in-memory image buffer with pattern from fuzzer data
  size_t bufSize = width * height * tjPixelSize[pixelFormat];
  if ((imgBuf = (unsigned char *)malloc(bufSize)) == NULL)
    goto bailout;
  
  // Fill buffer with pattern derived from fuzzer data
  unsigned char pattern[16];
  memcpy(pattern, data + 8, size >= 24 ? 16 : (size - 8));
  
  for (size_t i = 0; i < bufSize; i++) {
    imgBuf[i] = pattern[i % 16];
  }
  
  // Call tj3SaveImage8 with the fuzzer-created image
  if (tj3SaveImage8(handle, filename, imgBuf, width, pitch, height, pixelFormat) < 0) {
    // Errors are expected part of fuzzing - don't report them
  }
  
  // Test reading the file back
  if (access(filename, F_OK) == 0) {
    // File exists, try to read it
    FILE *file = fopen(filename, "rb");
    if (file) {
      char buffer[4];
      if (fread(buffer, 1, sizeof(buffer), file) > 0) {
        // Successfully read some data
      }
      fclose(file);
    }
  }

bailout:
  // Clean up resources
  free(imgBuf);
  tj3Destroy(handle);
  if (strlen(filename) > 0) unlink(filename);  // Remove temp file
  
  return 0;
}