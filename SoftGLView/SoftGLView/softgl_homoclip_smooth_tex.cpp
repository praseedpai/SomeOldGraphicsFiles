#include "stdafx.h"
//============================================================================
// softgl_homoclip_smooth_tex.cpp : polygon clipping in homogeneous coordinates
//============================================================================

static inline void LinearInterp(const float A[4], const float Ca[3], const float Ta[4],
                                const float B[4], const float Cb[3], const float Tb[4], 
                                float P[4], float PC[3], float PT[4],
                                const float t)
{
  P[0]  =  A[0] + (B[0]-A[0])*t;
  P[1]  =  A[1] + (B[1]-A[1])*t;
  P[2]  =  A[2] + (B[2]-A[2])*t;
  P[3]  =  A[3] + (B[3]-A[3])*t;
  PC[0] = Ca[0] + (Cb[0]-Ca[0])*t;
  PC[1] = Ca[1] + (Cb[1]-Ca[1])*t;
  PC[2] = Ca[2] + (Cb[2]-Ca[2])*t;
  PT[0] = Ta[0] + (Tb[0]-Ta[0])*t;
  PT[1] = Ta[1] + (Tb[1]-Ta[1])*t;
  PT[2] = Ta[2] + (Tb[2]-Ta[2])*t;
  PT[3] = Ta[3] + (Tb[3]-Ta[3])*t;
}

//----------------------------------------------------------------------------
// Clips a 4D homogeneous polygon defined by the packed array of float InPts.
// to the frustum boundary defined by WhichBoundary. The clipped polygon is
// put in OutPts (which must be a different location than InPts) and the number
// of vertices in the clipped polygon is returned. InPts must have NumInPts*4 
// floats (enough to contain poly), OutPts must be preallocated to at least 
// (NumInPts+1)*4 floats. Regular orthographic NDC clipping can be achieved by
// making the w coordinate of each point be 1. WhichBoundary: 0=LEFT, 1=RIGHT,
// 2=BOTTOM, 3=TOP, 4=NEAR, 5=FAR
//----------------------------------------------------------------------------
static int ClipPolyToBoundary(float *InPts, float *InPtColors, float *InPtTexCoords, const int NumInPts, 
                              float *OutPts, float *OutPtColors, float *OutPtTexCoords, const int WhichBoundary)
{
  float *A, *B;                                  // CURRENT EDGE ENDPOINTS
  float *AC, *BC;                                // EDGE ENDPOINT COLORS
  float *AT, *BT;                                // EDGE ENDPOINT TEX-COORDS
  int Aout, Bout;                                // CURRENT EDGE ENDPOINT "OUTSIDE" CONDITION
  float t;                                       // EDGE PARAMETRIC VALUE (FOR PLANE ISECT CALC)

  int iOut=0, iOutColor=0, iOutTexCoords=0;
  int Npts=NumInPts*4, Ncolors=NumInPts*3, Ntexcoords=NumInPts*4;
  for (int iIn=0, iInColors=0, iInTexCoords=0; iIn<Npts; iIn+=4, iInColors+=3, iInTexCoords+=4)
  {
    A = &(InPts[iIn]);                           // SET CURRENT EDGE ENDPOINTS
    AC = &(InPtColors[iInColors]);
    AT = &(InPtTexCoords[iInTexCoords]);
    B = &(InPts[(iIn+4)%Npts]);
    BC = &(InPtColors[(iInColors+3)%Ncolors]);
    BT = &(InPtTexCoords[(iInTexCoords+4)%Ntexcoords]);

    switch(WhichBoundary)                        // SET OUTSIDE CONDITION FOR EACH ENDPOINT
    {
      case 0: Aout=(A[0]<-A[3]); Bout=(B[0]<-B[3]); break;  // LEFT
      case 1: Aout=(A[0]>A[3]);  Bout=(B[0]>B[3]);  break;  // RIGHT
      case 2: Aout=(A[1]<-A[3]); Bout=(B[1]<-B[3]); break;  // BOTTOM
      case 3: Aout=(A[1]>A[3]);  Bout=(B[1]>B[3]);  break;  // TOP
      case 4: Aout=(A[2]<-A[3]); Bout=(B[2]<-B[3]); break;  // NEAR
      case 5: Aout=(A[2]>A[3]);  Bout=(B[2]>B[3]);  break;  // FAR
    };

    if (Aout != Bout)                            // OUT->IN: OUTPUT INTERSECTION POINT
    { 
      switch(WhichBoundary)
      {
        case 0: t=(-A[3]-A[0])/(B[0]-A[0]+B[3]-A[3]); break;  // LEFT
        case 1: t=(A[3]-A[0])/(B[0]-A[0]-B[3]+A[3]);  break;  // RIGHT
        case 2: t=(-A[3]-A[1])/(B[1]-A[1]+B[3]-A[3]); break;  // BOTTOM
        case 3: t=(A[3]-A[1])/(B[1]-A[1]-B[3]+A[3]);  break;  // TOP
        case 4: t=(-A[3]-A[2])/(B[2]-A[2]+B[3]-A[3]); break;  // NEAR
        case 5: t=(A[3]-A[2])/(B[2]-A[2]-B[3]+A[3]);  break;  // FAR
      };
      LinearInterp(A,AC,AT,B,BC,BT,&(OutPts[iOut]),&(OutPtColors[iOutColor]),&(OutPtTexCoords[iOutTexCoords]),t);
      iOut+=4;
      iOutColor+=3;
      iOutTexCoords+=4;
    }

    if ( (Aout && !Bout) || (!Aout && !Bout) )   // OUT->IN, IN->IN : OUTPUT 2ND EDGE VERTEX
    { 
      OutPts[iOut  ]=B[0];
      OutPts[iOut+1]=B[1];
      OutPts[iOut+2]=B[2];
      OutPts[iOut+3]=B[3];
      OutPtColors[iOutColor  ]=BC[0];
      OutPtColors[iOutColor+1]=BC[1];
      OutPtColors[iOutColor+2]=BC[2];
      OutPtTexCoords[iOutTexCoords  ]=BT[0];
      OutPtTexCoords[iOutTexCoords+1]=BT[1];
      OutPtTexCoords[iOutTexCoords+2]=BT[2];
      OutPtTexCoords[iOutTexCoords+3]=BT[3];
      iOut+=4;
      iOutColor+=3;
      iOutTexCoords+=4;
    }
  }
  return( iOut>>2 );  // RETURN NUMBER OF OutPts
}

