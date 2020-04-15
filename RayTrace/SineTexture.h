#ifndef _SINE_TEXTURE_DOT_H
#define _SINE_TEXTURE_DOT_H
#include "Texture.h"

class SineTexture: public Texture 
{

private:
  double multiplier;
  double scale;
  int modFunction;

public:
  //////////////////////////////////////////////////
  //
  //
  //
  //
  //
  //
  SineTexture (double multiplier, double scale, int modFunction) {
    this->multiplier = multiplier;
    this->scale = scale;
    this->modFunction = modFunction;
  }
  ////////////////////////////////////////////////
  //
  //
  //
  //
  //
  //
  //
  pRGB * getTexel (double i, double j) {
    i *= multiplier;
    j *= multiplier;
    double f = scale * (sin (i) + sin (j));
    return ((int) f % modFunction == 0)
      ? new pRGB (1.0, 0.0, 0.0)
      : new pRGB (0.0, 1.0, 0.0);
  }
};








#endif
