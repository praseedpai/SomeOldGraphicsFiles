//============================================================================
// softgl_matrixstack.cpp
//============================================================================
#include "stdafx.h"
#include <stdio.h>
#include "softgl_matrixstack.hpp"

MatrixStack::MatrixStack(int MaxStackDepth)
{
  NumMatricesAlloced = MaxStackDepth;
  Matrices = new Matrix16fv[NumMatricesAlloced];
  invMatrices = new Matrix16fv[NumMatricesAlloced];
  NumMatrices=1;
  Identity16fv(Matrices[0]);
  Identity16fv(invMatrices[0]);
}

MatrixStack::~MatrixStack()
{
  delete[] Matrices;
  delete[] invMatrices;
}

void MatrixStack::Push()
{
  NumMatrices++;
  if (NumMatrices>NumMatricesAlloced)
  { 
    NumMatrices--;
    printf("SoftGL ERROR: (Push) stack depth greater than MaxStackDepth!\n");
    return;
  }
  Copy16fv(Matrices[NumMatrices-1],Matrices[NumMatrices-2]);
  Copy16fv(invMatrices[NumMatrices-1],invMatrices[NumMatrices-2]);
}

void MatrixStack::Pop()
{
  if (NumMatrices==1)
  {
    printf("SoftGL ERROR: (Pop) stack depth less than 1!\n");
    return;
  }
  NumMatrices--;
}

float* MatrixStack::Top()
{
  return( Matrices[NumMatrices-1] );
}

float* MatrixStack::InverseTop()
{
  return( invMatrices[NumMatrices-1] );
}

void MatrixStack::PrintTop()
{
  float *M = Matrices[NumMatrices-1];
  printf("\n%f %f %f %f\n",M[0],M[4],M[8],M[12]);
  printf("%f %f %f %f\n",M[1],M[5],M[9],M[13]);
  printf("%f %f %f %f\n",M[2],M[6],M[10],M[14]);
  printf("%f %f %f %f\n\n",M[3],M[7],M[11],M[15]);
}

void MatrixStack::PrintInverseTop()
{
  float *M = invMatrices[NumMatrices-1];
  printf("\n%f %f %f %f\n",M[0],M[4],M[8],M[12]);
  printf("%f %f %f %f\n",M[1],M[5],M[9],M[13]);
  printf("%f %f %f %f\n",M[2],M[6],M[10],M[14]);
  printf("%f %f %f %f\n\n",M[3],M[7],M[11],M[15]);
}

void MatrixStack::LoadIdentity()
{
  Identity16fv(Top());
  Identity16fv(InverseTop());
}

void MatrixStack::Rotate(float DegAng, float AxisX, float AxisY, float AxisZ)
{
  float Temp[16];
  float Axis[]={AxisX,AxisY,AxisZ};
  Rotate16fv(Temp,DegAng,Axis);
  Mult16fv(Top(),Top(),Temp);
  invRotate16fv(Temp,DegAng,Axis);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::Translate(float TransX, float TransY, float TransZ)
{
  float Temp[16];
  Translate16fv(Temp,TransX,TransY,TransZ);
  Mult16fv(Top(),Top(),Temp);
  invTranslate16fv(Temp,TransX,TransY,TransZ);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::Scale(float ScaleX, float ScaleY, float ScaleZ)
{
  float Temp[16];
  Scale16fv(Temp,ScaleX,ScaleY,ScaleZ);
  Mult16fv(Top(),Top(),Temp);
  invScale16fv(Temp,ScaleX,ScaleY,ScaleZ);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::LookAt(float eyex, float eyey, float eyez,
                         float lookatptx, float lookatpty, float lookatptz,
                         float upx, float upy, float upz)
{
  float Temp[16];
  float Eye[]={eyex,eyey,eyez};
  float LookAtPt[]={lookatptx,lookatpty,lookatptz};
  float Up[]={upx,upy,upz};
  LookAt16fv(Temp,Eye,LookAtPt,Up);
  Mult16fv(Top(),Top(),Temp);
  invLookAt16fv(Temp,Eye,LookAtPt,Up);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::Frustum(float l, float r, float b, float t, float n, float f)
{
  float Temp[16];
  Frustum16fv(Temp,l,r,b,t,n,f);
  Mult16fv(Top(),Top(),Temp);
  invFrustum16fv(Temp,l,r,b,t,n,f);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
{
  float Temp[16];
  Perspective16fv(Temp,Yfov,Aspect,Ndist,Fdist);
  Mult16fv(Top(),Top(),Temp);
  invPerspective16fv(Temp,Yfov,Aspect,Ndist,Fdist);
  Mult16fv(InverseTop(),Temp,InverseTop());
}

void MatrixStack::Ortho(float l, float r, float b, float t, float n, float f)
{
  float Temp[16];
  Ortho16fv(Temp,l,r,b,t,n,f);
  Mult16fv(Top(),Top(),Temp);
  invOrtho16fv(Temp,l,r,b,t,n,f);
  Mult16fv(InverseTop(),Temp,InverseTop());
}
