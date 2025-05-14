do_transverse(j_decompress_ptr srcinfo, j_compress_ptr dstinfo,
              JDIMENSION x_crop_offset, JDIMENSION y_crop_offset,
              jvirt_barray_ptr *src_coef_arrays,
              jvirt_barray_ptr *dst_coef_arrays)
/* Transverse transpose is equivalent to
 *   1. 180 degree rotation;
 *   2. Transposition;
 * or
 *   1. Horizontal mirroring;
 *   2. Transposition;
 *   3. Horizontal mirroring.
 * These steps are merged into a single processing routine.
 */
{
  JDIMENSION MCU_cols, MCU_rows, comp_width, comp_height, dst_blk_x, dst_blk_y;
  JDIMENSION x_crop_blocks, y_crop_blocks;
  int ci, i, j, offset_x, offset_y;
  JBLOCKARRAY src_buffer, dst_buffer;
  JCOEFPTR src_ptr, dst_ptr;
  jpeg_component_info *compptr;

  MCU_cols = srcinfo->output_height /
             (dstinfo->max_h_samp_factor * dstinfo_min_DCT_h_scaled_size);
  MCU_rows = srcinfo->output_width /
             (dstinfo->max_v_samp_factor * dstinfo_min_DCT_v_scaled_size);

  for (ci = 0; ci < dstinfo->num_components; ci++) {
    compptr = dstinfo->comp_info + ci;
    comp_width = MCU_cols * compptr->h_samp_factor;
    comp_height = MCU_rows * compptr->v_samp_factor;
    x_crop_blocks = x_crop_offset * compptr->h_samp_factor;
    y_crop_blocks = y_crop_offset * compptr->v_samp_factor;
    for (dst_blk_y = 0; dst_blk_y < compptr->height_in_blocks;
         dst_blk_y += compptr->v_samp_factor) {
      dst_buffer = (*srcinfo->mem->access_virt_barray)
        ((j_common_ptr)srcinfo, dst_coef_arrays[ci], dst_blk_y,
         (JDIMENSION)compptr->v_samp_factor, TRUE);
      for (offset_y = 0; offset_y < compptr->v_samp_factor; offset_y++) {
        for (dst_blk_x = 0; dst_blk_x < compptr->width_in_blocks;
             dst_blk_x += compptr->h_samp_factor) {
          if (x_crop_blocks + dst_blk_x < comp_width) {
            /* Block is within the mirrorable area. */
            src_buffer = (*srcinfo->mem->access_virt_barray)
              ((j_common_ptr)srcinfo, src_coef_arrays[ci],
               comp_width - x_crop_blocks - dst_blk_x -
               (JDIMENSION)compptr->h_samp_factor,
               (JDIMENSION)compptr->h_samp_factor, FALSE);
          } else {
            src_buffer = (*srcinfo->mem->access_virt_barray)
              ((j_common_ptr)srcinfo, src_coef_arrays[ci],
               dst_blk_x + x_crop_blocks,
               (JDIMENSION)compptr->h_samp_factor, FALSE);
          }
          for (offset_x = 0; offset_x < compptr->h_samp_factor; offset_x++) {
            dst_ptr = dst_buffer[offset_y][dst_blk_x + offset_x];
            if (y_crop_blocks + dst_blk_y < comp_height) {
              if (x_crop_blocks + dst_blk_x < comp_width) {
                /* Block is within the mirrorable area. */
                src_ptr = src_buffer[compptr->h_samp_factor - offset_x - 1]
                  [comp_height - y_crop_blocks - dst_blk_y - offset_y - 1];
                for (i = 0; i < DCTSIZE; i++) {
                  for (j = 0; j < DCTSIZE; j++) {
                    dst_ptr[j * DCTSIZE + i] = src_ptr[i * DCTSIZE + j];
                    j++;
                    dst_ptr[j * DCTSIZE + i] = -src_ptr[i * DCTSIZE + j];
                  }
                  i++;
                  for (j = 0; j < DCTSIZE; j++) {
                    dst_ptr[j * DCTSIZE + i] = -src_ptr[i * DCTSIZE + j];
                    j++;
                    dst_ptr[j * DCTSIZE + i] = src_ptr[i * DCTSIZE + j];
                  }
                }
              } else {
                /* Right-edge blocks are mirrored in y only */
                src_ptr = src_buffer[offset_x]
                  [comp_height - y_crop_blocks - dst_blk_y - offset_y - 1];
                for (i = 0; i < DCTSIZE; i++) {
                  for (j = 0; j < DCTSIZE; j++) {
                    dst_ptr[j * DCTSIZE + i] = src_ptr[i * DCTSIZE + j];
                    j++;
                    dst_ptr[j * DCTSIZE + i] = -src_ptr[i * DCTSIZE + j];
                  }
                }
              }
            } else {
              if (x_crop_blocks + dst_blk_x < comp_width) {
                /* Bottom-edge blocks are mirrored in x only */
                src_ptr = src_buffer[compptr->h_samp_factor - offset_x - 1]
                  [dst_blk_y + offset_y + y_crop_blocks];
                for (i = 0; i < DCTSIZE; i++) {
                  for (j = 0; j < DCTSIZE; j++)
                    dst_ptr[j * DCTSIZE + i] = src_ptr[i * DCTSIZE + j];
                  i++;
                  for (j = 0; j < DCTSIZE; j++)
                    dst_ptr[j * DCTSIZE + i] = -src_ptr[i * DCTSIZE + j];
                }
              } else {
                /* At lower right corner, just transpose, no mirroring */
                src_ptr = src_buffer[offset_x]
                  [dst_blk_y + offset_y + y_crop_blocks];
                for (i = 0; i < DCTSIZE; i++)
                  for (j = 0; j < DCTSIZE; j++)
                    dst_ptr[j * DCTSIZE + i] = src_ptr[i * DCTSIZE + j];
              }
            }
          }
        }
      }
    }
  }
}