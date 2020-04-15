//============================================================================
// softgl_trirast_smooth_tex.cpp : perspectively incorrect version
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
// RETURNS POINTER TO RGB TEXEL VALUE (FLOATING POINT TRIPLE)
//----------------------------------------------------------------------------
static inline float* GetTexColor_CLAMP_NEAREST(float *Texture, const int TW, const int TH,
                                               float s, float t)
{
  int i,j;
  if (s<0) i=0; else if (s>=1) i=TW-1; else i=(int)(s*((float)TW));
  if (t<0) j=0; else if (t>=1) j=TH-1; else j=(int)(t*((float)TH));
  return( &(Texture[ (j*TW+i)*3 ]) );
}

static inline float* GetTexColor_REPEAT_NEAREST(float *Texture, const int TW, const int TH,
                                                float s, float t)
{
  s-=(float)((int)s);  if (s<0) s++;
  t-=(float)((int)t);  if (t<0) t++;
  int i = (int)(s*((float)TW));
  int j = (int)(t*((float)TH));
  return( &(Texture[ (j*TW+i)*3 ]) );
}

static inline void GetTexColor_CLAMP_LINEAR(float *Texture, const int TW, const int TH,
                                            float s, float t, float *FinalColor)
{
  int i0,j0,i1,j1;
  float u=s*(float)TW-0.5f, v=t*(float)TH-0.5f;
  if (u<0) i0=i1=0; else if (u>=TW-1) i0=i1=TW-1; else { i0=(int)u; i1=i0+1; }
  if (v<0) j0=j1=0; else if (v>=TH-1) j0=j1=TH-1; else { j0=(int)v; j1=j0+1; }
  int bl=(j0*TW+i0)*3, br=(j0*TW+i1)*3, tl=(j1*TW+i0)*3, tr=(j1*TW+i1)*3;
  float *t00=&(Texture[bl]), *t01=&(Texture[br]), *t10=&(Texture[tl]), *t11=&(Texture[tr]);
  s*=(float)TW;  s-=0.5f;  s-=(float)((int)s);  if (s<0) s++;
  t*=(float)TH;  t-=0.5f;  t-=(float)((int)t);  if (t<0) t++;
  float ts=1-s, tt=1-t;
  FinalColor[0] = tt*((ts)*t00[0] + s*t01[0]) + t*((ts)*t10[0] + s*t11[0]);
  FinalColor[1] = tt*((ts)*t00[1] + s*t01[1]) + t*((ts)*t10[1] + s*t11[1]);
  FinalColor[2] = tt*((ts)*t00[2] + s*t01[2]) + t*((ts)*t10[2] + s*t11[2]);
}

static inline void GetTexColor_REPEAT_LINEAR(float *Texture, const int TW, const int TH,
                                             float s, float t, float *FinalColor)
{
  s-=(float)((int)s);  if (s<0) s++;
  t-=(float)((int)t);  if (t<0) t++;
  int i0,j0,i1,j1;
  float u=s*(float)TW-0.5f, v=t*(float)TH-0.5f;
  if (u<0) { i0=TW-1; i1=0; } else if (u>=TW-1) { i0=TW-1; i1=0; } else { i0=(int)u; i1=i0+1; }
  if (v<0) { j0=TH-1; j1=0; } else if (v>=TH-1) { j0=TH-1; j1=0; } else { j0=(int)v; j1=j0+1; }
  int bl=(j0*TW+i0)*3, br=(j0*TW+i1)*3, tl=(j1*TW+i0)*3, tr=(j1*TW+i1)*3;
  float *t00=&(Texture[bl]), *t01=&(Texture[br]), *t10=&(Texture[tl]), *t11=&(Texture[tr]);
  s*=(float)TW;  s-=0.5f;  s-=(float)((int)s);  if (s<0) s++;
  t*=(float)TH;  t-=0.5f;  t-=(float)((int)t);  if (t<0) t++;
  float ts=1-s, tt=1-t;
  FinalColor[0] = tt*((ts)*t00[0] + s*t01[0]) + t*((ts)*t10[0] + s*t11[0]);
  FinalColor[1] = tt*((ts)*t00[1] + s*t01[1]) + t*((ts)*t10[1] + s*t11[1]);
  FinalColor[2] = tt*((ts)*t00[2] + s*t01[2]) + t*((ts)*t10[2] + s*t11[2]);
}                                                

