/* TurboJPEG 3.0+ */
DLLEXPORT int GET_NAME(tj3SaveImage, BITS_IN_JSAMPLE)
  (tjhandle handle, const char *filename, const _JSAMPLE *buffer, int width,
   int pitch, int height, int pixelFormat)
{
  static const char FUNCTION_NAME[] =
    GET_STRING(tj3SaveImage, BITS_IN_JSAMPLE);
  int retval = 0;

#if BITS_IN_JSAMPLE != 16 || defined(D_LOSSLESS_SUPPORTED)

  tjhandle handle2 = NULL;
  tjinstance *this2;
  j_decompress_ptr dinfo = NULL;
  djpeg_dest_ptr dst;
  FILE *file = NULL;
  char *ptr = NULL;
  boolean invert;

  GET_TJINSTANCE(handle, -1)

  if (!filename || !buffer || width < 1 || pitch < 0 || height < 1 ||
      pixelFormat < 0 || pixelFormat >= TJ_NUMPF)
    THROW("Invalid argument");

  /* The instance handle passed to this function is used only for parameter retrieval.  Create a new temporary instance to avoid interfering with the libjpeg state of the primary instance. */
  if ((handle2 = tj3Init(TJINIT_DECOMPRESS)) == NULL)
    return -1;
  this2 = (tjinstance *)handle2;
  dinfo = &this2->dinfo;

#ifdef _MSC_VER
  if (fopen_s(&file, filename, "wb") || file == NULL)
#else
  if ((file = fopen(filename, "wb")) == NULL)
#endif
    THROW_UNIX("Cannot open output file");

  if (setjmp(this2->jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error. */
    retval = -1;  goto bailout;
  }

  this2->dinfo.out_color_space = pf2cs[pixelFormat];
  dinfo->image_width = width;  dinfo->image_height = height;
  dinfo->global_state = DSTATE_READY;
  dinfo->scale_num = dinfo->scale_denom = 1;
  dinfo->data_precision = BITS_IN_JSAMPLE;

  ptr = strrchr(filename, '.');
  if (ptr && !strcasecmp(ptr, ".bmp")) {
    if ((dst = jinit_write_bmp(dinfo, FALSE, FALSE)) == NULL)
      THROW("Could not initialize bitmap writer");
    invert = !this->bottomUp;
    dinfo->X_density = (UINT16)this->xDensity;
    dinfo->Y_density = (UINT16)this->yDensity;
    dinfo->density_unit = (UINT8)this->densityUnits;
  } else {
#if BITS_IN_JSAMPLE == 8
    if (this->precision >= 2 && this->precision <= BITS_IN_JSAMPLE)
#else
    if (this->precision >= BITS_IN_JSAMPLE - 3 &&
        this->precision <= BITS_IN_JSAMPLE)
#endif
      dinfo->data_precision = this->precision;
    if ((dst = _jinit_write_ppm(dinfo)) == NULL)
      THROW("Could not initialize PPM writer");
    invert = this->bottomUp;
  }

  dinfo->mem->max_memory_to_use = (long)this->maxMemory * 1048576L;

  dst->output_file = file;
  (*dst->start_output) (dinfo, dst);
  (*dinfo->mem->realize_virt_arrays) ((j_common_ptr)dinfo);

  if (pitch == 0) pitch = width * tjPixelSize[pixelFormat];

  while (dinfo->output_scanline < dinfo->output_height) {
    _JSAMPLE *rowptr;

    if (invert)
      rowptr =
        (_JSAMPLE *)&buffer[(height - dinfo->output_scanline - 1) * pitch];
    else
      rowptr = (_JSAMPLE *)&buffer[dinfo->output_scanline * pitch];
    memcpy(dst->_buffer[0], rowptr,
           width * tjPixelSize[pixelFormat] * sizeof(_JSAMPLE));
    (*dst->put_pixel_rows) (dinfo, dst, 1);
    dinfo->output_scanline++;
  }

  (*dst->finish_output) (dinfo, dst);

bailout:
  tj3Destroy(handle2);
  if (file) fclose(file);
  return retval;

#else /* BITS_IN_JSAMPLE != 16 || defined(D_LOSSLESS_SUPPORTED) */

  GET_TJINSTANCE(handle, -1)
  THROW("16-bit data precision requires lossless JPEG,\n"
        "which was disabled at build time.")
bailout:
  return retval;

#endif
}