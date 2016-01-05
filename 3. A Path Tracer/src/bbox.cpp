#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CMU462 {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO:
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.


  // implementation of Smits’ algorithm
  double tmin, tmax, tymin, tymax, tzmin, tzmax;

  if (r.d.x >= 0.0) {
    tmin = (min.x - r.o.x) / r.d.x;
    tmax = (max.x - r.o.x) / r.d.x;
  }
  else {
    tmin = (max.x - r.o.x) / r.d.x;
    tmax = (min.x - r.o.x) / r.d.x;
  }

  if (r.d.y >= 0) {
    tymin = (min.y - r.o.y) / r.d.y;
    tymax = (max.y - r.o.y) / r.d.y;
  }
  else {
    tymin = (max.y - r.o.y) / r.d.y;
    tymax = (min.y - r.o.y) / r.d.y;
  }

  if ( (tmin > tymax) || (tymin > tmax) )
   return false;

  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;


  if (r.d.z >= 0) {
    tzmin = (min.z - r.o.z) / r.d.z;
    tzmax = (max.z - r.o.z) / r.d.z;
  }
  else {
    tzmin = (max.z - r.o.z) / r.d.z;
    tzmax = (min.z - r.o.z) / r.d.z;
  }

  if ( (tmin > tzmax) || (tzmin > tmax) )
    return false;

  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;

  if ((tmin < t1) && (tmax > t0) ) {
    t0 = tmin;
    t1 = tmax;
    return true;
  }

  return false;
}



void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CMU462
