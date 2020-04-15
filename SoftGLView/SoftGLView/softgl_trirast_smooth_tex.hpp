//============================================================================
// softgl_trirast_smooth_tex.hpp : perspectively incorrect version
//============================================================================

void RasterizeTriangle_SMOOTH_TEX(unsigned char *Color, float *Depth, const int W, const int H, 
                                  float A[3], float B[3], float C[3],
                                  float Ca[3], float Cb[3], float Cc[3],  // PER-VERTEX RGB COLORS
                                  float Ta[2], float Tb[2], float Tc[2],  // PER-VERTEX (S,T) TEX-COORDS
                                  float *Texture, const int TW, const int TH,
                                  int Wrapping, int Filtering, int Function);
