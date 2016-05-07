#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// helper method
inline void uint8_to_float( float dst[4], unsigned char* src ) {
  uint8_t* src_uint8 = (uint8_t *)src;
  dst[0] = src_uint8[0] / 255.f;
  dst[1] = src_uint8[1] / 255.f;
  dst[2] = src_uint8[2] / 255.f;
  dst[3] = src_uint8[3] / 255.f;
}

inline void float_to_uint8( unsigned char* dst, float src[4] ) {
  uint8_t* dst_uint8 = (uint8_t *)dst;
  dst_uint8[0] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[0])));
  dst_uint8[1] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[1])));
  dst_uint8[2] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[2])));
  dst_uint8[3] = (uint8_t) ( 255.f * max( 0.0f, min( 1.0f, src[3])));
}

// helper method for Edge Equation
// return the sign of a double input
inline int getSgn(double d) {
  if (d == 0.0) return 0.0;
  return d < 0.0? -1.0 : 1.0; 
}

inline unsigned char clamp(float c) {
  return (unsigned char)(255.f * max(0.f, min(c, 1.f)));
}

inline double fpart(double d) {
  return d < 0 ? 1 - (d - floor(d)) : d - floor(d);
}

inline double rfpart(double d) { return 1 - fpart(d); }


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = canvas_to_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();
  clear_supersample_target();
}

// 

void SoftwareRendererImp::set_sample_rate( size_t sample_rate ) {

   // Task 3: 
   // You may want to modify this for supersampling support
   this->sample_rate = sample_rate;
   
   // allocate new buffer once sample_rate change
   this->superTarget_w = this->target_w * sample_rate;
   this->superTarget_h = this->target_h * sample_rate;
   this->supersample_target = new unsigned char[4 * superTarget_w * superTarget_h]; 
}


void SoftwareRendererImp::set_render_target( unsigned char* render_target,
					       size_t width, size_t height ) {

  // Task 3: 
  // You may want to modify this for supersampling support
  this->render_target = render_target;
  this->target_w = width;
  this->target_h = height;

  // allocate supersampling buffer
  this->superTarget_w = width * sample_rate;
  this->superTarget_h = height * sample_rate;
  this->supersample_target = new unsigned char[4 * superTarget_w * superTarget_h]; 

 }

void SoftwareRendererImp::draw_element( SVGElement* element ) {

  // Task 4 (part 1):
  // Modify this to implement the transformation stack

  // update transformation matrix by transfrom matrix
  Matrix3x3 transMatrix = element->transform;
  transformation =  transformation * transMatrix;

  switch(element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
  }

  // take transform matrix out of transformation matrix
  transformation =  transformation * transMatrix.inv();
}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point(p.x, p.y, point.style.fillColor);
}

void SoftwareRendererImp::draw_line( Line& line ) { 
  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );
}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(   x   ,   y   ));
  Vector2D p1 = transform(Vector2D( x + w ,   y   ));
  Vector2D p2 = transform(Vector2D(   x   , y + h ));
  Vector2D p3 = transform(Vector2D( x + w , y + h ));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }
}


void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates



