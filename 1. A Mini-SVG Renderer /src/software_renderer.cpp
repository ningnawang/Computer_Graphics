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

// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {
  // cout << "---------------call draw_svg-----------------" << endl;

  // set top level transformation
  transformation = canvas_to_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y++;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y++;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y--;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y--;

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
  // cout << "------------------In set_sample_rate-----------" << endl;

  // Task 3: 
  // You may want to modify this for supersampling support
	this->sample_rate = sample_rate;

  if (sample_rate > 1) {
    this->superTarget_w = this->target_w * sample_rate;
    this->superTarget_h = this->target_h * sample_rate;
    this->supersample_target = new unsigned char[4 * superTarget_w * superTarget_h]; 
  }

}


void SoftwareRendererImp::set_render_target( unsigned char* render_target,
                                             size_t width, size_t height ) {

  // Task 3: 
  // You may want to modify this for supersampling support
  // cout << "----------------In set_render_target----------------" << endl;

	this->render_target = render_target;
	this->target_w = width;
	this->target_h = height;

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
  rasterize_point( p.x, p.y, point.style.fillColor );

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

  // cout << "-----------------In rasterize_point---------------------------" << endl;

  int sx = (int) floor(x);
  int sy = (int) floor(y);  

  // check bounds
  if ( sx < 0 || sx >= superTarget_w ) return;
  if ( sy < 0 || sy >= superTarget_h ) return;
  // if ( sx < 0 || sx >= target_w ) return;
  // if ( sy < 0 || sy >= target_h ) return;

  // // fill sample - NOT doing alpha blending!
  // render_target[4 * (sx + sy * target_w)    ] = (uint8_t) (color.r * 255);
  // render_target[4 * (sx + sy * target_w) + 1] = (uint8_t) (color.g * 255);
  // render_target[4 * (sx + sy * target_w) + 2] = (uint8_t) (color.b * 255);
  // render_target[4 * (sx + sy * target_w) + 3] = (uint8_t) (color.a * 255);

  // // fill super sample buffer
  // supersample_target[4 * (sx + sy * superTarget_w)    ] = (uint8_t) (color.r * 255);
  // supersample_target[4 * (sx + sy * superTarget_w) + 1] = (uint8_t) (color.g * 255);
  // supersample_target[4 * (sx + sy * superTarget_w) + 2] = (uint8_t) (color.b * 255);
  // supersample_target[4 * (sx + sy * superTarget_w) + 3] = (uint8_t) (color.a * 255);

  float_to_uint8(&supersample_target[4 * (sx + sy * superTarget_w)], &color.r);
}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

  // cout << "-----------------In rasterize_line---------------------------" << endl;

  // Task 1: 
  // Implement line rasterization

  size_t sample_rate = this->sample_rate;
  float x0_super = x0 * sample_rate;
  float y0_super = y0 * sample_rate;
  float x1_super = x1 * sample_rate;
  float y1_super = y1 * sample_rate;
 

	float dx = (x1_super - x0_super);
	float dy = (y1_super - y0_super);
	float x, y, xend, yend;
	int step = (dx > 0 && dy > 0) || (dx < 0 && dy < 0) ? 1 : -1; // +1 or -1
  int isSwap = 0;

  // |k| > 1, swap x and y 
  if (fabs(dy) > fabs(dx)) {
    swap(dx, dy);
    swap(x0_super, y0_super);
    swap(x1_super, y1_super);
    isSwap = 1;
  }

	// |k| < 1
	if (dx >= 0) {
		x = floor(x0_super) + 0.5; // round to the nearest sample point
		y = floor(y0_super) + 0.5;
		xend = x1_super;
	} else {
		x = floor(x1_super) + 0.5;
		y = floor(y1_super) + 0.5;
		xend = x0_super;
	}
	float d = dx > 0 ? fabs(y0_super - y) : fabs(y1_super - y);
	float k = fabs(dy / dx);
	for ( ; x <= xend; x++) {
    if (isSwap) {
      rasterize_point(y, x, color); // x and y was swapped (|K| > 1)
    } else {
      rasterize_point(x, y, color);
    }
    d = d + k;
    if (d > 0.5) {
      d = d - 1;
      y += step;
    }
  }
} 

// return the sign of a double input
int SoftwareRendererImp::getSgn(double d) {
	if (d == 0.0) return 0.0;
	return d < 0.0? -1.0 : 1.0; 
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

						// int super_x = (int) floor(x);
						// int super_y = (int) floor(y);


					 //  supersample_target[4 * (super_x + super_y * superTarget_w)    ] = (color.r * 255);
					 //  supersample_target[4 * (super_x + super_y * superTarget_w) + 1] = (color.g * 255);
					 //  supersample_target[4 * (super_x + super_y * superTarget_w) + 2] = (color.b * 255);
					 //  supersample_target[4 * (super_x + super_y * superTarget_w) + 3] = (color.a * 255);
			}
  	}
  }

}


