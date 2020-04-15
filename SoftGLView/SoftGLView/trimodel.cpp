//============================================================================
// trimodel.cpp
//============================================================================
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "trimodel.hpp"
#include "vec3fv.hpp"

clock_t Time;

TriModel::TriModel()
{
  NumTris=0;
  NumTrisAlloced=1;
  Tris = new Tri[NumTrisAlloced];
}

TriModel::TriModel(char *FileName)
{
  Read(FileName);
}

TriModel::TriModel(char *FileName, unsigned char r, unsigned char g, unsigned char b)
{
  Read(FileName,r,g,b);
}

TriModel::~TriModel()
{
  delete[] Tris;
}

void TriModel::Read(char *FileName)
{
  unsigned int HexColor, RedMask=0xff0000, GreenMask=0xff00, BlueMask=0xff;

  FILE* fp = fopen(FileName,"r");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fscanf(fp, "%d", &NumTris);   // READ IN # OF TRIANGLES
  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  Tris = new Tri[NumTris];      // ALLOCATE THE TRIANGLES ARRAY
  NumTrisAlloced=NumTris;
  int iTri=0;
  for (int i=0; i<NumTris; i++) // READ IN THE TRIS
  {
    fscanf(fp, "%f %f %f", &Tris[iTri].A[0], &Tris[iTri].A[1], &Tris[iTri].A[2]);
    fscanf(fp, "%f %f %f", &Tris[iTri].B[0], &Tris[iTri].B[1], &Tris[iTri].B[2]);
    fscanf(fp, "%f %f %f", &Tris[iTri].C[0], &Tris[iTri].C[1], &Tris[iTri].C[2]);
    fscanf(fp,"%x",&HexColor);  // READ IN "HEX" COLOR

    if (CalcTriNormal(Tris[iTri].N, Tris[iTri].A,Tris[iTri].B,Tris[iTri].C)) // NOT ZERO AREA TRI
    {
      Tris[iTri].Color[0] = (unsigned char)((HexColor & RedMask) >> 16);
      Tris[iTri].Color[1] = (unsigned char)((HexColor & GreenMask) >> 8);
      Tris[iTri].Color[2] = (unsigned char)(HexColor & BlueMask);
      iTri++;
    }
    else
    {
      printf("removing zero area triangle...\n");
    }
  }
  fclose(fp);

  NumTris=iTri;
  CompressArrays();
  UpdateMinMax();   // CALC MIN AND MAX EXTENTS OF THE MODEL
}

void TriModel::Read(char *FileName, unsigned char r, unsigned char g,  unsigned char b)
{
  FILE* fp = fopen(FileName,"r");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fscanf(fp, "%d", &NumTris);   // READ IN # OF TRIANGLES
  printf("Reading in %s (%d triangles). . .\n", FileName, NumTris);
  Tris = new Tri[NumTris];      // ALLOCATE THE TRIANGLES ARRAY
  NumTrisAlloced=NumTris;
  int iTri=0;
  for (int i=0; i<NumTris; i++) // READ IN THE TRIS
  {
    fscanf(fp, "%f %f %f", &Tris[iTri].A[0], &Tris[iTri].A[1], &Tris[iTri].A[2]);
    fscanf(fp, "%f %f %f", &Tris[iTri].B[0], &Tris[iTri].B[1], &Tris[iTri].B[2]);
    fscanf(fp, "%f %f %f", &Tris[iTri].C[0], &Tris[iTri].C[1], &Tris[iTri].C[2]);

    if (CalcTriNormal(Tris[iTri].N, Tris[iTri].A,Tris[iTri].B,Tris[iTri].C)) // NOT ZERO AREA TRI
    {
      Tris[iTri].Color[0] = r;
      Tris[iTri].Color[1] = g;
      Tris[iTri].Color[2] = b;
      iTri++;
    }
    else
    {
      printf("removing zero area triangle...\n");
    }
  }
  fclose(fp);

  NumTris=iTri;
  CompressArrays();
  UpdateMinMax(); // CALC MIN AND MAX EXTENTS OF THE MODEL
}

void TriModel::Write(char *FileName)
{
  printf("Writing to %s (%d triangles). . .\n", FileName, NumTris);

  unsigned int HexColor;

  FILE* fp = fopen(FileName,"w");
  if (fp==NULL) { printf("ERROR: unable to open TriObj [%s]!\n",FileName); exit(1); }

  fprintf(fp, "%d\n", NumTris);
  for (int i=0; i<NumTris; i++)
  {
    fprintf(fp, "%f %f %f ", Tris[i].A[0], Tris[i].A[1], Tris[i].A[2]);
    fprintf(fp, "%f %f %f ", Tris[i].B[0], Tris[i].B[1], Tris[i].B[2]);
    fprintf(fp, "%f %f %f ", Tris[i].C[0], Tris[i].C[1], Tris[i].C[2]);

    HexColor = (((unsigned int)Tris[i].Color[0])<<16) +
               (((unsigned int)Tris[i].Color[1])<<8) +
               ((unsigned int)Tris[i].Color[2]);
    fprintf(fp,"%x\n",HexColor);
  }

  fclose(fp);
}

