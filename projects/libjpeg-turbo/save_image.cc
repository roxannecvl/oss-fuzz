/*
 * Fuzzer harness for tj3SaveImage8/12/16 functions in libjpeg-turbo
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <turbojpeg.h>
#include <unistd.h>
// Constants to limit input sizes
#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080
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
  unsigned char *imgBuf8 = NULL;
  short *imgBuf12 = NULL;
  unsigned short *imgBuf16 = NULL;
  char filename8[] = "/tmp/fuzz_output8_XXXXXX";
  char filename12[] = "/tmp/fuzz_output12_XXXXXX";
  char filename16[] = "/tmp/fuzz_output16_XXXXXX";
  int fd = -1;
  int retval = 0;
  int width, height, pitch;
  int pf;
  size_t copySize;
  // Reading width and height from data
  memcpy(&width, data, sizeof(int));
  memcpy(&height, data + sizeof(int), sizeof(int));
  width = (int)(((uint32_t)width) % (MAX_WIDTH - MIN_DIM + 1)) + MIN_DIM;
  height = (int)(((uint32_t)height) % (MAX_HEIGHT - MIN_DIM + 1)) + MIN_DIM;
  // Pixel format selection based on next byte
  pf = pixelFormats[data[2 * sizeof(int)] % NUMPF];
  // Allocate image buffers
  pitch = width * tjPixelSize[pf];
  size_t imgSize = pitch * height;

  // Allocate 8-bit buffer
  imgBuf8 = (unsigned char *)malloc(imgSize);
  if (!imgBuf8) goto bailout;

  // Allocate 12-bit buffer
  imgBuf12 = (short *)malloc(imgSize * sizeof(short) / sizeof(unsigned char));
  if (!imgBuf12) goto bailout;

  // Allocate 16-bit buffer
  imgBuf16 = (unsigned short *)malloc(imgSize * sizeof(unsigned short) / sizeof(unsigned char));
  if (!imgBuf16) goto bailout;

  // Fill 8-bit buffer with fuzz data or zeros if not enough data
  copySize = size - 2 * sizeof(int) - 1;
  if (copySize > imgSize)
    copySize = imgSize;
  if (copySize > 0) {
    memcpy(imgBuf8, data + 2 * sizeof(int) + 1, copySize);
  }
  if (copySize < imgSize) {
    memset(imgBuf8 + copySize, 0, imgSize - copySize);
  }

  // Convert 8-bit data to 12-bit (scale up values)
  for (size_t i = 0; i < imgSize; i++) {
    imgBuf12[i] = (short)(imgBuf8[i] << 4); // Scale from 8-bit to 12-bit
  }

  // Convert 8-bit data to 16-bit (scale up values)
  for (size_t i = 0; i < imgSize; i++) {
    imgBuf16[i] = (unsigned short)(imgBuf8[i] << 8); // Scale from 8-bit to 16-bit
  }

  for (int i = 0; i < 3; i++) {
    // Making filename unique
    if (i == 0) {
      if ((fd = mkstemp(filename8)) < 0)
        goto bailout;
    } else if (i == 1) {
      if ((fd = mkstemp(filename12)) < 0)
        goto bailout;
    } else {
      if ((fd = mkstemp(filename16)) < 0)
        goto bailout;
    }
    close(fd); // Closing the file descriptor

    // Create a TurboJPEG decompression instance
    if ((handle = tj3Init(TJINIT_DECOMPRESS)) == NULL)
      goto bailout;

    // Call the appropriate save function with the proper buffer
    if (i == 0) {
      retval = tj3SaveImage8(handle, filename8, imgBuf8, width, pitch, height, pf);
    } else if (i == 1) {
      retval = tj3SaveImage12(handle, filename12, imgBuf12, width, pitch, height, pf);
    } else {
      retval = tj3SaveImage16(handle, filename16, imgBuf16, width, pitch, height, pf);
    }

    // Cleanup handle for this iteration
    if (handle) {
      tj3Destroy(handle);
      handle = NULL;
    }
  }

bailout:
  // Clean up resources
  if (imgBuf8)
    free(imgBuf8);
  if (imgBuf12)
    free(imgBuf12);
  if (imgBuf16)
    free(imgBuf16);
  if (handle)
    tj3Destroy(handle);
  // Remove the output files to avoid filling up the disk
  unlink(filename8);
  unlink(filename12);
  unlink(filename16);
  return 0;
}
