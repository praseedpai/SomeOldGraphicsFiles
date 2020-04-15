//============================================================================
// softgl_trirast_smooth_pc.hpp : with perspectively correct (PC) color interpolation
//============================================================================

void RasterizeTriangle_SMOOTH_PC(unsigned char *Color, float *Depth, const int W, const int H, 
                                 float A[4], float B[4], float C[4],
                                 float Ca[3], float Cb[3], float Cc[3]);  // PER-VERTEX RGB COLORS