//----------------------------------------------------------------------------
// Fill in interpolated values for one scanline span given the endpoint x boundaries
// (A and B endpoints) and perform Z-buffer visibility. Actual scanline Y floating
// point value should be cast to int before call (chop fraction).
//----------------------------------------------------------------------------
static void RasterizeSpan(unsigned char *Color, float *Depth, const int W, int ScanlineY, 
                          float Ax, float Az, float Ca[3], float Ta[2],
                          float Bx, float Bz, float Cb[3], float Tb[2],
                          float *Texture, const int TW, const int TH,
                          int Wrapping, int Filtering, int Function)
{
  // FIND STARTING AND ENDING PIXEL SAMPLE X VALUES (MIN BIASED)
  float MinX=FindMin(Ax), MaxX=FindMax(Bx);
  if (MinX>MaxX) return;  // POSSIBLE EARLY TERMINATION

  // CALCULATED PROPER INTERPOLATED VALUES FOR STARTING VALUE
  float ABx=Bx-Ax, ABz=Bz-Az, 
        ABr=Cb[0]-Ca[0], ABg=Cb[1]-Ca[1], ABb=Cb[2]-Ca[2],
        ABs=Tb[0]-Ta[0], ABt=Tb[1]-Ta[1];
  float t=(MinX-Ax)/ABx;
  float Z=Az+ABz*t;
  float R=Ca[0]+ABr*t;
  float G=Ca[1]+ABg*t;
  float B=Ca[2]+ABb*t;
  float S=Ta[0]+ABs*t;
  float T=Ta[1]+ABt*t;

  // CALCULATE RATES OF CHANGE OF INTERPOLATED VALUES W.R.T. CHANGES IN X-VALUE
  float dZ = ABz/ABx;
  float dR = ABr/ABx;
  float dG = ABg/ABx;
  float dB = ABb/ABx;
  float dS = ABs/ABx;
  float dT = ABt/ABx;

  // CALCULATE STARTING ADDRESS OF COLOR AND DEPTH BUFFERS FOR SCANLINE (reuse variables)
  int StartX=(int)MinX, EndX=(int)MaxX, i=ScanlineY*W+StartX;
  Color = &(Color[i*3]);
  Depth = &(Depth[i]);
  float *Texel, BiLinTexel[3];
  for (; StartX<=EndX; StartX++)
  {
    if (Z<(*Depth)) 
    {
      if (Filtering==0)
      {
        if (Wrapping==0) Texel = GetTexColor_CLAMP_NEAREST(Texture,TW,TH,S,T);
                    else Texel = GetTexColor_REPEAT_NEAREST(Texture,TW,TH,S,T);
      }                    
      else                  
      {
        if (Wrapping==0) GetTexColor_CLAMP_LINEAR(Texture,TW,TH,S,T,BiLinTexel);
                    else GetTexColor_REPEAT_LINEAR(Texture,TW,TH,S,T,BiLinTexel);
        Texel=BiLinTexel;                    
      }

      if (Function==0) // REPLACE
      {                         
        Color[0]=(unsigned char)(255.0f * Texel[0]);
        Color[1]=(unsigned char)(255.0f * Texel[1]);
        Color[2]=(unsigned char)(255.0f * Texel[2]);
      }
      else             // MODULATE
      {                         
        Color[0]=(unsigned char)(R * Texel[0]);
        Color[1]=(unsigned char)(G * Texel[1]);
        Color[2]=(unsigned char)(B * Texel[2]);
      }
      
      (*Depth)=Z;
    }
    Color+=3;
    Depth++;
    Z+=dZ;
    R+=dR;
    G+=dG;
    B+=dB;
    S+=dS;
    T+=dT;
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
// values are simply linearly interpolated across the face of the triangle (NOT
// perspectively correct). Per-vertex ST tex-coords are also linearly interpolated
// (NOT perspectively correct). Texture image of RGB floating point triplets
// of size TWxWH is given. Texture colors are normalized RGB values in [0,1].
//   clamp and repeat wrapping modes : Wrapping={0,1} 
//   nearest and bilinear filtering: Filtering={0,1} 
//   replace and modulate application modes: Function={0,1} 
//---------------------------------------------------------------------------
void RasterizeTriangle_SMOOTH_TEX(unsigned char *Color, float *Depth, const int W, const int H, 
                                  float A[3], float B[3], float C[3],
                                  float Ca[3], float Cb[3], float Cc[3], // PER-VERTEX RGB COLORS
                                  float Ta[2], float Tb[2], float Tc[2], // PER-VERTEX (S,T) TEX-COORDS
                                  float *Texture, const int TW, const int TH,
                                  int Wrapping, int Filtering, int Function)
{
  float *a=A, *b=B, *c=C, *t;   // TEMPORARY VERTEX SORTING PTRS
  float *ca=Ca, *cb=Cb, *cc=Cc;
  float *ta=Ta, *tb=Tb, *tc=Tc;

  // SORT VERTICES BY Y VALUES
  #define SWAP(a,b,t) t=a,a=b,b=t
  if (b[1]<a[1] && b[1]<=c[1]) 
    { SWAP(a,b,t); SWAP(ca,cb,t); SWAP(ta,tb,t); }
  else if (c[1]<a[1] && c[1]<=b[1]) 
    { SWAP(a,c,t); SWAP(ca,cc,t); SWAP(ta,tc,t); }
  if (c[1]<b[1]) 
    { SWAP(b,c,t); SWAP(cb,cc,t); SWAP(tb,tc,t); }

  // FIND START AND END SCANLINES FOR TWO SECTIONS SPLIT BY SHORT EDGES (biased towards min: includes min)
  float bStartY=FindMin(b[1]);
  float bEndY=FindMax(c[1]);
  float aStartY=FindMin(a[1]);
  float aEndY=bStartY-1.0f;

  // CALCULATE EDGE VALUE DIFFERENCES
  float ABx=b[0]-a[0], ABy=b[1]-a[1], ABz=b[2]-a[2],
        ABr=cb[0]-ca[0], ABg=cb[1]-ca[1], ABb=cb[2]-ca[2],
        ABs=tb[0]-ta[0], ABt=tb[1]-ta[1];
  float ACx=c[0]-a[0], ACy=c[1]-a[1], ACz=c[2]-a[2],
        ACr=cc[0]-ca[0], ACg=cc[1]-ca[1], ACb=cc[2]-ca[2],
        ACs=tc[0]-ta[0], ACt=tc[1]-ta[1];
  float BCx=c[0]-b[0], BCy=c[1]-b[1], BCz=c[2]-b[2],
        BCr=cc[0]-cb[0], BCg=cc[1]-cb[1], BCb=cc[2]-cb[2],
        BCs=tc[0]-tb[0], BCt=tc[1]-tb[1];

  // CALC LEFT AND RIGHT VALUES AND RATES OF CHANGE
  float ltX, rtX, ltDX, rtDX;
  float ltZ, rtZ, ltDZ, rtDZ;
  float ltC[3], rtC[3], ltDC[3], rtDC[3];
  float ltT[2], rtT[2], ltDT[2], rtDT[2];
  
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
      ltC[0]=ca[0]+ABr*t;
      ltC[1]=ca[1]+ABg*t;
      ltC[2]=ca[2]+ABb*t;
      ltT[0]=ta[0]+ABs*t;
      ltT[1]=ta[1]+ABt*t;
      ltDX=dABx;                       // LEFT RATES OF CHANGE
      ltDZ=ABz/ABy;
      ltDC[0]=ABr/ABy;
      ltDC[1]=ABg/ABy;
      ltDC[2]=ABb/ABy;
      ltDT[0]=ABs/ABy;
      ltDT[1]=ABt/ABy;
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtC[0]=ca[0]+ACr*t;
      rtC[1]=ca[1]+ACg*t;
      rtC[2]=ca[2]+ACb*t;
      rtT[0]=ta[0]+ACs*t;
      rtT[1]=ta[1]+ACt*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
      rtDC[0]=ACr/ACy;
      rtDC[1]=ACg/ACy;
      rtDC[2]=ACb/ACy;
      rtDT[0]=ACs/ACy;
      rtDT[1]=ACt/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=StartDY/ACy;                   // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltC[0]=ca[0]+ACr*t;
      ltC[1]=ca[1]+ACg*t;
      ltC[2]=ca[2]+ACb*t;
      ltT[0]=ta[0]+ACs*t;
      ltT[1]=ta[1]+ACt*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      ltDC[0]=ACr/ACy;
      ltDC[1]=ACg/ACy;
      ltDC[2]=ACb/ACy;
      ltDT[0]=ACs/ACy;
      ltDT[1]=ACt/ACy;
      t=StartDY/ABy;                   // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=a[0]+ABx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ABz*t;
      rtC[0]=ca[0]+ABr*t;
      rtC[1]=ca[1]+ABg*t;
      rtC[2]=ca[2]+ABb*t;
      rtT[0]=ta[0]+ABs*t;
      rtT[1]=ta[1]+ABt*t;
      rtDX=dABx;                       // RIGHT RATES OF CHANGE
      rtDZ=ABz/ABy;
      rtDC[0]=ABr/ABy;
      rtDC[1]=ABg/ABy;
      rtDC[2]=ABb/ABy;
      rtDT[0]=ABs/ABy;
      rtDT[1]=ABt/ABy;
    }
    for (; aStartY<=aEndY; aStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,(int)aStartY,ltX,ltZ,ltC,ltT,rtX,rtZ,rtC,rtT,
                    Texture,TW,TH,Wrapping,Filtering,Function);
      ltX+=ltDX;
      ltZ+=ltDZ;
      ltC[0]+=ltDC[0]; ltC[1]+=ltDC[1]; ltC[2]+=ltDC[2];
      ltT[0]+=ltDT[0]; ltT[1]+=ltDT[1];
      rtX+=rtDX;
      rtZ+=rtDZ;
      rtC[0]+=rtDC[0]; rtC[1]+=rtDC[1]; rtC[2]+=rtDC[2];
      rtT[0]+=rtDT[0]; rtT[1]+=rtDT[1];
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
      ltC[0]=cb[0]+BCr*t;
      ltC[1]=cb[1]+BCg*t;
      ltC[2]=cb[2]+BCb*t;
      ltT[0]=tb[0]+BCs*t;
      ltT[1]=tb[1]+BCt*t;
      ltDX=dBCx;                       // LEFT RATES OF CHANGE
      ltDZ=BCz/BCy;
      ltDC[0]=BCr/BCy;
      ltDC[1]=BCg/BCy;
      ltDC[2]=BCb/BCy;
      ltDT[0]=BCs/BCy;
      ltDT[1]=BCt/BCy;
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      rtX=a[0]+ACx*t;                  // RIGHT STARTING VALUES
      rtZ=a[2]+ACz*t;
      rtC[0]=ca[0]+ACr*t;
      rtC[1]=ca[1]+ACg*t;
      rtC[2]=ca[2]+ACb*t;
      rtT[0]=ta[0]+ACs*t;
      rtT[1]=ta[1]+ACt*t;
      rtDX=dACx;                       // RIGHT RATES OF CHANGE
      rtDZ=ACz/ACy;
      rtDC[0]=ACr/ACy;
      rtDC[1]=ACg/ACy;
      rtDC[2]=ACb/ACy;
      rtDT[0]=ACs/ACy;
      rtDT[1]=ACt/ACy;
    }
    else                               // ELSE AC IS ON THE LEFT
    {
      t=acStartDY/ACy;                 // FRACTION ALONG AC FOR STARTING SCANLINE
      ltX=a[0]+ACx*t;                  // LEFT STARTING VALUES
      ltZ=a[2]+ACz*t;
      ltC[0]=ca[0]+ACr*t;
      ltC[1]=ca[1]+ACg*t;
      ltC[2]=ca[2]+ACb*t;
      ltT[0]=ta[0]+ACs*t;
      ltT[1]=ta[1]+ACt*t;
      ltDX=dACx;                       // LEFT RATES OF CHANGE
      ltDZ=ACz/ACy;
      ltDC[0]=ACr/ACy;
      ltDC[1]=ACg/ACy;
      ltDC[2]=ACb/ACy;
      ltDT[0]=ACs/ACy;
      ltDT[1]=ACt/ACy;
      t=bcStartDY/BCy;                 // FRACTION ALONG AB FOR STARTING SCANLINE
      rtX=b[0]+BCx*t;                  // RIGHT STARTING VALUES
      rtZ=b[2]+BCz*t;
      rtC[0]=cb[0]+BCr*t;
      rtC[1]=cb[1]+BCg*t;
      rtC[2]=cb[2]+BCb*t;
      rtT[0]=tb[0]+BCs*t;
      rtT[1]=tb[1]+BCt*t;
      rtDX=dBCx;                       // RIGHT RATES OF CHANGE
      rtDZ=BCz/BCy;
      rtDC[0]=BCr/BCy;
      rtDC[1]=BCg/BCy;
      rtDC[2]=BCb/BCy;
      rtDT[0]=BCs/BCy;
      rtDT[1]=BCt/BCy;
    }
    for (; bStartY<=bEndY; bStartY++)  // INCREMENTALLY COMPUTE LT,RT VALUES FOR EACH SCANLINE
    {
      RasterizeSpan(Color,Depth,W,(int)bStartY,ltX,ltZ,ltC,ltT,rtX,rtZ,rtC,rtT,
                    Texture,TW,TH,Wrapping,Filtering,Function);
      ltX+=ltDX;
      ltZ+=ltDZ;
      ltC[0]+=ltDC[0]; ltC[1]+=ltDC[1]; ltC[2]+=ltDC[2];
      ltT[0]+=ltDT[0]; ltT[1]+=ltDT[1];
      rtX+=rtDX;
      rtZ+=rtDZ;
      rtC[0]+=rtDC[0]; rtC[1]+=rtDC[1]; rtC[2]+=rtDC[2];
      rtT[0]+=rtDT[0]; rtT[1]+=rtDT[1];
    }
  }
}
