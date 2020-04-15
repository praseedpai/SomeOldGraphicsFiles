//============================================================================
// softgl_homoclip_smooth.hpp : polygon clipping in homogeneous coordinates
//============================================================================

int ClipPolyToFrustum_SMOOTH(float *InPts, float *InPtColors, const int NumInPts, 
                             float* &OutPts, float* &OutPtColors);