void SoftwareRendererImp::rasterize_point( float x, float y, Color color ) {
  
  // Attension: this function will write color to supersample_target directly (not render_target),
  //            please make sure x, y has been scaled by sample_rate before calling this function

  int sx = (int) floor(x);
  int sy = (int) floor(y);  

  // check bounds
  if ( sx < 0 || sx >= superTarget_w ) return;
  if ( sy < 0 || sy >= superTarget_h ) return;
  
  // Task 7: Simple Alpha Compositing
  Color curr_color;
  Color new_color;
  uint8_to_float(&curr_color.r, &supersample_target[4 * (sx + sy * superTarget_w)]);
  
  // premultiplied alpha
  curr_color = Color(curr_color.a * curr_color.r, curr_color.a * curr_color.g, curr_color.a * curr_color.b, curr_color.a);
  new_color  = Color(color.a * color.r, color.a * color.g, color.a * color.b, color.a);
  
  Color c = new_color + (1 - color.a) * curr_color;
  float_to_uint8(&supersample_target[4 * (sx + sy * superTarget_w)], &c.r);
}
  
  
void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // Task 1: 
  // Implement line rasterization


  size_t sample_rate = this->sample_rate;
  float x0_super = x0 * sample_rate;
  float y0_super = y0 * sample_rate;
  float x1_super = x1 * sample_rate;
  float y1_super = y1 * sample_rate;

  float dx = x1_super - x0_super;
  float dy = y1_super - y0_super;


  /* Bresenham's Algorithm */

  // float x, y, xend, yend;
  // int step = (dx > 0 && dy > 0) || (dx < 0 && dy < 0) ? 1 : -1; // +1 or -1
  // int isSwap = 0;

  // // |k| > 1, swap x and y 
  // if (fabs(dy) > fabs(dx)) {
  //   swap(dx, dy);
  //   swap(x0_super, y0_super);
  //   swap(x1_super, y1_super);
  //   isSwap = 1;
  // }

  // // |k| < 1
  // if (dx >= 0) {
  //   x = floor(x0_super) + 0.5; // round to the nearest sample point
  //   y = floor(y0_super) + 0.5;
  //   xend = x1_super;
  // } else {
  //   x = floor(x1_super) + 0.5;
  //   y = floor(y1_super) + 0.5;
  //   xend = x0_super;
  // }
  // float d = dx > 0 ? fabs(y0_super - y) : fabs(y1_super - y);
  // float k = fabs(dy / dx);
  // for ( ; x <= xend; x++) {
  //   if (isSwap) {
  //     rasterize_point(y, x, color); // x and y was swapped (|K| > 1)
  //   } else {
  //     rasterize_point(x, y, color);
  //   }
  //   d = d + k;
  //   if (d > 0.5) {
  //     d = d - 1;
  //     y += step;
  //   }
  // }


  /* Xiaolin Wu's Algorithm */

  bool isSwap = fabs(dy) > fabs(dx);

  // |k| > 1, swap x and y 
  if (fabs(dy) > fabs(dx)) {
    swap(dx, dy);
    swap(x0_super, y0_super);
    swap(x1_super, y1_super);
  }

  if (x0_super > x1_super) {
    swap(x0_super, x1_super);
    swap(y0_super, y1_super);
  }

  // compute gradient
  float gradient = dy / dx;

  // align gradient to center of pixel
  x0_super -= 0.5;
  y0_super -= 0.5;
  x1_super -= 0.5;
  y1_super -= 0.5; 

  // handle first endpoint
  int xend = round(x0_super);
  float yend = y0_super + gradient * (xend - x0_super);
  float xgap = rfpart(x0_super + 0.5);
  int xpxl1 = xend; // this will be used in the main loop
  int ypxl1 = int(yend);
  if (isSwap) {
    rasterize_point(ypxl1, xpxl1, rfpart(yend) * xgap * color);
    rasterize_point(ypxl1 + 1, xpxl1, fpart(yend) * xgap * color);
  } else {
    rasterize_point(xpxl1, ypxl1, rfpart(yend) * xgap * color);
    rasterize_point(xpxl1, ypxl1 + 1, fpart(yend) * xgap * color);
  }
  float intery = yend + gradient; // first y-intersection for the main loop

  // handle second endpoint
  xend = round(x1_super);
  yend = y1_super + gradient * (xend - x1_super);
  xgap = fpart(x1_super + 0.5);
  int xpxl2 = xend; // this will be used in the main loop
  int ypxl2 = int(yend);
  if (isSwap) {
    rasterize_point(ypxl2, xpxl2, rfpart(yend) * xgap * color);
    rasterize_point(ypxl2 + 1, xpxl2, fpart(yend) * xgap * color);
  } else {
    rasterize_point(xpxl2, ypxl2, rfpart(yend) * xgap * color);
    rasterize_point(xpxl2, ypxl2 + 1, fpart(yend) * xgap * color);
  }

  // main loop
  for (float x = (xpxl1 + 1); x <= (xpxl2 - 1); x++) {
    if (isSwap) {
      rasterize_point(int(intery), x, rfpart(intery) * color);
      rasterize_point(int(intery) + 1, x, fpart(intery) * color);
    } else {
      rasterize_point(x, int(intery), rfpart(intery) * color);
      rasterize_point(x, int(intery) + 1, fpart(intery) * color);
    }
    intery += gradient;
  }

} 


void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
  // Task 2: 
  // Implement triangle rasterization

  // supersampling
  size_t sample_rate = this->sample_rate;
  float x0_super = x0 * sample_rate;
  float y0_super = y0 * sample_rate;
  float x1_super = x1 * sample_rate;
  float y1_super = y1 * sample_rate;
  float x2_super = x2 * sample_rate;
  float y2_super = y2 * sample_rate;

  // create sample points
  int xmin = (int) floor(min(min(x0_super, x1_super), x2_super));
  int xmax = (int) ceil(max(max(x0_super, x1_super), x2_super));
  int ymin = (int) floor(min(min(y0_super, y1_super), y2_super));
  int ymax = (int) ceil(max(max(y0_super, y1_super), y2_super));

  // generate edge equations
  // edge from p0 to p1
  double A01 = (y1_super - y0_super);
  double B01 = (x0_super - x1_super);
  double C01 = (x1_super * y0_super - x0_super * y1_super);

  // edge from p1 to p2
  double A12 = (y2_super - y1_super);
  double B12 = (x1_super - x2_super);
  double C12 = (x2_super * y1_super - x1_super * y2_super);

  // edge from p2 to p0
  double A20 = (y0_super - y2_super);
  double B20 = (x2_super - x0_super);
  double C20 = (x0_super * y2_super - x2_super * y0_super);

  // check if the sample point is in the triangle
  for (double y = ymin + 0.5; y < ymax; y += 1) {
    for (double x = xmin + 0.5; x < xmax; x += 1) {
      // if the sample point is in the triagle, then all edge equations 
      // should be either positive or negative 
      if (getSgn(A01 * x + B01 * y + C01) == getSgn(A12 * x + B12 * y + C12) &&
	         getSgn(A12 * x + B12 * y + C12) == getSgn(A20 * x + B20 * y + C20)) {
	           rasterize_point(x, y, color);
      }
    }
  }

}


