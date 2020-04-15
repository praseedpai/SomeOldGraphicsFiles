//============================================================================
// softgl_trirast_smooth_pc.hpp : with perspectively correct (PC) color interpolation
//============================================================================
#include "stdafx.h"
//----------------------------------------------------------------------------
// CLAMP TO PIXEL VALUE THAT IS "INSIDE" BIASED TOWARDS MIN (INCLUDES MIN)
//----------------------------------------------------------------------------
static inline float FindMin(const float MinBound)
{
  float CntrVal = ((float)((int)MinBound)) + 0.5f;
  return( (MinBound>CntrVal) ? CntrVal+1.0f : CntrVal );
}

static inline float FindMax(const float MaxBound)
{
  float CntrVal = ((float)((int)MaxBound)) + 0.5f;
  return( (MaxBound<=CntrVal) ? CntrVal-1.0f : CntrVal );
}

//----------------------------------------------------------------------------
// Fill in interpolated values for one scanline span given the endpoint x boundaries
// (A and B endpoints) and perform Z-buffer visibility. Actual scanline Y floating
// point value should be cast to int before call (chop fraction).
//----------------------------------------------------------------------------
static void RasterizeSpan(unsigned char *Color, float *Depth, const int Width, int ScanlineY, 
                          float Ax, float Az, float Aw, float Ca[3],
                          float Bx, float Bz, float Bw, float Cb[3])
{
  // FIND STARTING AND ENDING PIXEL SAMPLE X VALUES (MIN BIASED)
  float MinX=FindMin(Ax), MaxX=FindMax(Bx);
  if (MinX>MaxX) return;  // POSSIBLE EARLY TERMINATION

  // CALCULATED PROPER INTERPOLATED VALUES FOR STARTING VALUE
  float ABx=Bx-Ax, ABz=Bz-Az, ABw=Bw-Aw, ABr=Cb[0]-Ca[0], ABg=Cb[1]-Ca[1], ABb=Cb[2]-Ca[2];
  float t=(MinX-Ax)/ABx;
  float Z=Az+ABz*t;
  float W=Aw+ABw*t;
  float R=Ca[0]+ABr*t;
  float G=Ca[1]+ABg*t;
  float B=Ca[2]+ABb*t;

  // CALCULATE RATES OF CHANGE OF INTERPOLATED VALUES W.R.T. CHANGES IN X-VALUE
  float dZ = ABz/ABx;
  float dW = ABw/ABx;
  float dR = ABr/ABx;
  float dG = ABg/ABx;
  float dB = ABb/ABx;

  // CALCULATE STARTING ADDRESS OF COLOR AND DEPTH BUFFERS FOR SCANLINE (reuse variables)
  int StartX=(int)MinX, EndX=(int)MaxX, i=ScanlineY*Width+StartX;
  Color = &(Color[i*3]);
  Depth = &(Depth[i]);
  for (; StartX<=EndX; StartX++)
  {
    if (Z<(*Depth)) 
    {
      Color[0]=(unsigned char)(R/W);
      Color[1]=(unsigned char)(G/W);
      Color[2]=(unsigned char)(B/W);
      (*Depth)=Z;
    }
    Color+=3;
    Depth++;
    Z+=dZ;
    W+=dW;
    R+=dR;
    G+=dG;
    B+=dB;
  }
}

