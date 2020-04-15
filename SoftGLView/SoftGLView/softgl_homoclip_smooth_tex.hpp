//============================================================================
// softgl_homoclip_smooth_tex.hpp : polygon clipping in homogeneous coordinates
//============================================================================

int ClipPolyToFrustum_SMOOTH_TEX(float *InPts, float *InPtColors, float *InPtTexCoords, const int NumInPts, 
                                 float* &OutPts, float* &OutPtColors, float* &OutPtTexCoords);