void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task ?: 
  // Implement image rasterization
    
  size_t sample_rate = this->sample_rate;
  float x0_super = x0 * sample_rate;
  float y0_super = y0 * sample_rate;
  float x1_super = x1 * sample_rate;
  float y1_super = y1 * sample_rate;
  
  // box filter
  int xmin = (int) floor(min(x0_super, x1_super));
  int xmax = (int) ceil(max(x0_super, x1_super));
  int ymin = (int) floor(min(y0_super, y1_super));
  int ymax = (int) ceil(max(y0_super, y1_super));

  // Generate transfromation matrix (from cavas coordinates to texture coordinates)
  // Step1: translate (-x0, -y0)
  Matrix3x3 translateMatrix;
  translateMatrix(0,0) = 1.0;
  translateMatrix(0,1) = 0.0;
  translateMatrix(0,2) = -x0_super;
  translateMatrix(1,0) = 0.0;
  translateMatrix(1,1) = 1.0;
  translateMatrix(1,2) = -y0_super;
  translateMatrix(2,0) = 0.0;
  translateMatrix(2,1) = 0.0;
  translateMatrix(2,2) = 1.0;

  // Step2: scale (1 / abs(x1 - x0), 1 / abs(y1 - y0))
  Matrix3x3 scaleMatrix;
  scaleMatrix(0,0) = 1 / abs(x1_super - x0_super);
  scaleMatrix(0,1) = 0.0;
  scaleMatrix(0,2) = 0.0;
  scaleMatrix(1,0) = 0.0;
  scaleMatrix(1,1) = 1 / abs(y1_super - y0_super);
  scaleMatrix(1,2) = 0.0;
  scaleMatrix(2,0) = 0.0;
  scaleMatrix(2,1) = 0.0;
  scaleMatrix(2,2) = 1.0;

  // first translate then scale 
  // (ATTENSION: order cannot be changed, coz translate is not linear)
  Matrix3x3 transform = scaleMatrix * translateMatrix;
  sampler = new Sampler2DImp();

  for (double j = ymin + 0.5; j < ymax; j++){
    for (double i = xmin + 0.5; i < xmax; i++) {
      // transfrom to [0-1]^2 domain(texture space)
      Vector3D newCo = transform * Vector3D(i , j, 1); 
      Vector3D newCo10 = transform * Vector3D(i + 1, j ,1);
      Vector3D newCo01 = transform * Vector3D(i, j + 1,1);
      float u_scale = (newCo10 / newCo10.z - newCo / newCo.z).x; // du
      float v_scale = (newCo01 / newCo01.z - newCo / newCo.z).y; // dv
 
      // use nearest neighbor filtering method
      //       Color color = sampler->sample_nearest(tex, newCo.x / newCo.z, newCo.y / newCo.z, 0);

      // use bilinear filtering method
      //       Color color = sampler->sample_bilinear(tex, newCo.x / newCo.z, newCo.y / newCo.z, 0);

      // use trilinear filtering method
      Color color = sampler->sample_trilinear(tex, newCo.x / newCo.z, newCo.y / newCo.z, u_scale, v_scale);
      
      rasterize_point(i, j, color);
    }
  }
}



// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

    // Task 3: 
    // Implement supersampling
    // You may also need to modify other functions marked with "Task 3".

    size_t sample_rate = this->sample_rate;
    size_t sample_rate_square = pow(sample_rate, 2);

    float avg;
    for (int x = 0; x < target_w; x++) {
      for (int y = 0; y < target_h; y++) {

        // calculate color attributes: r g b a
        for (int i = 0; i < 4; i++) {
          avg = 0;
          size_t sx = x * sample_rate;
          size_t sy = y * sample_rate;

          // fill box
          for (int m = 0; m < sample_rate; m++) {
            for (int n = 0; n < sample_rate; n++) {
              avg += supersample_target[4 * ((sx + m) + (sy + n) * superTarget_w) + i];
            }
          }

          avg *= 1.0f / sample_rate_square;
          render_target[4 * (x + y * target_w) + i] = (uint8_t)avg;
        }
      }
    }
    
    // supersample_target will be cleared in draw_svg()
    return;
  }


} // namespace CMU462
