#include "stdafx.h"
//============================================================================
// softgl_homoclip_flat.cpp : polygon clipping in homogeneous coordinates
//============================================================================

static inline void LinearInterp(const float A[4], const float B[4], float C[4], const float t)
{
  C[0] = A[0] + (B[0]-A[0])*t;
  C[1] = A[1] + (B[1]-A[1])*t;
  C[2] = A[2] + (B[2]-A[2])*t;
  C[3] = A[3] + (B[3]-A[3])*t;
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
static int ClipPolyToBoundary(float *InPts, const int NumInPts, float *OutPts, const int WhichBoundary)
{
  float *A, *B;                                  // CURRENT EDGE ENDPOINTS
  int Aout, Bout;                                // CURRENT EDGE ENDPOINT "OUTSIDE" CONDITION
  float t;                                       // EDGE PARAMETRIC VALUE (FOR PLANE ISECT CALC)

  int iOut=0, N=(NumInPts<<2);
  for (int i=0; i<N; i+=4)
  {
    A = &(InPts[i]);                             // SET CURRENT EDGE ENDPOINTS
    B = &(InPts[(i+4)%N]);

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
      LinearInterp(A,B,&(OutPts[iOut]),t);
      iOut+=4; 
    }

    if ( (Aout && !Bout) || (!Aout && !Bout) )   // OUT->IN, IN->IN : OUTPUT 2ND EDGE VERTEX
    { 
      OutPts[iOut]=B[0];
      OutPts[iOut+1]=B[1];
      OutPts[iOut+2]=B[2];
      OutPts[iOut+3]=B[3];
      iOut+=4;
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
//----------------------------------------------------------------------------
int ClipPolyToFrustum_FLAT(float *InPts, const int NumInPts, float* &OutPts)
{
  float *tInPts = new float[(NumInPts+6)*4];
  OutPts = new float[(NumInPts+6)*4];
  int i, NumOutPts;

  for (i=0; i<(NumInPts*4); i++) tInPts[i]=InPts[i];

  NumOutPts = ClipPolyToBoundary(tInPts,NumInPts,OutPts,0);
  NumOutPts = ClipPolyToBoundary(OutPts,NumOutPts,tInPts,1);
  NumOutPts = ClipPolyToBoundary(tInPts,NumOutPts,OutPts,2);
  NumOutPts = ClipPolyToBoundary(OutPts,NumOutPts,tInPts,3);
  NumOutPts = ClipPolyToBoundary(tInPts,NumOutPts,OutPts,4);
  NumOutPts = ClipPolyToBoundary(OutPts,NumOutPts,tInPts,5);

  for (i=0; i<(NumOutPts*4); i++) OutPts[i]=tInPts[i];

  delete[] tInPts;
  return(NumOutPts);
}
