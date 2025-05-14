#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <turbojpeg.h>

void write_le32(FILE *out, int val) {
    uint8_t b[4] = {
        (uint8_t)(val & 0xff),          // least significant byte
        (uint8_t)((val >> 8) & 0xff),
        (uint8_t)((val >> 16) & 0xff),
        (uint8_t)((val >> 24) & 0xff)   // most significant byte
    };
    fwrite(b, 1, 4, out);
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input.jpg> <output.seed>\n", argv[0]);
        return 1;
    }

    tjhandle tjInstance = NULL;
    FILE *jpegFile = NULL;
    unsigned char *jpegBuf = NULL;
    unsigned char *imgBuf = NULL;
    long jpegSize;
    int width, height;
    int pixelFormat = TJPF_RGB;  // must match an entry in your harness pixelFormats[]
    int pf_index = 0;            // TJPF_RGB is index 0 in your harness
    int success = 0;

    // Init TurboJPEG
    if ((tjInstance = tj3Init(TJINIT_DECOMPRESS)) == NULL) {
        fprintf(stderr, "TurboJPEG init error: %s\n", tj3GetErrorStr(tjInstance));
        return 1;
    }

    // Open input JPEG
    if ((jpegFile = fopen(argv[1], "rb")) == NULL) {
        perror("Error opening JPEG");
        goto cleanup;
    }

    fseek(jpegFile, 0, SEEK_END);
    jpegSize = ftell(jpegFile);
    fseek(jpegFile, 0, SEEK_SET);

    jpegBuf = (unsigned char *)malloc(jpegSize);
    if (!jpegBuf) goto cleanup;

    if (fread(jpegBuf, 1, jpegSize, jpegFile) != jpegSize) {
        perror("JPEG read failed");
        goto cleanup;
    }

    if (tj3DecompressHeader(tjInstance, jpegBuf, jpegSize) < 0) {
        fprintf(stderr, "Header error: %s\n", tj3GetErrorStr(tjInstance));
        goto cleanup;
    }

    width = tj3Get(tjInstance, TJPARAM_JPEGWIDTH);
    height = tj3Get(tjInstance, TJPARAM_JPEGHEIGHT);

    // Get JPEG colorspace and select matching pixel format used in harness
    int colorspace = tj3Get(tjInstance, TJPARAM_COLORSPACE);

    switch (colorspace) {
        case TJCS_GRAY:
            pixelFormat = TJPF_GRAY;
            pf_index = 1;
            break;
        case TJCS_CMYK:
            pixelFormat = TJPF_CMYK;
            pf_index = 2;
            break;
        default:
            pixelFormat = TJPF_RGB;
            pf_index = 0;
    }

    size_t pixelSize = tjPixelSize[pixelFormat];
    size_t imageSize = width * height * pixelSize;

    imgBuf = (unsigned char *)malloc(imageSize);
    if (!imgBuf) goto cleanup;

    if (tj3Decompress8(tjInstance, jpegBuf, jpegSize, imgBuf, 0, pixelFormat) < 0) {
        fprintf(stderr, "Decompression failed: %s\n", tj3GetErrorStr(tjInstance));
        goto cleanup;
    }

    // Create the seed file
    FILE *out = fopen(argv[2], "wb");
    if (!out) {
        perror("Error opening output");
        goto cleanup;
    }

    write_le32(out, width);
    write_le32(out, height);
    fwrite(&pf_index, 1, 1, out);
    fwrite(imgBuf, 1, imageSize, out);
    fclose(out);

    printf("Wrote seed with size %zu for %dx%d RGB\n", imageSize + 9, width, height);
    success = 1;

cleanup:
    if (jpegBuf) free(jpegBuf);
    if (imgBuf) free(imgBuf);
    if (jpegFile) fclose(jpegFile);
    if (tjInstance) tj3Destroy(tjInstance);

    return (success ? 0 : 1);
}
