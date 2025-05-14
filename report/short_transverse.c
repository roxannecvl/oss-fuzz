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