//----------------------------------------------------------------------------
// Clips a 4D homogeneous polygon defined by the packed array of float InPts.
// to the viewing frustum defined by w components of the verts. The clipped polygon is
// put in OutPts (which must be a different location than InPts) and the number
// of vertices in the clipped polygon is returned. InPts must have NumInPts*4
// floats (enough to contain poly). Regular orthographic NDC clipping can be 
// achieved by making the w coordinate of each point be 1. OutPts will be
// allocated and return filled with the clipped polygon.
// Each vertex has an associated RGB floating point triple and (s,t,r,q) texture
// coordinates (floating point pair) that must be correctly interpolated to the 
// clip boundaries.
//----------------------------------------------------------------------------
int ClipPolyToFrustum_SMOOTH_TEX(float *InPts, float *InPtColors, float *InPtTexCoords, const int NumInPts, 
                                 float* &OutPts, float* &OutPtColors, float* &OutPtTexCoords)
{
  float *tInPts = new float[(NumInPts+6)*4];
  float *tInPtColors = new float[(NumInPts+6)*3];
  float *tInPtTexCoords = new float[(NumInPts+6)*4];
  OutPts = new float[(NumInPts+6)*4];
  OutPtColors = new float[(NumInPts+6)*3];
  OutPtTexCoords = new float[(NumInPts+6)*4];
  int i, NumOutPts;

  for (i=0; i<(NumInPts*4); i++) tInPts[i]=InPts[i];
  for (i=0; i<(NumInPts*3); i++) tInPtColors[i]=InPtColors[i];
  for (i=0; i<(NumInPts*4); i++) tInPtTexCoords[i]=InPtTexCoords[i];

  NumOutPts = ClipPolyToBoundary(tInPts,tInPtColors,tInPtTexCoords,NumInPts, OutPts,OutPtColors,OutPtTexCoords,0);
  NumOutPts = ClipPolyToBoundary(OutPts,OutPtColors,OutPtTexCoords,NumOutPts,tInPts,tInPtColors,tInPtTexCoords,1);
  NumOutPts = ClipPolyToBoundary(tInPts,tInPtColors,tInPtTexCoords,NumOutPts,OutPts,OutPtColors,OutPtTexCoords,2);
  NumOutPts = ClipPolyToBoundary(OutPts,OutPtColors,OutPtTexCoords,NumOutPts,tInPts,tInPtColors,tInPtTexCoords,3);
  NumOutPts = ClipPolyToBoundary(tInPts,tInPtColors,tInPtTexCoords,NumOutPts,OutPts,OutPtColors,OutPtTexCoords,4);
  NumOutPts = ClipPolyToBoundary(OutPts,OutPtColors,OutPtTexCoords,NumOutPts,tInPts,tInPtColors,tInPtTexCoords,5);

  for (i=0; i<(NumOutPts*4); i++) OutPts[i]=tInPts[i];
  for (i=0; i<(NumOutPts*3); i++) OutPtColors[i]=tInPtColors[i];
  for (i=0; i<(NumOutPts*4); i++) OutPtTexCoords[i]=tInPtTexCoords[i];

  delete[] tInPts;
  delete[] tInPtColors;
  delete[] tInPtTexCoords;

  return(NumOutPts);
}
