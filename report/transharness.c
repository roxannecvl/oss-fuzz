/*                 New from here                */ 
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].op = TJXOP_TRANSPOSE;
   transforms[0].options = TJXOPT_CROP;
   dstSizes[0] = 0;
   transforms[0].r.w = (height + 10);
   transforms[0].r.h = (width + 10);
 
   tj3Set(handle, TJPARAM_NOREALLOC, 0);
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }


   /* ----------------------------------- */
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].r.w = (height + 1) / 2;
   transforms[0].r.h = (width + 1) / 2;
   transforms[0].op = TJXOP_TRANSPOSE;
   transforms[0].options = TJXOPT_GRAY | TJXOPT_CROP |
                           TJXOPT_OPTIMIZE;
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   tj3Set(handle, TJPARAM_NOREALLOC, 1); // NEW crashes if we remove this line
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }


   /* ------------------------------------- */
   free(dstBufs[0]); 
   dstBufs[0] = NULL; 
 
   transforms[0].r.w = (height * 2);
   transforms[0].r.h = (width * 3);
   transforms[0].op = TJXOP_HFLIP;
   transforms[0].options = TJXOPT_GRAY | TJXOPT_TRIM | TJXOPT_COPYNONE |
                           TJXOPT_OPTIMIZE;
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }

    /* ------------------------------------- */
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].r.w = (height + 1) / 2;
   transforms[0].r.h = (width + 1) / 2;
   transforms[0].op = TJXOP_VFLIP;
   transforms[0].options = TJXOPT_CROP | TJXOPT_COPYNONE | TJXOPT_OPTIMIZE;
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }

   /* ------------------------------------- */
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].r.w = height;
   transforms[0].r.h = width;
   transforms[0].op = TJXOP_TRANSVERSE;
   transforms[0].options = TJXOPT_ARITHMETIC;
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   tj3Set(handle, TJPARAM_NOREALLOC, 1); // NEW crashes if we remove this line
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }

   /* ------------------------------------- */
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].r.w = (height + 1) / 2;
   transforms[0].r.h = (width + 1) / 2;
   transforms[0].op = TJXOP_ROT180;
   transforms[0].options = TJXOPT_NOOUTPUT ;
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }

   /* ----------------------------------- */
   free(dstBufs[0]);
   dstBufs[0] = NULL;
 
   transforms[0].r.w = (height + 1) / 2;
   transforms[0].r.h = (width + 1) / 2;
   transforms[0].op = TJXOP_ROT270;
   transforms[0].options = TJXOPT_TRIM; 
   dstSizes[0] = maxBufSize = tj3TransformBufSize(handle, &transforms[0]);
   if (dstSizes[0] == 0 ||
       (dstBufs[0] = (unsigned char *)tj3Alloc(dstSizes[0])) == NULL)
     goto bailout;
 
   tj3Set(handle, TJPARAM_NOREALLOC, 1); // NEW crashes if we remove this line
   if (tj3Transform(handle, data, size, 1, dstBufs, dstSizes,
                    transforms) == 0) {
     size_t sum = 0;
 
     for (i = 0; i < dstSizes[0]; i++)
       sum += dstBufs[0][i];
 
     if (sum > 255 * maxBufSize)
       goto bailout;
   }
   


/*       UNTIL HERE                  */ 
