#ifndef _RGB_DOT_H
#define _RGB_DOT_H

class pRGB {

private:

  double r, g, b;

public :
  ///////////////////////////////////
  //
  //
  //
  //
  pRGB (double r, double g, double b) 
  {
    this->r = r;
    this->g = g;
    this->b = b;
  }
  /////////////////////////////////////////////
  //
  //
  //
  //
  pRGB (int rgb) {
    r = (double) (rgb >> 16 & 0xff) / 255;
    g = (double) (rgb >> 8 & 0xff) / 255;
    b = (double) (rgb >> 0 & 0xff) / 255;
  }
  //////////////////////////////////////////
  //
  //
  //
  //
  //
  void scale (double scale) {
    r *= scale;
    g *= scale;
    b *= scale;
  }

  void add (const pRGB& texel) {
    r += texel.r;
    g += texel.g;
    b += texel.b;
  }
  ////////////////////////////////////////
  //
  //
  //
  //
  //
  //
  int toRGB () {
    return 0xff000000 | (int) (r * 255.99) << 16 | (int) (g * 255.99) << 8 | (int) (b * 255.99) << 0;
  }
};







#endif
