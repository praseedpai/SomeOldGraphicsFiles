//============================================================================
// trimodel.hpp
//============================================================================

#ifndef _TRIMODEL_
#define _TRIMODEL_

struct Tri
{
  float A[3],B[3],C[3], N[3]; // THREE VERTICES (CCW, RIGHT-HAND RULE), NORMAL
  unsigned char Color[3];     // RED, GREEN, BLUE
};

class TriModel
{
 public:

  Tri *Tris;
  int NumTris, NumTrisAlloced;

  float Min[3], Max[3];

  TriModel();
  TriModel(char *FileName);
  TriModel(char *FileName, unsigned char r, unsigned char g, unsigned char b);
  ~TriModel();

  void Read(char *FileName, unsigned char r, unsigned char g, unsigned char b);
  void Read(char *FileName);
  void Write(char *FileName);

  void BeginModel();
  void AddTri(float A[3], float B[3], float C[3], float Color[3]);
  void EndModel();

  void CompressArrays();
  int CalcTriNormal(float N[3], float A[3], float B[3], float C[3]);
  void CalcNormals();

  void UpdateMinMax();
  void Normalize();
};

#endif
