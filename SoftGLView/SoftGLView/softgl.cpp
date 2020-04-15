//============================================================================
// softgl.cpp
//============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "softgl.hpp"

//#define TURN_OFF_RASTERIZATION

SoftGL::SoftGL(int pixelwidth, int pixelheight)
{
  PixelWidth=pixelwidth;
  PixelHeight=pixelheight;
  int NumPixels=PixelWidth*PixelHeight;
  ColorBuffer = new unsigned char[NumPixels*3];
  DepthBuffer = new float[NumPixels];
  Clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

  Modelview = new MatrixStack(MODELVIEW_MATRIXSTACK_DEPTH);
  Projection = new MatrixStack(PROJECTION_MATRIXSTACK_DEPTH);
  TextureMat = new MatrixStack(TEXTURE_MATRIXSTACK_DEPTH);
  CurrentMatrixStack = Modelview;

  CurrentColor[0]=1;
  CurrentColor[1]=1;
  CurrentColor[2]=1;

  CurrentNormal[0]=0;
  CurrentNormal[1]=0;
  CurrentNormal[2]=1;
  
  CurrentTexCoords[0]=0;
  CurrentTexCoords[1]=0;
  CurrentTexCoords[2]=0;
  CurrentTexCoords[3]=1;

  PrimitiveBeingBuilt=0;
  CurrentVert=0;
  
  Disable(CULL_FACE);
  ShadeModel(SMOOTH);
  Disable(NORMALIZE);

  UsePerspCorrectInterpolation=0;

  SetDefaultLightingState();
  SetDefaultMaterialState();
  SetDefaultTextureState();
}

SoftGL::~SoftGL()
{
  delete[] ColorBuffer;
  delete[] DepthBuffer;
  delete Modelview;
  delete Projection;
  delete TextureMat;
  for (unsigned int i=0; i<NumTextures; i++)
    if (Textures[i]) delete Textures[i];
  delete[] Textures;
}

void SoftGL::Clear(unsigned int WhichBuffers)
{
  int NumPixels=PixelWidth*PixelHeight;
  if (WhichBuffers & COLOR_BUFFER_BIT)
    memset(ColorBuffer,0,NumPixels*3);   // SET TO BLACK
  if (WhichBuffers & DEPTH_BUFFER_BIT)
    for (int i=0; i<NumPixels; i++)
      DepthBuffer[i]=1;                  // SET TO MAX DEPTH
}

void SoftGL::Begin(int WhichPrimitive)
{
  PrimitiveBeingBuilt=WhichPrimitive;
  CurrentVert=0;
}

void SoftGL::End()
{
  PrimitiveBeingBuilt=0;
  CurrentVert=0;
}

void SoftGL::SetEnableState(int i, int value)
{
  switch(i)
  {
    case LIGHT0: case LIGHT1: case LIGHT2: case LIGHT3:
    case LIGHT4: case LIGHT5: case LIGHT6: case LIGHT7: 
      Lights[i].IsEnabled=value; break;
    case LIGHTING: LightingEnabled=value; break;
    case CULL_FACE: BackfaceCullingEnabled=value; break;
    case COLOR_MATERIAL: ColorMaterialEnabled=value; break;
    case NORMALIZE: AutoNormalizationEnabled=value; break;
    case TEXTURE_2D: Texture2dEnabled=value; break;
    default: printf("SoftGL error: Enable/Disable: unsupported state!\n"); break;
  };
}

void SoftGL::TransformNormalIntoEyeCoords(float *NewN, float *N)
{
  float M[16];
  Copy16fv(M,Modelview->InverseTop());
  Transpose16fv(M);
  Mult16fv3fv(NewN,M,N);  // NewN = (Modelview^-1)^T * N
}

int SoftGL::CullBackfacingEyeSpaceTri(float *A, float *B, float *C)
{
  // EYE IS AT THE ORIGIN (0,0,0) IN EYE SPACE
  return( A[0]*(B[1]*C[2]-B[2]*C[1]) +
          A[1]*(B[2]*C[0]-B[0]*C[2]) +
          A[2]*(B[0]*C[1]-B[1]*C[0]) >= 0 );
}

