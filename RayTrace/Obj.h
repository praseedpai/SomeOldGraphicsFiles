#ifndef _OBJ_DOT_H
#define _OBJ_DOT_H
#include "Rgb.h"
#include "Vec.h"

////////////////////////////////////
//
//
//
//
//
//
//

class Obj 
{
  public:
    virtual pRGB *getIntersection (const Vec& ray)=0;


};



#endif


