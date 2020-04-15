#include "stdafx.h"
//============================================================================
// softgl_lighting.hpp
//============================================================================

#include <math.h>
#include "softgl.hpp"

//----------------------------------------------------------------------------
// Based on the given current vertex in eye coords (p), the current normal
// in eye coords (n), the location of the eye (or camera) in eye coords
// at the origin, the current state of the lights and lighting model, and the
// current material properties for both the front and back sides of polygons
// (as determined by the vertex normal), compute the final vertex color. The
// components of the final vertex color must be clamped to the range [0,1].
// Normal is assumed to already be normalized. Directional light direction
// is also assumed to already be normalized (w component of light position
// equals 0, L=(x,y,z) and is already normalized). We have to pass in the
// front and back materials properties corresponding to the current vertex
// (front is ColorMaterials[0] and back is ColorMaterials[1]). If 
// LightModelTwoSide=0, then only the front material is used. The IsBackfacing
// flag indicates if the triangle that this vertex belongs to is backfacing or
// frontfacing, this value is only used if LightModelTwoSide=1.
//----------------------------------------------------------------------------
void SoftGL::CalcLighting(float FinalVertexColor[4], const float p[4], const float n[3], 
                          MaterialParams ColorMaterials[2], const int IsBackfacing)
{
  // CALCULATE CORRECT NORMAL N.
  // DETERMINES WHICH MATERIAL TO USE, FRONT OR BACK. USES BACKFACE TEST TO DETERMINE THIS
  int iMaterial=0;             // FRONT, BY DEFAULT
  float N[]={n[0],n[1],n[2]};
  if (LightModelTwoSide && IsBackfacing)
  {
    iMaterial=1;  // BACK
    N[0]=-N[0]; N[1]=-N[1]; N[2]=-N[2];
  }  

  // CALCULATE NORMALIZED VIEWING DIRECTION V (COULD BE LOCAL OR INFINITE)
  float V[3];
  if (LightModelLocalViewer)  // LOCAL
  {
    V[0]=-p[0]; V[1]=-p[1]; V[2]=-p[2];  // NOT EXACTLY, COULD HAVE p[3]!=1
    float l = (float)sqrt( V[0]*V[0] + V[1]*V[1] + V[2]*V[2] );
    V[0]/=l; V[1]/=l; V[2]/=l;
  }
  else  // INFINITE (0,0,INF)
  {
    V[0]=0; V[1]=0; V[2]=1;
  }

  // EMISSIVE COMPONENT
  float *Emissive = ColorMaterials[iMaterial].Emission;

  // GLOBAL AMBIENT COMPONENT
  float GlobalAmbient[] = { LightModelAmbient[0] * ColorMaterials[iMaterial].Ambient[0],
                            LightModelAmbient[1] * ColorMaterials[iMaterial].Ambient[1],
                            LightModelAmbient[2] * ColorMaterials[iMaterial].Ambient[2],
                            LightModelAmbient[3] * ColorMaterials[iMaterial].Ambient[3] };

  // FOR EACH ACTIVE LIGHT: ADD IN DIST ATTEN, SPOTLIGHT, AMBIENT, DIFFUSE, AND SPECULAR
  float TotalFromLights[]={0,0,0,0},            // SUM OF CONTRIBUTIONS FROM ALL LIGHT SOURCES
        L[3],                                   // LIGHT DIRECTION VECTOR
        Dist,                                   // DISTANCE BETWEEN LIGHT AND VERTEX
        distance, spotlight, diffuse, specular, // ATTENUATION FACTORS
        tmp;
  for (int i=0; i<MAX_NUM_LIGHTS; i++)
    if (Lights[i].IsEnabled)
    {
      // CALCULATE NORMALIZED LIGHT DIRECTION (COULD BE POSITIONAL OR DIRECTIONAL (INFINITE))
      // FOR POSITIONAL LIGHTS, ALSO COMPUTE DISTANCE BETWEEN VERTEX AND LIGHT (UNDEFINED FOR DIRECTIONAL)
      if (Lights[i].Position[3]==0)  // DIRECTIONAL (INFINITE) IF W=0
      {
        L[0]=Lights[i].Position[0];
        L[1]=Lights[i].Position[1];
        L[2]=Lights[i].Position[2];
        distance=1;
      }
      else // POSITIONAL (FROM VERTEX POS p TO LIGHT POSITION), AGAIN, COULD HAVE p[3]!=1
      {
        L[0]=Lights[i].Position[0]-p[0];
        L[1]=Lights[i].Position[1]-p[1];
        L[2]=Lights[i].Position[2]-p[2];
        Dist=(float)sqrt( L[0]*L[0] + L[1]*L[1] + L[2]*L[2] );  // DIST BETWEEN LIGHT AND VERTEX
        L[0]/=Dist; L[1]/=Dist; L[2]/=Dist;

        // DISTANCE ATTENUATION FACTOR, ONLY APPLIES TO POSITIONAL LIGHTS
        distance = 1.0f / (Lights[i].ConstantAttenuation + 
                           Lights[i].LinearAttenuation*Dist + 
                           Lights[i].QuadraticAttenuation*Dist*Dist);
      }

      // SPOTLIGHT EFFECT
      spotlight=1;
      if (Lights[i].SpotExponent!=0 && Lights[i].SpotCutoff<=90 && Lights[i].SpotCutoff>=0)
      {
        float v[] = {-L[0],-L[1],-L[2]};    // VECTOR FROM LIGHT TO VERTEX
        float *d = Lights[i].SpotDirection; // SPOT LIGHT DIRECTION
        float VdotD = v[0]*d[0] + v[1]*d[1] + v[2]*d[2];
        float SpotCutoffRadians = Lights[i].SpotCutoff * 0.017453293f;
        if (VdotD < (float)cos(SpotCutoffRadians))
          spotlight=0;
        else
          spotlight = (float)pow(VdotD,Lights[i].SpotExponent);
      }

      // DIFFUSE COMPONENT
      diffuse = L[0]*N[0] + L[1]*N[1] + L[2]*N[2];

      // SPECULAR COMPONENT
      if (diffuse<=0)
      {
        diffuse=specular=0;
      }
      else
      {
        if (ColorMaterials[iMaterial].Shininess==0)
        {
          specular=1;
        }
        else
        {
          // CALCULATE HALFWAY VECTOR H BETWEEN VIEWER (V) AND LIGHT VECTORS (L)
          float H[] = { V[0]+L[0],V[1]+L[1],V[2]+L[2] };
          tmp = (float)sqrt( H[0]*H[0] + H[1]*H[1] + H[2]*H[2] );
          H[0]/=tmp; H[1]/=tmp; H[2]/=tmp;

          // COMPUTE SPECULAR ATTENUATION
          float HdotN = H[0]*N[0] + H[1]*N[1] + H[2]*N[2];
          if (HdotN<0) HdotN=0;
          specular = (float)pow(HdotN,ColorMaterials[iMaterial].Shininess);
        }
      }

      // ADD IN THE CONTRIBUTION FROM CURRENT LIGHT SOURCE TO TOTAL ILLUMINATION
      for (int j=0; j<4; j++)
        TotalFromLights[j] += 
          ( distance * spotlight * 
            (            Lights[i].Ambient[j]  * ColorMaterials[iMaterial].Ambient[j] +
              diffuse  * Lights[i].Diffuse[j]  * ColorMaterials[iMaterial].Diffuse[j] +
              specular * Lights[i].Specular[j] * ColorMaterials[iMaterial].Specular[j] ) );
    }

  // CREATE FINAL SUM AND CLAMP TO [0,1]
  for (i=0; i<4; i++)
  {
    FinalVertexColor[i] = Emissive[i] + GlobalAmbient[i] + TotalFromLights[i];
    if (FinalVertexColor[i]>1) FinalVertexColor[i]=1;
    else if (FinalVertexColor[i]<0) FinalVertexColor[i]=0;
  }
}