// GIVEN 4D HOMOGENEOUS COORDS OF A TRIANGLE IN A 12-FLOAT PACKED ARRAY FORMAT,
// DECIDE IF THE TRIANGLE CAN BE TRIVIALLY ACCEPTED (-1: COMPLETELY IN THE FRUSTUM),
// TRIVIALLY REJECTED (1: COMPLETELY OUTSIDE ANY FRUSTUM PLANE), OR MAY NEED TO BE
// CLIPPED (0: POTENTIALLY CROSSES A FRUSTUM BOUNDARY)
int SoftGL::HomogeneousViewFrustumCull(float *HomoTriVerts) 
{
  unsigned int Code[3]={0,0,0};  // VERTEX BIT CODES WHERE 6 LEAST SIG BITS CORRESPOND l,r,b,t,n,f
  float x,y,z,w;
  int j=0;
  for (int i=0; i<3; i++)        // SET BIT MEANS OUTSIDE OF CORRESPONDING FRUSTUM PLANE
  {
    x=HomoTriVerts[j]; j++;
    y=HomoTriVerts[j]; j++;
    z=HomoTriVerts[j]; j++;
    w=HomoTriVerts[j]; j++;
    Code[i] |= ((x<-w)<<5);
    Code[i] |= ((x> w)<<4);
    Code[i] |= ((y<-w)<<3);
    Code[i] |= ((y> w)<<2);
    Code[i] |= ((z<-w)<<1);
    Code[i] |=  (z> w);
  }
  if (Code[0] & Code[1] & Code[2]) return(1);     // EACH VERTEX IS OUTSIDE OF AT LEAST ONE PLANE
  if (!(Code[0] | Code[1] | Code[2])) return(-1); // ALL VERTICES ARE INSIDE ALL PLANES
  return(0);
}

// GIVEN A PACKED ARRAY OF 4D HOMOGENEOUS POINTS,
// APPLY PERSPECTIVE DIVIDE TO RESULTING CLIPPED POLYGON (XFORM TO NDC SCREEN SPACE)
// APPLY VIEWPORT AND DEPTH NORMALIZATION TRANSFORMATION
void SoftGL::PerspectiveDivide_ViewportXform(float *Pts, int NumPts)
{
  int i,j,k,l;
  float HalfW=PixelWidth*0.5f, HalfH=PixelHeight*0.5f;
  for (i=0; i<(NumPts*4); i+=4)
  {
    j=i+1;
    k=i+2;
    l=i+3;
    Pts[i]/=Pts[l];           // PERSPECTIVE DIVIDE
    Pts[j]/=Pts[l];
    Pts[k]/=Pts[l];
    Pts[i]=(Pts[i]+1)*HalfW;  // VIEWPORT XFORM
    Pts[j]=(Pts[j]+1)*HalfH;
    Pts[k]=(Pts[k]+1)*0.5f;   // DEPTH NORMALIZATION
  }
}