void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
  // Task ?: 
  // Implement image rasterization

  // box filter
  int xmin = (int) floor(min(x0, x1));
  int xmax = (int) ceil(max(x0, x1));
  int ymin = (int) floor(min(y0, y1));
  int ymax = (int) ceil(max(y0, y1));

  // Generate transfromation matrix (from cavas coordinates to texture coordinates)
  // Step1: translate (-x0, -y0)
  Matrix3x3 translateMatrix;
  translateMatrix(0,0) = 1.0;
  translateMatrix(0,1) = 0.0;
  translateMatrix(0,2) = -x0;
  translateMatrix(1,0) = 0.0;
  translateMatrix(1,1) = 1.0;
  translateMatrix(1,2) = -y0;
  translateMatrix(2,0) = 0.0;
  translateMatrix(2,1) = 0.0;
  translateMatrix(2,2) = 1.0;

  // Step2: scale (1 / abs(x1 - x0), 1 / abs(y1 - y0))
  Matrix3x3 scaleMatrix;
  scaleMatrix(0,0) = 1 / abs(x1 - x0);
  scaleMatrix(0,1) = 0.0;
  scaleMatrix(0,2) = 0.0;
  scaleMatrix(1,0) = 0.0;
  scaleMatrix(1,1) = 1 / abs(y1 - y0);
  scaleMatrix(1,2) = 0.0;
  scaleMatrix(2,0) = 0.0;
  scaleMatrix(2,1) = 0.0;
  scaleMatrix(2,2) = 1.0;

  // first translate then scale 
  // (ATTENSION: order cannot be changed, coz translate is not linear)
  Matrix3x3 transform = scaleMatrix * translateMatrix;

  for (double j = ymin + 0.5; j < ymax; j++){
    for (double i = xmin + 0.5; i < xmax; i++) {
      // transfrom to [0-1]^2 domain(texture space)
      Vector3D newCo = transform * Vector3D(i , j, 1); 
      Vector3D newCo10 = transform * Vector3D(i + 1, j ,1);
      Vector3D newCo01 = transform * Vector3D(i, j + 1,1);
      float u_scale = (newCo10 / newCo10.z - newCo / newCo.z).x; // du
      float v_scale = (newCo01 / newCo01.z - newCo / newCo.z).y; // dv
      sampler = new Sampler2DImp();
 
      // use nearest neighbor filtering method
      // Color color = sampler->sample_nearest(tex, newCo.x / newCo.z, newCo.y / newCo.z, 0);

      // use bilinear filtering method
      Color color = sampler->sample_bilinear(tex, newCo.x / newCo.z, newCo.y / newCo.z, 0);

      // Color color = sampler->sample_trilinear(tex, newCo.x / newCo.z, newCo.y / newCo.z, u_scale, v_scale);
      rasterize_point(i, j, color);
    }
  }
}



// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

  // Task 3: 
  // Implement supersampling
  // You may also need to modify other functions marked with "Task 3".
  // cout << "-------------In Resolve--------------" << endl;

  size_t sample_rate = this->sample_rate;
  size_t sample_rate_square = pow(sample_rate, 2);

  // resampling using box filter
  Color c_unweighted;
  vector<Color> box = vector<Color>(sample_rate * sample_rate);

  for (int y = 0; y < superTarget_h; y += sample_rate) {
    for (int x = 0; x < superTarget_w; x += sample_rate) {
      // fill box
      int box_x = 0, box_y = 0;
      while (box_y < sample_rate) {
        while (box_x < sample_rate) {
          uint8_to_float(&box[box_x + box_y * sample_rate].r, 
                          &supersample_target[4 * ((x + box_x) + (y + box_y) * superTarget_w)]);
          box_x++;
        }
        box_x = 0;
        box_y++;
      }

      // unweighted average
      for (int i = 0; i < box.size(); i++) {
        if (i == 0) c_unweighted = box[i];
        else {
          c_unweighted += box[i];
        }
      }
      c_unweighted *= (float) 1 / sample_rate_square; // size_t is a big integer, 
                                                      // the divide operator may make c_unweighted always be 0 

      float_to_uint8(&render_target[4 * (x / sample_rate + y / sample_rate * target_w)], 
                     &c_unweighted.r);
    }
  }

  return;
}


} // namespace CMU462
