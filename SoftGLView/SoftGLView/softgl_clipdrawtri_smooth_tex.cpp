//============================================================================
// softgl_clipdrawtri_smooth_tex.cpp
//============================================================================
#include "stdafx.h"
#include "softgl.hpp"
#include "softgl_homoclip_smooth_tex.hpp"
#include "softgl_trirast_smooth_tex.hpp"
#include "softgl_trirast_smooth_tex_pc.hpp"

// VERTICES:   STORE 1/W IN W COMPONENT
// COLORS:     DIVIDE BY W
// TEX-COORDS: DIVIDE BY W
void SoftGL::PerspectiveDivide_PerspCorrect_Color_Tex(float *V, float *C, float *T, int N)
{
  for (int i=0, iV=3, iC=0, iT=0; i<N; i++, iV+=4, iC+=3, iT+=4)
  {
    V[iV]=1.0f/V[iV];
    C[iC  ]*=V[iV];
    C[iC+1]*=V[iV];
    C[iC+2]*=V[iV];
    T[iT  ]*=V[iV];
    T[iT+1]*=V[iV];
    T[iT+2]*=V[iV];
    T[iT+3]*=V[iV];
  }
}

void SoftGL::ClipAndDrawCurrentTri_SMOOTH_TEX()
{
  int i;

  // CHECK WHICH WAY THE TRI IS FACING (BACK OR FRONT, IF NEEDED FOR BACKFACE CULLING OR 2-SIDEDLIGHTING)
  int IsBackfacing=0;
  if (BackfaceCullingEnabled || (LightingEnabled && LightModelTwoSide))
    IsBackfacing = CullBackfacingEyeSpaceTri(Verts[0].EyeCoords,Verts[1].EyeCoords,Verts[2].EyeCoords);

  // PERFORM BACKFACE CULLING, IF ENABLED
  if (BackfaceCullingEnabled && IsBackfacing) return;

  // TRANSFORM EYE-SPACE VERTICES TO HOMOGENEOUS SPACE FOR CLIPPING (1D ARRAY FOR CLIPPER)
  float HomoVerts[12];
  Mult16fv4fv(      HomoVerts,Projection->Top(),Verts[0].EyeCoords);
  Mult16fv4fv(&(HomoVerts[4]),Projection->Top(),Verts[1].EyeCoords);
  Mult16fv4fv(&(HomoVerts[8]),Projection->Top(),Verts[2].EyeCoords);

  // PERFORM VIEW-FRUSTUM CULLING FOR TRIVIAL REJECTION OR ACCEPTANCE
  int VFCstatus = HomogeneousViewFrustumCull(HomoVerts);
  if (VFCstatus==1) return;  // TRIANGLE IS COMPLETELY OUTSIDE OF FRUSTUM

  // CALCULATE LIGHTING FOR TRIANGLE VERTICES (IGNORE ALPHA)
  // USES CURRENT VERTEX, NORMAL, LIGHTS, AND MATERIALS TO CALC THE VERTEX COLOR
  if (LightingEnabled)
  {
    CalcLighting(Verts[0].Color, Verts[0].EyeCoords, Verts[0].Normal, Materials[0], IsBackfacing);
    CalcLighting(Verts[1].Color, Verts[1].EyeCoords, Verts[1].Normal, Materials[1], IsBackfacing);
    CalcLighting(Verts[2].Color, Verts[2].EyeCoords, Verts[2].Normal, Materials[2], IsBackfacing);
  }

  // COPY VERTEX COLORS INTO A 1D ARRAY FOR USE BY THE CLIPPER (SCALE INTO [0,255])
  float VertColors[] = { Verts[0].Color[0]*255, Verts[0].Color[1]*255, Verts[0].Color[2]*255, 
                         Verts[1].Color[0]*255, Verts[1].Color[1]*255, Verts[1].Color[2]*255, 
                         Verts[2].Color[0]*255, Verts[2].Color[1]*255, Verts[2].Color[2]*255 };

  // COPY 4D TEX COORDS INTO A 1D ARRAY FOR USE BY THE CLIPPER
  float VertTexCoords[] = { Verts[0].TexCoords[0], Verts[0].TexCoords[1], Verts[0].TexCoords[2], Verts[0].TexCoords[3],
                            Verts[1].TexCoords[0], Verts[1].TexCoords[1], Verts[1].TexCoords[2], Verts[1].TexCoords[3],
                            Verts[2].TexCoords[0], Verts[2].TexCoords[1], Verts[2].TexCoords[2], Verts[2].TexCoords[3] };

  // CLIP TO VIEWING FRUSTUM IN HOMOGENEOUS SPACE
  float *ClippedPolyPts;
  float *ClippedPolyPtColors;
  float *ClippedPolyPtTexCoords;
  int NumClippedPolyPts;
  if (VFCstatus==-1)  // TRIANGLE IS COMPLETELY INSIDE FRUSTUM, NO NEED TO CLIP
  {
    ClippedPolyPts=HomoVerts;
    ClippedPolyPtColors=VertColors;
    ClippedPolyPtTexCoords=VertTexCoords;
    NumClippedPolyPts=3;  
  }
  else  // VFCstatus==0, POTENTIALLY INTERSECTS FRUSTUM BOUNDARIES, SO CLIP
  {
    NumClippedPolyPts = ClipPolyToFrustum_SMOOTH_TEX(HomoVerts,VertColors,VertTexCoords,3,
                                                     ClippedPolyPts,ClippedPolyPtColors,ClippedPolyPtTexCoords);
  }

  PerspectiveDivide_ViewportXform(ClippedPolyPts,NumClippedPolyPts);

  if (UsePerspCorrectInterpolation)  // DIVIDE ALL COLOR AND TEXTURE COMPONENTS BY W, STORE 1/W
    PerspectiveDivide_PerspCorrect_Color_Tex(ClippedPolyPts,ClippedPolyPtColors,ClippedPolyPtTexCoords,NumClippedPolyPts);

  // TRIANGULATE POLYGON AND RASTERIZE PIXEL SCREEN SPACE TRIANGLES
  #ifndef TURN_OFF_RASTERIZATION
    if (UsePerspCorrectInterpolation)
    {
      for (i=1; i<(NumClippedPolyPts-1); i++)
        RasterizeTriangle_SMOOTH_TEX_PC(ColorBuffer,DepthBuffer,PixelWidth,PixelHeight, 
                                        ClippedPolyPts, &(ClippedPolyPts[i*4]), &(ClippedPolyPts[(i+1)*4]),
                                        ClippedPolyPtColors, &(ClippedPolyPtColors[i*3]), &(ClippedPolyPtColors[(i+1)*3]),
                                        ClippedPolyPtTexCoords, &(ClippedPolyPtTexCoords[i*4]), &(ClippedPolyPtTexCoords[(i+1)*4]),
                                        Textures[iCurrentBoundTexture]->Texels, 
                                        Textures[iCurrentBoundTexture]->W, Textures[iCurrentBoundTexture]->H,
                                        Textures[iCurrentBoundTexture]->WrapS, Textures[iCurrentBoundTexture]->MinFilter,
                                        TexEnvMode);
    }
    else
    {
      for (i=1; i<(NumClippedPolyPts-1); i++)
        RasterizeTriangle_SMOOTH_TEX(ColorBuffer,DepthBuffer,PixelWidth,PixelHeight, 
                                     ClippedPolyPts, &(ClippedPolyPts[i*4]), &(ClippedPolyPts[(i+1)*4]),
                                     ClippedPolyPtColors, &(ClippedPolyPtColors[i*3]), &(ClippedPolyPtColors[(i+1)*3]),
                                     ClippedPolyPtTexCoords, &(ClippedPolyPtTexCoords[i*4]), &(ClippedPolyPtTexCoords[(i+1)*4]),
                                     Textures[iCurrentBoundTexture]->Texels, 
                                     Textures[iCurrentBoundTexture]->W, Textures[iCurrentBoundTexture]->H,
                                     Textures[iCurrentBoundTexture]->WrapS, Textures[iCurrentBoundTexture]->MinFilter,
                                     TexEnvMode);
    }
  #else  // JUST DRAW VERTEX POINTS
    for (i=0; i<NumClippedPolyPts*4; i+=4)
    {
      int j = (((int)(ClippedPolyPts[i+1]))*PixelWidth+((int)(ClippedPolyPts[i])))*3;
      ColorBuffer[j]  =(unsigned char)(CurrentColor[0]*255);
      ColorBuffer[j+1]=(unsigned char)(CurrentColor[1]*255);
      ColorBuffer[j+2]=(unsigned char)(CurrentColor[2]*255);
    }
  #endif

  if (VFCstatus==0)  // IF TRIANGLE WAS CLIPPED, FREE THE CLIPPER-ALLOCATED MEMORY
  {
    delete[] ClippedPolyPts;
    delete[] ClippedPolyPtColors;
    delete[] ClippedPolyPtTexCoords;
  }
}
