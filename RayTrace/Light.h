#ifndef _LIGHT_DOT_H
#define _LIGHT_DOT_H
#include "Loc.h"
#include "Vec.h"

class Light 
{

public :

  virtual Vec * getDirection (const Loc& position)=0;
};



class DirectionalLight: public Light 
{
private:

  Vec *dir;

public:
  /////////////////////////////////////
  //
  //
  //
  //
  DirectionalLight (double x, double y, double z) 
  {
    dir = new Vec (x, y, z);
    double length = dir->getLength ();
    dir->scale (1.0 / length);
  }

  Vec * getDirection (const Loc& loc)
  {
    return dir;
  }

};



#endif
