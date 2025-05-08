/*
 * Fuzzer harness for tj3SaveImage8 function in libjpeg-turbo
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <turbojpeg.h>
#include <unistd.h>

// Constants to limit input sizes
#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024
#define MIN_DIM 1

// Supported pixel formats to test
#define NUMPF 3
static const enum TJPF pixelFormats[NUMPF] = {
    TJPF_RGB,  // Most common format
    TJPF_GRAY, // Grayscale
    TJPF_CMYK  // CMYK format
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  // Need at least 12 bytes for 3 int32 values (width, height, pixelFormat)
  if (size < 12)
    return 0;

  tjhandle handle = NULL;
  unsigned char *imgBuf = NULL;
  char filename[] = "/tmp/fuzz_output_XXXXXX.jpg";
  int fd = -1;
  int retval = 0;
  int width, height, pitch;
  int pf;

  // Reading width and height from data
  memcpy(&width, data, sizeof(int));
  memcpy(&height, data + sizeof(int), sizeof(int));

  // Constrain dimensions
  width = abs(width) % (MAX_WIDTH - MIN_DIM + 1) + MIN_DIM;
  height = abs(height) % (MAX_HEIGHT - MIN_DIM + 1) + MIN_DIM;

  // Pixel format selection based on next byte
  pf = pixelFormats[data[2 * sizeof(int)] % NUMPF];

  // Allocate image buffer
  pitch = width * tjPixelSize[pf];
  size_t imgSize = pitch * height;
  imgBuf = (unsigned char *)malloc(imgSize);

  // Fill buffer with fuzz data or zeros if not enough data
  size_t copySize = size - 2 * sizeof(int) - 1;
  if (copySize > imgSize)
    copySize = imgSize;
  if (copySize > 0) {
    memcpy(imgBuf, data + 2 * sizeof(int) + 1, copySize);
  }
  if (copySize < imgSize) {
    memset(imgBuf + copySize, 0, imgSize - copySize);
  }

  // Checking that the malloc worked
  if (!imgBuf) {
    goto bailout;
  }

  // Making filename unique
  if ((fd = mkstemp(filename)) < 0)
    goto bailout;

  // Create a TurboJPEG decompression instance
  if ((handle = tj3Init(TJINIT_DECOMPRESS)) == NULL)
    goto bailout;

  // Call the function
  retval = tj3SaveImage8(handle, filename, imgBuf, width, pitch, height, pf);

bailout:
  // Clean up resources
  if (imgBuf)
    free(imgBuf);
  if (handle)
    tj3Destroy(handle);

  // Remove the output file to avoid filling up the disk
  remove(filename);

  return 0;
}
