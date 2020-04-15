//============================================================================
// softgl_trirast_smooth_tex_pc.hpp : with perspectively correct (PC) color and 
//                                 texture-coordinate interpolation
//============================================================================

void RasterizeTriangle_SMOOTH_TEX_PC(unsigned char *Color, float *Depth, const int W, const int H, 
                                     float A[4], float B[4], float C[4],
                                     float Ca[3], float Cb[3], float Cc[3],  // PER-VERTEX RGB COLORS
                                     float Ta[2], float Tb[2], float Tc[2],  // PER-VERTEX (S,T) TEX-COORDS
                                     float *Texture, const int TW, const int TH,
                                     int Wrapping, int Filtering, int Function);