void SoftGL::VertexHandler_TRIANGLES_POLYGON_TRIANGLE_FAN(float V[4])
{
  // TRANSFORM VERTEX TO EYE SPACE FOR LIGHTING
  Mult16fv4fv(Verts[CurrentVert].EyeCoords,Modelview->Top(),V);

  // STORE CURRENT PER-VERTEX COLOR OR CURRENT NORMAL AND MATERIALS DEPENDING ON IF LIGHTING IS ENABLED
  if (LightingEnabled)
  {
    Verts[CurrentVert].Normal[0]=CurrentNormal[0];
    Verts[CurrentVert].Normal[1]=CurrentNormal[1];
    Verts[CurrentVert].Normal[2]=CurrentNormal[2];
    Materials[CurrentVert][0]=ColorMaterials[0];                        // FRONT MATERIALS
    if (LightModelTwoSide) Materials[CurrentVert][1]=ColorMaterials[1]; // BACK MATERIALS
  }
  else
  {
    Verts[CurrentVert].Color[0] = CurrentColor[0];
    Verts[CurrentVert].Color[1] = CurrentColor[1];
    Verts[CurrentVert].Color[2] = CurrentColor[2];
  }    
  
  // STORE CURRENT PER-VERTEX TEX-COORDS IF TEXTURING IS ENABLED
  if (Texture2dEnabled)
  {
    Verts[CurrentVert].TexCoords[0] = CurrentTexCoords[0];
    Verts[CurrentVert].TexCoords[1] = CurrentTexCoords[1];
    Verts[CurrentVert].TexCoords[2] = CurrentTexCoords[2];
    Verts[CurrentVert].TexCoords[3] = CurrentTexCoords[3];
  }

  // DRAW THE TRIANGLES AS THEY ARE COMPLETED
  CurrentVert++;
  if (CurrentVert==3) 
  {
    if (Texture2dEnabled)
    {
      ClipAndDrawCurrentTri_SMOOTH_TEX();
    }
    else  // NO TEXTURING
    {
      if (ColorInterpolation==FLAT)
        ClipAndDrawCurrentTri_FLAT(); 
      else 
        ClipAndDrawCurrentTri_SMOOTH();
    }

    if (PrimitiveBeingBuilt==TRIANGLES)
    { 
      CurrentVert=0;
    } 
    else 
    { 
      Verts[1]=Verts[2];
      if (LightingEnabled)
      { 
        Materials[1][0]=Materials[2][0];
        if (LightModelTwoSide) Materials[1][1]=Materials[2][1];
      }
      CurrentVert=2;
    }
  }
}

void SoftGL::Vertex3f(float x, float y, float z)
{
  float V[]={x,y,z,1};
  switch(PrimitiveBeingBuilt)
  {
    case TRIANGLES:
    case POLYGON:
    case TRIANGLE_FAN: 
      VertexHandler_TRIANGLES_POLYGON_TRIANGLE_FAN(V); break;
    default: printf("SoftGL error: Begin(): currently unsupported primitive type!\n"); break;
  };
}

void SoftGL::Vertex3fv(float V[3])
{
  Vertex3f(V[0],V[1],V[2]);
}

void SoftGL::Color3f(float r, float g, float b)
{
  CurrentColor[0]=r;
  CurrentColor[1]=g;
  CurrentColor[2]=b;

  if (ColorMaterialEnabled)  // UPDATE CURRENTLY SPECIFIED COLOR MATERIAL
  {
    float C[] = { r,g,b,1 };
    Materialfv(ColorMaterialFace,ColorMaterialMode,C);
  }
}

void SoftGL::Color3ub(unsigned char r, unsigned char g, unsigned char b)
{
  Color3f(((float)r)/255.0f,((float)g)/255.0f,((float)b)/255.0f);
}

void SoftGL::Color3ubv(unsigned char C[3])
{
  Color3ub(C[0],C[1],C[2]);
}

void SoftGL::Normal3f(float x, float y, float z)
{
  CurrentNormal[0]=x;
  CurrentNormal[1]=y;
  CurrentNormal[2]=z;
  TransformNormalIntoEyeCoords(CurrentNormal,CurrentNormal);  // STORE IN EYE COORDS
  if (AutoNormalizationEnabled) // NORMALIZE IF ENABLED
  {
    float x2 = CurrentNormal[0] * CurrentNormal[0];
    float y2 = CurrentNormal[1] * CurrentNormal[1];
    float z2 = CurrentNormal[2] * CurrentNormal[2];
    float l=(float)sqrt(x2+y2+z2);
    CurrentNormal[0]/=l;
    CurrentNormal[1]/=l;
    CurrentNormal[2]/=l;
  }
}

void SoftGL::Normal3fv(float N[3])
{
  Normal3f(N[0],N[1],N[2]);
}