void TriModel::BeginModel()
{
  printf("creating triangle model... ");
  Time = clock();
}

void TriModel::AddTri(float A[3], float B[3], float C[3], float Color[3])
{
  float Normal[3];
  if (!CalcTriNormal(Normal,A,B,C)) return;  // DO NOT ADD ZERO AREA TRIANGLES
  if (NumTris==NumTrisAlloced)  // MUST EXPAND THE ARRAY
  {
    NumTrisAlloced*=2;
    Tri *NewTris = new Tri[NumTrisAlloced];
    for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
    delete[] Tris;
    Tris=NewTris;
  }
  Tris[NumTris].A[0] = A[0];        // PERFORM A NORMAL ADD
  Tris[NumTris].A[1] = A[1];
  Tris[NumTris].A[2] = A[2];
  Tris[NumTris].B[0] = B[0];
  Tris[NumTris].B[1] = B[1];
  Tris[NumTris].B[2] = B[2];
  Tris[NumTris].C[0] = C[0];
  Tris[NumTris].C[1] = C[1];
  Tris[NumTris].C[2] = C[2];
  Tris[NumTris].N[0] = Normal[0];
  Tris[NumTris].N[1] = Normal[1];
  Tris[NumTris].N[2] = Normal[2];
  Tris[NumTris].Color[0] = (unsigned char)(Color[0] * 255);
  Tris[NumTris].Color[1] = (unsigned char)(Color[1] * 255);
  Tris[NumTris].Color[2] = (unsigned char)(Color[2] * 255);
  NumTris++;
}

void TriModel::EndModel()
{
  CompressArrays();
  UpdateMinMax();

  printf("%d tris... %.2f seconds\n", NumTris, (clock()-Time)/(float)CLOCKS_PER_SEC);
}

void TriModel::CompressArrays()
{
  if (NumTrisAlloced==NumTris) return;
  NumTrisAlloced = NumTris;
  Tri *NewTris = new Tri[NumTrisAlloced];
  for (int i=0; i<NumTris; i++) NewTris[i]=Tris[i];
  delete[] Tris;
  Tris=NewTris;
}

void TriModel::UpdateMinMax()
{
  Copy3fv(Min,Tris[0].A);
  Copy3fv(Max,Tris[0].A);
  for (int i=0; i<NumTris; i++)
  {
    UpdateMinMax3fv(Min,Max,Tris[i].A);
    UpdateMinMax3fv(Min,Max,Tris[i].B);
    UpdateMinMax3fv(Min,Max,Tris[i].C);
  }
}

int TriModel::CalcTriNormal(float N[3], float A[3], float B[3], float C[3])
{
  float AB[3], AC[3];
  Subtract3fv(AB,B,A);
  Subtract3fv(AC,C,A);
  CrossProd3fv(N,AB,AC);
  float Length = Length3fv(N);
  if (Length==0) return(0);    // CHECK FOR ZERO AREA TRIANGLES (CROSSPROD = 2*AREA)
  ScalarDiv3fv(N,Length);
  return(1);
}

void TriModel::CalcNormals()
{
  for (int i=0; i<NumTris; i++)
    CalcTriNormal(Tris[i].N, Tris[i].A,Tris[i].B,Tris[i].C);
}

void TriModel::Normalize()  // CENTER ABOUT ORIGIN AND MAKE FIT IN [-1,1]
{
  float Trans[3], Dim[3], Scale;
  Subtract3fv(Dim,Max,Min);
  if (Dim[0]>=Dim[1] && Dim[0]>=Dim[2]) Scale=2.0f/Dim[0];
  else if (Dim[1]>=Dim[0] && Dim[1]>=Dim[2]) Scale=2.0f/Dim[1];
  else Scale=2.0f/Dim[2];
  Add3fv(Trans,Min,Max);
  ScalarMult3fv(Trans,Trans,0.5f);
  for (int i=0; i<NumTris; i++)
  {
    Subtract3fv(Tris[i].A,Tris[i].A,Trans);
    Subtract3fv(Tris[i].B,Tris[i].B,Trans);
    Subtract3fv(Tris[i].C,Tris[i].C,Trans);
    ScalarMult3fv(Tris[i].A,Tris[i].A,Scale);
    ScalarMult3fv(Tris[i].B,Tris[i].B,Scale);
    ScalarMult3fv(Tris[i].C,Tris[i].C,Scale);
  }
  UpdateMinMax();
}
