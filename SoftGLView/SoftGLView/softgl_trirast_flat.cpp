//============================================================================
// softgl_trirast_flat.cpp
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
static void RasterizeSpan(unsigned char *Color, float *Depth, const int W, const int H,
                          int ScanlineY, float Ax, float Az, float Bx, float Bz,
                          unsigned char CurrentColor[3])
{
  // FIND STARTING AND ENDING PIXEL SAMPLE X VALUES (MIN BIASED)
  float MinX=FindMin(Ax), MaxX=FindMax(Bx);
  if (MinX>MaxX) return;  // POSSIBLE EARLY TERMINATION

  // CALCULATED PROPER INTERPOLATED VALUES FOR STARTING VALUE
  float ABx=(Bx-Ax), ABz=(Bz-Az);
  float t=(MinX-Ax)/ABx;
  float Z=Az+ABz*t;

  // CALCULATE RATES OF CHANGE OF INTERPOLATED VALUES W.R.T. CHANGES IN X-VALUE
  float dZ = ABz/ABx;

  // CALCULATE STARTING ADDRESS OF COLOR AND DEPTH BUFFERS FOR SCANLINE (reuse variables)
  int StartX=(int)MinX, EndX=(int)MaxX, i=ScanlineY*W+StartX;
  Color = &(Color[i*3]);
  Depth = &(Depth[i]);
  for (; StartX<=EndX; StartX++)
  {
    if (Z<(*Depth)) 
    {
      Color[0]=CurrentColor[0];
      Color[1]=CurrentColor[1];
      Color[2]=CurrentColor[2];
      (*Depth)=Z;
    }
    Color+=3;
    Depth++;
    Z+=dZ;
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
//---------------------------------------------------------------------------
void RasterizeTriangle_FLAT(unsigned char *Color, float *Depth, const int W, const int H, 
                            float A[3], float B[3], float C[3],
                            unsigned char CurrentColor[3])
{
  float *a=A, *b=B, *c=C, *t; // TEMPORARY VERTEX SORTING PTRS

  // SORT VERTICES BY Y VALUES
  #define SWAP(a,b,t) t=a,a=b,b=t
  if (b[1]<a[1] && b[1]<=c[1]) SWAP(a,b,t); else if (c[1]<a[1] && c[1]<=b[1]) SWAP(a,c,t);
  if (c[1]<b[1]) SWAP(b,c,t);

  // FIND START AND END SCANLINES FOR TWO SECTIONS SPLIT BY SHORT EDGES (biased towards min: includes min)
  float bStartY=FindMin(b[1]);
  float bEndY=FindMax(c[1]);
  float aStartY=FindMin(a[1]);
  float aEndY=bStartY-1.0f;

  // CALCULATE EDGE VALUE DIFFERENCES
  float ABx=b[0]-a[0], ABy=b[1]-a[1], ABz=b[2]-a[2];
  float ACx=c[0]-a[0], ACy=c[1]-a[1], ACz=c[2]-a[2];
  float BCx=c[0]-b[0], BCy=c[1]-b[1], BCz=c[2]-b[2];

  // CALC LEFT AND RIGHT VALUES AND RATES OF CHANGE
  float ltX, rtX, ltDX, rtDX;
  float ltZ, rtZ, ltDZ, rtDZ;
  
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
      ltDX=dABx;                       // LEFT RATES OF CHANGE
      ltDZ=ABz/ABy;
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      t=StartDY/ABy;                   // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=a[0]+ABx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ABz*t;
      rtDX=dABx;                       // RIGHT RATES OF CHANGE
      rtDZ=ABz/ABy;
    }
    for (; aStartY<=aEndY; aStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,H,(int)aStartY,ltX,ltZ,rtX,rtZ,CurrentColor);
      ltX+=ltDX;
      ltZ+=ltDZ;
      rtX+=rtDX;
      rtZ+=rtDZ;
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
      ltDX=dBCx;                       // LEFT RATES OF CHANGE
      ltDZ=BCz/BCy;
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      t=bcStartDY/BCy;                 // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=b[0]+BCx*t;                  // RIGHT STARTING VALUES
      rtZ=b[2]+BCz*t;
      rtDX=dBCx;                       // RIGHT RATES OF CHANGE
      rtDZ=BCz/BCy;
    }
    for (; bStartY<=bEndY; bStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,H,(int)bStartY,ltX,ltZ,rtX,rtZ,CurrentColor);
      ltX+=ltDX;
      ltZ+=ltDZ;
      rtX+=rtDX;
      rtZ+=rtDZ;
    }
  }
}
