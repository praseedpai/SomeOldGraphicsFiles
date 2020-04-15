//============================================================================
// softgl_clipdrawtri_flat.cpp
//============================================================================
#include "stdafx.h"
#include "softgl.hpp"
#include "softgl_homoclip_flat.hpp"
#include "softgl_trirast_flat.hpp"

void SoftGL::ClipAndDrawCurrentTri_FLAT()
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

  // DETERMINE WHICH VERTEX (i) TO USE TO COLOR THE ENTIRE FACE
  if (PrimitiveBeingBuilt==TRIANGLES || PrimitiveBeingBuilt==TRIANGLE_FAN) i=2; else i=0;

  // CALCULATE LIGHTING FOR SINGLE TRIANGLE VERTEX (IGNORE ALPHA)
  // USES CURRENT VERTEX, NORMAL, LIGHTS, AND MATERIALS TO CALC THE VERTEX COLOR
  if (LightingEnabled)
  {
    CalcLighting(Verts[i].Color, Verts[i].EyeCoords, Verts[i].Normal, Materials[i], IsBackfacing);
  }

  // COPY VERTEX COLORS INTO A 1D ARRAY FOR USE BY THE CLIPPER (SCALE INTO [0,255])
  unsigned char FlatColor[] = { (unsigned char)(Verts[i].Color[0]*255), 
                                (unsigned char)(Verts[i].Color[1]*255), 
                                (unsigned char)(Verts[i].Color[2]*255) };

  // CLIP TO VIEWING FRUSTUM IN HOMOGENEOUS SPACE
  float *ClippedPolyPts;
  int NumClippedPolyPts;
  if (VFCstatus==-1)  // TRIANGLE IS COMPLETELY INSIDE FRUSTUM, NO NEED TO CLIP
  {
    ClippedPolyPts=HomoVerts;
    NumClippedPolyPts=3;  
  }
  else  // VFCstatus==0, POTENTIALLY INTERSECTS FRUSTUM BOUNDARIES, SO CLIP
  {
    NumClippedPolyPts = ClipPolyToFrustum_FLAT(HomoVerts,3,ClippedPolyPts);
  }

  PerspectiveDivide_ViewportXform(ClippedPolyPts,NumClippedPolyPts);

  // TRIANGULATE POLYGON AND RASTERIZE PIXEL SCREEN SPACE TRIANGLES
  #ifndef TURN_OFF_RASTERIZATION
    for (i=1; i<(NumClippedPolyPts-1); i++)
      RasterizeTriangle_FLAT(ColorBuffer,DepthBuffer,PixelWidth,PixelHeight, 
                             ClippedPolyPts, &(ClippedPolyPts[i*4]),
                             &(ClippedPolyPts[(i+1)*4]), FlatColor);
  #else  // JUST DRAW VERTEX POINTS
    for (i=0; i<NumClippedPolyPts*4; i+=4)
    {
      int j = (((int)(ClippedPolyPts[i+1]))*PixelWidth+((int)(ClippedPolyPts[i])))*3;
      ColorBuffer[j]  =FlatColor[0];
      ColorBuffer[j+1]=FlatColor[1];
      ColorBuffer[j+2]=FlatColor[2];
    }
  #endif

  if (VFCstatus==0)  // IF TRIANGLE WAS CLIPPED, FREE THE CLIPPER-ALLOCATED MEMORY
    delete[] ClippedPolyPts;
}