//----------------------------------------------------------------------------
// Given 3D triangle ABC in screen space with clipped coordinates within the following
// bounds: x in [0,W], y in [0,H], z in [0,1]. The origin for (x,y) is in the bottom
// left corner of the pixel grid. z=0 is the near plane and z=1 is the far plane,
// so lesser values are closer. The coordinates of the pixels are evenly spaced
// in x and y 1 units apart starting at the bottom-left pixel with coords
// (0.5,0.5). In other words, the pixel sample point is in the center of the
// rectangular grid cell containing the pixel sample. The framebuffer has
// dimensions width x height (WxH). The Color buffer is a 1D array (row-major
// order) with 3 unsigned chars per pixel (24-bit color). The Depth buffer is 
// a 1D array (also row-major order) with a float value per pixel
// For a pixel location (x,y) we can obtain
// the Color and Depth array locations as: Color[(((int)y)*W+((int)x))*3] 
// (for the red value, green is offset +1, and blue is offset +2 and
// Depth[((int)y)*W+((int)x)]. Fills the pixels contained in the triangle
// with the global current color and the properly linearly interpolated depth
// value (performs Z-buffer depth test before writing new pixel). 
// Pixel samples that lie inside the triangle edges are filled with
// a bias towards the minimum values (samples that lie exactly on a triangle
// edge are filled only for minimum x values along a horizontal span and for
// minimum y values, samples lying on max values are not filled).
// Per-vertex colors are RGB floating point triplets in [0.0,255.0]. These color
// values are linearly interpolated perspectively correct across the face of 
// the triangle. The vertices A, B, and C include their w-components for use
// in perspectively correct interpolation. Actually, the fourth component
// will store 1/w of the original vertex. The colors are also perspectively
// divided, so for example Ca[0] is the original Ca[0] divided by w. The 
// 1/w values are interpolated in screen space and at each pixel, we divide 
// the colors by the interpolated 1/w value to obtain the perspectively 
// correct values.
//---------------------------------------------------------------------------
void RasterizeTriangle_SMOOTH_PC(unsigned char *Color, float *Depth, const int W, const int H, 
                                 float A[4], float B[4], float C[4],
                                 float Ca[3], float Cb[3], float Cc[3])  // PER-VERTEX RGB COLORS
{
  float *a=A, *b=B, *c=C, *t;   // TEMPORARY VERTEX SORTING PTRS
  float *ca=Ca, *cb=Cb, *cc=Cc;

  // SORT VERTICES BY Y VALUES
  #define SWAP(a,b,t) t=a,a=b,b=t
  if (b[1]<a[1] && b[1]<=c[1]) 
    { SWAP(a,b,t); SWAP(ca,cb,t); }
  else if (c[1]<a[1] && c[1]<=b[1]) 
    { SWAP(a,c,t); SWAP(ca,cc,t); }
  if (c[1]<b[1]) 
    { SWAP(b,c,t); SWAP(cb,cc,t); }

  // FIND START AND END SCANLINES FOR TWO SECTIONS SPLIT BY SHORT EDGES (biased towards min: includes min)
  float bStartY=FindMin(b[1]);
  float bEndY=FindMax(c[1]);
  float aStartY=FindMin(a[1]);
  float aEndY=bStartY-1.0f;

  // CALCULATE EDGE VALUE DIFFERENCES
  float ABx=b[0]-a[0], ABy=b[1]-a[1], ABz=b[2]-a[2], ABw=b[3]-a[3],
        ABr=cb[0]-ca[0], ABg=cb[1]-ca[1], ABb=cb[2]-ca[2];
  float ACx=c[0]-a[0], ACy=c[1]-a[1], ACz=c[2]-a[2], ACw=c[3]-a[3],
        ACr=cc[0]-ca[0], ACg=cc[1]-ca[1], ACb=cc[2]-ca[2];
  float BCx=c[0]-b[0], BCy=c[1]-b[1], BCz=c[2]-b[2], BCw=c[3]-b[3],
        BCr=cc[0]-cb[0], BCg=cc[1]-cb[1], BCb=cc[2]-cb[2];

  // CALC LEFT AND RIGHT VALUES AND RATES OF CHANGE
  float ltX, rtX, ltDX, rtDX;
  float ltZ, rtZ, ltDZ, rtDZ;
  float ltW, rtW, ltDW, rtDW;
  float ltC[3], rtC[3], ltDC[3], rtDC[3];
  
  // RASTERIZE FIRST SECTION
  if (aStartY<=aEndY)
  {
    float dABx=ABx/ABy, dACx=ACx/ACy;  // CALC RATE OF CHANGES OF X VALUE W.R.T. Y VALUES
    float StartDY=aStartY-a[1];        // CALC DIFFERENCE BETWEEN STARTING SCANLINE AND A's Y-val
    float t;
    if (dABx<dACx)                     // IF AB IS ON THE LEFT
    {
      t=StartDY/ABy;                   // FRACTION ALONG AB FOR STARTING SCANLINE
      ltX=a[0]+ABx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ABz*t;
      ltW=a[3]+ABw*t;
      ltC[0]=ca[0]+ABr*t;
      ltC[1]=ca[1]+ABg*t;
      ltC[2]=ca[2]+ABb*t;
      ltDX=dABx;                       // LEFT RATES OF CHANGE
      ltDZ=ABz/ABy;
      ltDW=ABw/ABy;
      ltDC[0]=ABr/ABy;
      ltDC[1]=ABg/ABy;
      ltDC[2]=ABb/ABy;
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtW=a[3]+ACw*t;
      rtC[0]=ca[0]+ACr*t;
      rtC[1]=ca[1]+ACg*t;
      rtC[2]=ca[2]+ACb*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
      rtDW=ACw/ACy;
      rtDC[0]=ACr/ACy;
      rtDC[1]=ACg/ACy;
      rtDC[2]=ACb/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltW=a[3]+ACw*t;
      ltC[0]=ca[0]+ACr*t;
      ltC[1]=ca[1]+ACg*t;
      ltC[2]=ca[2]+ACb*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      ltDW=ACw/ACy;
      ltDC[0]=ACr/ACy;
      ltDC[1]=ACg/ACy;
      ltDC[2]=ACb/ACy;
      t=StartDY/ABy;                   // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=a[0]+ABx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ABz*t;
      rtW=a[3]+ABw*t;
      rtC[0]=ca[0]+ABr*t;
      rtC[1]=ca[1]+ABg*t;
      rtC[2]=ca[2]+ABb*t;
      rtDX=dABx;                       // RIGHT RATES OF CHANGE
      rtDZ=ABz/ABy;
      rtDW=ABw/ABy;
      rtDC[0]=ABr/ABy;
      rtDC[1]=ABg/ABy;
      rtDC[2]=ABb/ABy;
    }
    for (; aStartY<=aEndY; aStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,(int)aStartY,ltX,ltZ,ltW,ltC,rtX,rtZ,rtW,rtC);
      ltX+=ltDX;
      ltZ+=ltDZ;
      ltW+=ltDW;
      ltC[0]+=ltDC[0]; ltC[1]+=ltDC[1]; ltC[2]+=ltDC[2];
      rtX+=rtDX;
      rtZ+=rtDZ;
      rtW+=rtDW;
      rtC[0]+=rtDC[0]; rtC[1]+=rtDC[1]; rtC[2]+=rtDC[2];
    }
  }

  // RASTERIZE SECOND SECTION
  if (bStartY<=bEndY)
  {
    float dBCx=BCx/BCy, dACx=ACx/ACy;  // CALC RATE OF CHANGES OF X VALUE W.R.T. Y VALUES
    float bcStartDY=bStartY-b[1];      // CALC DIFFERENCE BETWEEN STARTING SCANLINE AND B's Y-val
    float acStartDY=bStartY-a[1];      // CALC DIFFERENCE BETWEEN STARTING SCANLINE AND A's Y-val
    float t;
    if (dBCx>dACx)                     // IF BC IS ON THE LEFT
    {
      t=bcStartDY/BCy;                 // FRACTION ALONG BC FOR STARTING SCANLINE
      ltX=b[0]+BCx*t;                  // LEFT STARTING VALUES
      ltZ=b[2]+BCz*t;
      ltW=b[3]+BCw*t;
      ltC[0]=cb[0]+BCr*t;
      ltC[1]=cb[1]+BCg*t;
      ltC[2]=cb[2]+BCb*t;
      ltDX=dBCx;                       // LEFT RATES OF CHANGE
      ltDZ=BCz/BCy;
      ltDW=BCw/BCy;
      ltDC[0]=BCr/BCy;
      ltDC[1]=BCg/BCy;
      ltDC[2]=BCb/BCy;
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtW=a[3]+ACw*t;
      rtC[0]=ca[0]+ACr*t;
      rtC[1]=ca[1]+ACg*t;
      rtC[2]=ca[2]+ACb*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
      rtDW=ACw/ACy;
      rtDC[0]=ACr/ACy;
      rtDC[1]=ACg/ACy;
      rtDC[2]=ACb/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltW=a[3]+ACw*t;
      ltC[0]=ca[0]+ACr*t;
      ltC[1]=ca[1]+ACg*t;
      ltC[2]=ca[2]+ACb*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      ltDW=ACw/ACy;
      ltDC[0]=ACr/ACy;
      ltDC[1]=ACg/ACy;
      ltDC[2]=ACb/ACy;
      t=bcStartDY/BCy;                 // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=b[0]+BCx*t;                  // RIGHT STARTING VALUES
      rtZ=b[2]+BCz*t;
      rtW=b[3]+BCw*t;
      rtC[0]=cb[0]+BCr*t;
      rtC[1]=cb[1]+BCg*t;
      rtC[2]=cb[2]+BCb*t;
      rtDX=dBCx;                       // RIGHT RATES OF CHANGE
      rtDZ=BCz/BCy;
      rtDW=BCw/BCy;
      rtDC[0]=BCr/BCy;
      rtDC[1]=BCg/BCy;
      rtDC[2]=BCb/BCy;
    }
    for (; bStartY<=bEndY; bStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,(int)bStartY,ltX,ltZ,ltW,ltC,rtX,rtZ,rtW,rtC);
      ltX+=ltDX;
      ltZ+=ltDZ;
      ltW+=ltDW;
      ltC[0]+=ltDC[0]; ltC[1]+=ltDC[1]; ltC[2]+=ltDC[2];
      rtX+=rtDX;
      rtZ+=rtDZ;
      rtW+=rtDW;
      rtC[0]+=rtDC[0]; rtC[1]+=rtDC[1]; rtC[2]+=rtDC[2];
    }
  }
}
