#include <stdio.h>
#include <stdlib.h>
#include <turbojpeg.h>

int main(int argc, char **argv) {
    // Check arguments
    if (argc != 3) {
        printf("Usage: %s <input.jpg> <output.bmp>\n", argv[0]);
        return 1;
    }

    // Variables
    tjhandle tjInstance = NULL;
    FILE *jpegFile = NULL;
    unsigned char *jpegBuf = NULL;
    unsigned char *imgBuf = NULL;
    long jpegSize;
    int width, height, subsamp, colorspace;
    int pixelFormat = TJPF_RGB;
    int success = 0;

    // Create a TurboJPEG decompressor instance
    if ((tjInstance = tj3Init(TJINIT_DECOMPRESS)) == NULL) {
        printf("Error creating TurboJPEG instance: %s\n",
               tj3GetErrorStr(tjInstance));
        return 1;
    }

    // Open the JPEG file
    if ((jpegFile = fopen(argv[1], "rb")) == NULL) {
        printf("Error opening input file %s\n", argv[1]);
        goto cleanup;
    }

    // Determine JPEG file size
    fseek(jpegFile, 0, SEEK_END);
    jpegSize = ftell(jpegFile);
    fseek(jpegFile, 0, SEEK_SET);

    // Allocate memory for the JPEG buffer
    if ((jpegBuf = (unsigned char *)malloc(jpegSize)) == NULL) {
        printf("Memory allocation failure\n");
        goto cleanup;
    }

    // Read the JPEG file into the buffer
    if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1) {
        printf("Error reading input file\n");
        goto cleanup;
    }

    // Read JPEG header to get image info
    if (tj3DecompressHeader(tjInstance, jpegBuf, jpegSize) < 0) {
        printf("Error reading JPEG header: %s\n",
               tj3GetErrorStr(tjInstance));
        goto cleanup;
    }

    // Get image dimensions and other properties
    width = tj3Get(tjInstance, TJPARAM_JPEGWIDTH);
    height = tj3Get(tjInstance, TJPARAM_JPEGHEIGHT);
    subsamp = tj3Get(tjInstance, TJPARAM_SUBSAMP);
    colorspace = tj3Get(tjInstance, TJPARAM_COLORSPACE);

    printf("Image dimensions: %d x %d\n", width, height);

    // Allocate memory for the decompressed image
    if ((imgBuf = (unsigned char *)malloc(width * height * tjPixelSize[pixelFormat])) == NULL) {
        printf("Memory allocation failure\n");
        goto cleanup;
    }

    // Decompress the JPEG image
    if (tj3Decompress8(tjInstance, jpegBuf, jpegSize, imgBuf, 0, pixelFormat) < 0) {
        printf("Error decompressing JPEG image: %s\n",
               tj3GetErrorStr(tjInstance));
        goto cleanup;
    }

    // Save the decompressed image to a BMP file
    if (tj3SaveImage8(tjInstance, argv[2], imgBuf, width, 0, height, pixelFormat) < 0) {
        printf("Error saving image: %s\n",
               tj3GetErrorStr(tjInstance));
        goto cleanup;
    }

    printf("Successfully converted %s to %s\n", argv[1], argv[2]);
    success = 1;

cleanup:
    // Free resources
    if (jpegBuf) free(jpegBuf);
    if (imgBuf) free(imgBuf);
    if (jpegFile) fclose(jpegFile);
    if (tjInstance) tj3Destroy(tjInstance);

    return (success ? 0 : 1);
}
