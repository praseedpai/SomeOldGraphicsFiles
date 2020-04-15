#ifndef _TEXTURE_DOT_H
#define _TEXTURE_DOT_H

#include "RGB.h"

////////////////////////////////////////////
//
//
//
//
//
//
class Texture 
{
public:

  virtual pRGB *getTexel (double i, double j) = 0;

};



#endif
