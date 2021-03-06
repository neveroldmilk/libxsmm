#if defined(LIBXSMM_INTRINSICS_AVX512) /*__AVX512F__*/
# define TRANSPOSE_W_CHUNK(img, src_ifm1, ij, w_offset, src_ifm2, dst_ifm1, dst_ifm2) \
        base_addr = &LIBXSMM_VLA_ACCESS(6, input_nopad, img, src_ifm1, ij, w_offset, src_ifm2, 0, handle->blocksifm_lp, handle->ifhp, handle->ifwp, handle->ifmblock, handle->fm_lp_block); \
        gather_reg = _mm512_i32gather_epi32(vgindex, base_addr, 1); \
        lo_reg= LIBXSMM_INTRINSICS_MM512_EXTRACTI64X4_EPI64(gather_reg,0); \
        hi_reg= LIBXSMM_INTRINSICS_MM512_EXTRACTI64X4_EPI64(gather_reg,1); \
        compressed_low  = _mm256_unpacklo_epi16(lo_reg, hi_reg); \
        compressed_low =  _mm256_permutevar8x32_epi32(compressed_low, shuffler); \
        compressed_high  = _mm256_unpackhi_epi16(lo_reg, hi_reg); \
        compressed_high =  _mm256_permutevar8x32_epi32(compressed_high, shuffler); \
        compressed_low_store = _mm256_insertf128_si256(compressed_low_store, _mm256_extractf128_si256(compressed_low,0), 0); \
        compressed_low_store = _mm256_insertf128_si256(compressed_low_store, _mm256_extractf128_si256(compressed_high, 0), 1); \
        compressed_high_store = _mm256_insertf128_si256(compressed_high_store, _mm256_extractf128_si256(compressed_low,1), 0); \
        compressed_high_store = _mm256_insertf128_si256(compressed_high_store, _mm256_extractf128_si256(compressed_high, 1), 1); \
        _mm256_storeu_si256((union __m256i *) &LIBXSMM_VLA_ACCESS(5, tr_input_nopad, img, dst_ifm1, ij, dst_ifm2, w_offset, BLOCKSIFM, handle->ifhp, handle->ifmblock, ifwp_extended), compressed_low_store); \
        _mm256_storeu_si256((union __m256i *) &LIBXSMM_VLA_ACCESS(5, tr_input_nopad, img, dst_ifm1, ij, dst_ifm2+1, w_offset, BLOCKSIFM, handle->ifhp, handle->ifmblock, ifwp_extended), compressed_high_store)

{ /* scope for local variable declarations */
  int w_chunks = handle->ifwp/16;
  int w_i, w;
  int c_i;
  element_input_type *base_addr;
  const __m512i vgindex = _mm512_set_epi32(960,832,448,320,  704,576,192,64,  896,768,384,256,  640,512,128,0);
  const __m256i shuffler = _mm256_set_epi32(7,5,3,1,6,4,2,0);
  __m512i gather_reg;
  __m256i lo_reg, hi_reg, compressed_low, compressed_high, compressed_low_store, compressed_high_store;

  if (w_chunks == 1) {
    for (ifm1 = 0; ifm1 < handle->blocksifm_lp; ++ifm1) {
      for (ij = 0; ij < handle->ifhp; ++ij) {
        for (ifm2 = 0; ifm2 < 8; ++ifm2) {
          TRANSPOSE_W_CHUNK(img, ifm1, ij, 0, ifm2, 2*ifm1, 2*ifm2);
          TRANSPOSE_W_CHUNK(img, ifm1, ij, 0, ifm2+8, 2*ifm1+1, 2*ifm2);
        }
      }
    }
  } else {
    for (ifm1 = 0; ifm1 < handle->blocksifm_lp; ++ifm1) {
      for (ij = 0; ij < handle->ifhp; ++ij) {
        for (w = 0; w < w_chunks; w++) {
          for (ifm2 = 0; ifm2 < 8; ++ifm2) {
            TRANSPOSE_W_CHUNK(img, ifm1, ij, w*16, ifm2, 2*ifm1, 2*ifm2);
            TRANSPOSE_W_CHUNK(img, ifm1, ij, w*16, ifm2+8, 2*ifm1+1, 2*ifm2);
          }
        }
      }
    }
  }
}
# undef TRANSPOSE_W_CHUNK
#else /* won't happen as this code only runs on AVX512 platforms */
  LIBXSMM_ASSERT(0);
#endif

