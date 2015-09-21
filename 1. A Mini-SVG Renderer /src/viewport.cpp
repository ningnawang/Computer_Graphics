#include "viewport.h"

#include "CMU462.h"
// #include <cmath>
// #include <vector>
#include <iostream>
// #include <algorithm>


namespace CMU462 {

void ViewportImp::set_viewbox( float x, float y, float span ) {

  // Task 4 (part 2): 
  // Set svg to normalized device coordinate transformation. Your input
  // arguments are defined as SVG canvans coordinates.

  // std::cout << ", old anvas_to_norm :" << anvas_to_norm;

  double s_x = (double) x;
  double s_y = (double) y;
  double s_span = (double) span;

  // Step1: translate (-(x-span), -(y-span))
  Matrix3x3 translateMatrix;
  translateMatrix(0,0) = 1.0;
  translateMatrix(0,1) = 0.0;
  translateMatrix(0,2) = -(s_x-s_span);
  translateMatrix(1,0) = 0.0;
  translateMatrix(1,1) = 1.0;
  translateMatrix(1,2) = -(s_y-s_span);
  translateMatrix(2,0) = 0.0;
  translateMatrix(2,1) = 0.0;
  translateMatrix(2,2) = 1.0;


  // Step2: scale (1 / (2 * span), 1 / (2 * span))
  Matrix3x3 scaleMatrix;
  scaleMatrix(0,0) = (double) 1 / (2 * s_span);
  scaleMatrix(0,1) = 0.0;
  scaleMatrix(0,2) = 0.0;
  scaleMatrix(1,0) = 0.0;
  scaleMatrix(1,1) = (double) 1 / (2 * s_span);
  scaleMatrix(1,2) = 0.0;
  scaleMatrix(2,0) = 0.0;
  scaleMatrix(2,1) = 0.0;
  scaleMatrix(2,2) = 1.0;

  // first translate then scale 
  // (ATTENSION: order cannot be changed, coz translate is not linear)
  Matrix3x3 transform = scaleMatrix * translateMatrix;

  svg_2_norm = transform;

  this->x = x;
  this->y = y;
  this->span = span; 
}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) { 
  
  // std::cout << "-------------In update_viewbox------------" << std::endl;
  // std::cout << "old span" << span << std::endl;
  this->x -= dx; 
  this->y -= dy; 
  this->span *= scale; 
  set_viewbox( x, y, span );

}

} // namespace CMU462