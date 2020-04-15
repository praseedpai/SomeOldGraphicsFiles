//============================================================================
// softgl_texture_state.cpp
//============================================================================
#include "stdafx.h"
#include <stdio.h>
#include "softgl.hpp"

Texture::Texture()
{
  WrapS=WrapT=REPEAT;
  MinFilter=MagFilter=NEAREST;
  Texels=NULL;
  W=H=0;
}

Texture::~Texture()
{
  if (Texels) delete[] Texels;
}

void Texture::StoreRGB(unsigned char *Color, int Width, int Height)
{
  if (Texels) delete[] Texels;
  W=Width;
  H=Height;
  Texels = new float[W*H*3];
  for (int i=0; i<(W*H*3); i++)
    Texels[i] = (float)Color[i] / 255.0f;
}

void SoftGL::SetDefaultTextureState()
{
  Texture2dEnabled=0;
  TexEnvMode=MODULATE;
  iCurrentBoundTexture=0;      // TexID=0 IS THE DEFAULT TEXTURE (NO TEXTURE OBJECT)
  Textures = new Texture*[1];  // ALLOCATE LOCATION FOR DEFAULT TEXTURE
  Textures[0] = new Texture;
  NumTextures=1;
}

unsigned int SoftGL::GetNextTexID()  // RETURNS FIRST FREE INDEX INTO Textures, ADDS NEW ELEMENT IF FULL
{
  unsigned int i;

  // LOOP THROUGH AND RETURN INDEX OF FIRST FREE SLOT THAT IS NOT A TEXTURE
  for (i=1; i<NumTextures; i++)
    if (Textures[i]==NULL) return(i);

  // OTHERWISE, ADD A NEW TEXTURE PTR AND RETURN THAT INDEX
  unsigned int TexID = NumTextures;
  Texture* *NewTextures = new Texture*[NumTextures+1];
  for (i=0; i<NumTextures; i++)
    NewTextures[i]=Textures[i];
  delete[] Textures;
  Textures = NewTextures;
  NumTextures++;
  Textures[TexID] = new Texture;  // CREATE NEW TEXTURE TO MARK AS TAKEN
  return(TexID);
}

void SoftGL::GenTextures(int NumTexIDs, unsigned int *TexIDs) // TexIDs IS ASSUMED TO BE PREALLOCATED
{
  for (int i=0; i<NumTexIDs; i++)
    TexIDs[i] = GetNextTexID();
}

void SoftGL::DeleteTextures(int NumTexIDs, unsigned int *TexIDs)
{
  for (int i=0; i<NumTexIDs; i++)
    if (IsTexture(TexIDs[i])) 
      { delete Textures[TexIDs[i]]; Textures[TexIDs[i]]=NULL; }
}

int SoftGL::IsTexture(unsigned int TexID)
{
  if (TexID==0 || TexID>=NumTextures) return(0);  // RESERVED, DEFAULT TEXTURE OR INVALID VALUE
  return ( Textures[TexID]!=NULL );
}

void SoftGL::BindTexture(int Target, unsigned int TexID)
{
  if (Target!=TEXTURE_2D)
    { printf("SoftGL error: BindTexture: unsupported Target!\n"); return; }

  if (IsTexture(TexID) || TexID==0)  // TEXTURE OBJ ALREADY EXISTS OR DEFAULT TEXTURE (0)
    iCurrentBoundTexture=TexID;
  else
    printf("SoftGL error: BindTexture: invalid TexID (must use GenTextures)!\n");
}

void SoftGL::TexParameterf(int Target, int ParamName, int Param)
{
  if (Target!=TEXTURE_2D)
    { printf("SoftGL error: TexParameterf: unsupported Target!\n"); return; }

  switch(ParamName)
  {
    case TEXTURE_WRAP_S:
      if (Param!=REPEAT && Param!=CLAMP)
        { printf("SoftGL error: TexParameterf: unsupported wrapping mode!\n"); return; }
      Textures[iCurrentBoundTexture]->WrapS = Param;
      break;
    case TEXTURE_WRAP_T:
      if (Param!=REPEAT && Param!=CLAMP)
        { printf("SoftGL error: TexParameterf: unsupported wrapping mode!\n"); return; }
      Textures[iCurrentBoundTexture]->WrapT = Param;
      break;
    case TEXTURE_MIN_FILTER:
      if (Param!=NEAREST && Param!=LINEAR)
        { printf("SoftGL error: TexParameterf: unsupported filtering mode!\n"); return; }
      Textures[iCurrentBoundTexture]->MinFilter = Param;
      break;
    case TEXTURE_MAG_FILTER:
      if (Param!=NEAREST && Param!=LINEAR)
        { printf("SoftGL error: TexParameterf: unsupported filtering mode!\n"); return; }
      Textures[iCurrentBoundTexture]->MagFilter = Param;
      break;
    default:
      printf("SoftGL error: TexParameterf: unsupported ParamName!\n");
      break;
  };
}

void SoftGL::TexImage2D(int Target, int Level, int InternalFormat, int W, int H, 
                        int Border, int Format, int Type, const void *Texels)
{
  if (Target!=TEXTURE_2D)   { printf("SoftGL error: TexImage2D: unsupported Target!\n"); return; }
  if (Level!=0)             { printf("SoftGL error: TexImage2D: only Level of 0 is allowed!\n"); return; }
  if (InternalFormat!=_RGB) { printf("SoftGL error: TexImage2D: only InternalFormat of RGB is allowed!\n"); return; }
  if (Format!=_RGB)         { printf("SoftGL error: TexImage2D: only Format of _RGB is allowed!\n"); return; }
  if (Type!=UNSIGNED_BYTE)  { printf("SoftGL error: TexImage2D: only Type of UNSIGNED_BYTE is allowed!\n"); return; }
  unsigned char *ucTexels = (unsigned char*)Texels;
  Textures[iCurrentBoundTexture]->StoreRGB(ucTexels,W,H);
}

void SoftGL::TexEnvf(int Target, int ParamName, int Param)
{
  if (Target!=TEXTURE_ENV)
    { printf("SoftGL error: TexEnvf: unsupported Target!\n"); return; }
  
  switch (ParamName)
  {
    case TEXTURE_ENV_MODE:
      switch(Param)
      {
        case REPLACE: case MODULATE: TexEnvMode=Param; break;
        case DECAL: TexEnvMode=REPLACE; break;
        default: printf("SoftGL error: TexEnvf: unsupported Param!\n"); break;        
      };
      break;
    default: printf("SoftGL error: TexEnvf: unsupported ParamName!\n"); break;
  };
}

