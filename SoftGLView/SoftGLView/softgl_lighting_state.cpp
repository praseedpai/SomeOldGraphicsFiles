#include "stdafx.h"
//============================================================================
// softgl_lighting_state.cpp
//============================================================================

#include <stdio.h>
#include <math.h>
#include "softgl.hpp"

void SoftGL::SetDefaultLightingState()
{
  LightingEnabled=0;
  LightParams Light_0_Default   = { 0,0,0,1, 1,1,1,1, 1,1,1,1, 0,0,1,0, 0,0,-1, 0, 180, 1,0,0, 0 };
  LightParams Light_1_7_Default = { 0,0,0,1, 0,0,0,1, 0,0,0,1, 0,0,1,0, 0,0,-1, 0, 180, 1,0,0, 0 };
  Lights[0]=Light_0_Default;
  for (int i=1; i<8; i++)
    Lights[i]=Light_1_7_Default;

  LightModelAmbient[0]=0.2f;
  LightModelAmbient[1]=0.2f;
  LightModelAmbient[2]=0.2f;
  LightModelAmbient[3]=1.0f;
  LightModelLocalViewer=0;
  LightModelTwoSide=0;
}

void SoftGL::Lightf(int LightID, int ParamName, float Value)
{
  if (LightID>=MAX_NUM_LIGHTS)
  { printf("SoftGL error: Lightf(): LightID>=MAX_NUM_LIGHTS!\n"); return; }

  switch(ParamName)
  {
    case SPOT_EXPONENT:         Lights[LightID].SpotExponent=Value;         break;
    case SPOT_CUTOFF:           Lights[LightID].SpotCutoff=Value;           break;
    case CONSTANT_ATTENUATION:  Lights[LightID].ConstantAttenuation=Value;  break;
    case LINEAR_ATTENUATION:    Lights[LightID].LinearAttenuation=Value;    break;
    case QUADRATIC_ATTENUATION: Lights[LightID].QuadraticAttenuation=Value; break;
    default: printf("SoftGL error: Lightf(): unsupported light param!\n");  break;
  };
}

void SoftGL::Lightfv(int LightID, int ParamName, float *Value)
{
  if (LightID>=MAX_NUM_LIGHTS)
  { printf("SoftGL error: Lightf(): LightID>=MAX_NUM_LIGHTS!\n"); return; }

  float l;
  switch(ParamName)
  {
    case AMBIENT:
      Lights[LightID].Ambient[0]=Value[0];
      Lights[LightID].Ambient[1]=Value[1];
      Lights[LightID].Ambient[2]=Value[2];
      Lights[LightID].Ambient[3]=Value[3];
      break;
    case DIFFUSE:
      Lights[LightID].Diffuse[0]=Value[0];
      Lights[LightID].Diffuse[1]=Value[1];
      Lights[LightID].Diffuse[2]=Value[2];
      Lights[LightID].Diffuse[3]=Value[3];
      break;
    case SPECULAR:
      Lights[LightID].Specular[0]=Value[0];
      Lights[LightID].Specular[1]=Value[1];
      Lights[LightID].Specular[2]=Value[2];
      Lights[LightID].Specular[3]=Value[3];
      break;
    case POSITION:  // STORE IN EYE COORDS
      Mult16fv4fv(Lights[LightID].Position,Modelview->Top(),Value);
      if (Lights[LightID].Position[3]==0) // NORMALIZE IF DIRECTIONAL
      {
        l=(float)sqrt( Lights[LightID].Position[0] * Lights[LightID].Position[0] + 
                       Lights[LightID].Position[1] * Lights[LightID].Position[1] + 
                       Lights[LightID].Position[2] * Lights[LightID].Position[2] );
        Lights[LightID].Position[0]/=l;
        Lights[LightID].Position[1]/=l;
        Lights[LightID].Position[2]/=l;
      }
      break;
    case SPOT_DIRECTION:  // STORE IN EYE COORDS, TRANSFORM SAME AS NORMAL VECTOR
      TransformNormalIntoEyeCoords(Lights[LightID].SpotDirection,Value);
      l=(float)sqrt( Lights[LightID].SpotDirection[0] * Lights[LightID].SpotDirection[0] +
                     Lights[LightID].SpotDirection[1] * Lights[LightID].SpotDirection[1] +
                     Lights[LightID].SpotDirection[2] * Lights[LightID].SpotDirection[2] );
      Lights[LightID].SpotDirection[0]/=l;
      Lights[LightID].SpotDirection[1]/=l;
      Lights[LightID].SpotDirection[2]/=l;
      break;
    default: printf("SoftGL error: Lightfv(): unsupported light param!\n"); break;
  };
}

void SoftGL::LightModelfv(int ParamName, float *Value)
{
  switch(ParamName)
  {
    case LIGHT_MODEL_AMBIENT:
      LightModelAmbient[0]=Value[0];
      LightModelAmbient[1]=Value[1];
      LightModelAmbient[2]=Value[2];
      LightModelAmbient[3]=Value[3];
      break;
    default: printf("SoftGL error: LightModelfv(): unsupported light model param!\n"); break;
  };
}

void SoftGL::LightModeli(int ParamName, int Value)
{
  switch(ParamName)
  {
    case LIGHT_MODEL_LOCAL_VIEWER: LightModelLocalViewer=Value; break;
    case LIGHT_MODEL_TWO_SIDE:     LightModelTwoSide=Value;     break;
    default: printf("SoftGL error: LightModeli(): unsupported light model param!\n"); break;
  };
}