void SoftGL::TexCoord2f(float s, float t)
{
  TexCoord3f(s,t,0);
}

void SoftGL::TexCoord2fv(float T[2])
{
  TexCoord2f(T[0],T[1]);
}

void SoftGL::TexCoord3f(float s, float t, float r)
{
  CurrentTexCoords[0]=s;
  CurrentTexCoords[1]=t;
  CurrentTexCoords[2]=r;
  CurrentTexCoords[3]=1;
  Mult16fv4fv(CurrentTexCoords,TextureMat->Top(),CurrentTexCoords);
}

void SoftGL::TexCoord3fv(float T[3])
{
  TexCoord3f(T[0],T[1],T[2]);
}

void SoftGL::MatrixMode(int WhichMatrixMode)
{
  switch(WhichMatrixMode)
  {
    case MODELVIEW:  CurrentMatrixStack=Modelview; break;
    case PROJECTION: CurrentMatrixStack=Projection; break;
    case TEXTURE:    CurrentMatrixStack=TextureMat; break;
    default: printf("SoftGL error: MatrixMode(): unsupported matrix mode!\n"); break;
  };
}

void SoftGL::Hint(int Target, int hint)
{
  switch (Target)
  {
    case PERSPECTIVE_CORRECTION_HINT:
      if (hint==FASTEST) UsePerspCorrectInterpolation=0;
      else if (hint==NICEST) UsePerspCorrectInterpolation=1;
      else printf("SoftGL error: Hint(): unsupported hint!\n");
      break;
    default: printf("SoftGL error: Hint(): unsupported Target!\n"); break;
  };
}

void SoftGL::DrawToPPM(char *FileName)
{
  printf("Drawing to %dx%d PPM file [%s]...\n", PixelWidth,PixelHeight,FileName); 
  FILE *fp = fopen(FileName, "wb");
  fprintf(fp, "P6\n%d %d\n255\n", PixelWidth,PixelHeight);
  int NumRowPixels=PixelWidth*3;
  for (int i=(PixelHeight-1)*PixelWidth*3; i>=0; i-=(PixelWidth*3))
    fwrite(&(ColorBuffer[i]),1,NumRowPixels,fp);
  fclose(fp);
}


void SoftGL::RenderDevice(const HDC& dc )
{
    BITMAPINFO  bfh;
            LPVOID BitmapBuf = 0; 			
			memset(&bfh,0,sizeof(BITMAPINFO));

			bfh.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bfh.bmiHeader.biWidth = PixelWidth;
			bfh.bmiHeader.biHeight = PixelHeight;
			bfh.bmiHeader.biPlanes = 1;
			bfh.bmiHeader.biBitCount = 24;
			bfh.bmiHeader.biCompression =  BI_RGB;
            bfh.bmiHeader.biClrUsed =  256*256*256; 
            bfh.bmiHeader.biClrImportant = 0;  
			HBITMAP hbm = CreateDIBSection(dc,&bfh,
				DIB_RGB_COLORS , (void **)&BitmapBuf,
				0,0);

            for(int ctr = 0 ; ctr <= PixelWidth*PixelHeight*3;ctr+=3)
			{
                 unsigned char temp;

				 temp = ColorBuffer[ctr+2];
				 ColorBuffer[ctr+2] = ColorBuffer[ctr];
				 ColorBuffer[ctr]=temp;




			}


			memcpy(BitmapBuf,(void *)ColorBuffer , PixelWidth*PixelHeight*3 );
            GdiFlush();
		
			HDC hdc = CreateCompatibleDC(0);

			HBITMAP oldbitmap = 
			(HBITMAP)SelectObject(hdc,hbm);

			BitBlt(dc,0,0,abs(PixelWidth),abs(PixelHeight),hdc,0,0,SRCCOPY);
			SelectObject(hdc,oldbitmap);
			DeleteDC(hdc);
			DeleteObject(oldbitmap);


            return ;
  




}

