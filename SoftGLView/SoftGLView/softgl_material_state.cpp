//============================================================================
// softgl_material_state.cpp
//============================================================================
#include "stdafx.h"
#include <stdio.h>
#include "softgl.hpp"

void SoftGL::SetDefaultMaterialState()
{
  MaterialParams Material_Default = { 0.2f,0.2f,0.2f,1, 0.8f,0.8f,0.8f,1, 0,0,0,1, 0, 0,0,0,1 };
  ColorMaterials[0]=Material_Default;
  ColorMaterials[1]=Material_Default;

  ColorMaterialEnabled=0;
  ColorMaterialFace=FRONT_AND_BACK;
  ColorMaterialMode=AMBIENT_AND_DIFFUSE;
}

void SoftGL::Materialf(int Face, int ParamName, float Value)
{
  if (Face==FRONT_AND_BACK)
  {
    Materialf(FRONT,ParamName,Value);
    Materialf(BACK,ParamName,Value);
  }
  else
  {
    switch(ParamName)
    {
      case SHININESS: 
        ColorMaterials[Face].Shininess=Value;
        break;
      default: printf("SoftGL error: Materialf(): unsupported color material param!\n"); break;
    };
  }
}

void SoftGL::Materialfv(int Face, int ParamName, float *Value)
{
  if (Face==FRONT_AND_BACK)
  {
    Materialfv(FRONT,ParamName,Value);
    Materialfv(BACK,ParamName,Value);
  }
  else
  {
    switch(ParamName)
    {
      case AMBIENT:
        ColorMaterials[Face].Ambient[0]=Value[0];
        ColorMaterials[Face].Ambient[1]=Value[1];
        ColorMaterials[Face].Ambient[2]=Value[2];
        ColorMaterials[Face].Ambient[3]=Value[3];
        break;
      case DIFFUSE:
        ColorMaterials[Face].Diffuse[0]=Value[0];
        ColorMaterials[Face].Diffuse[1]=Value[1];
        ColorMaterials[Face].Diffuse[2]=Value[2];
        ColorMaterials[Face].Diffuse[3]=Value[3];
        break;
      case SPECULAR:
        ColorMaterials[Face].Specular[0]=Value[0];
        ColorMaterials[Face].Specular[1]=Value[1];
        ColorMaterials[Face].Specular[2]=Value[2];
        ColorMaterials[Face].Specular[3]=Value[3];
        break;
      case EMISSION:
        ColorMaterials[Face].Emission[0]=Value[0];
        ColorMaterials[Face].Emission[1]=Value[1];
        ColorMaterials[Face].Emission[2]=Value[2];
        ColorMaterials[Face].Emission[3]=Value[3];
        break;
      default: printf("SoftGL error: Materialfv(): unsupported color material param!\n"); break;
    };
  }
}

void SoftGL::ColorMaterial(int Face, int Mode)
{
  ColorMaterialFace=Face;
  ColorMaterialMode=Mode;
}
