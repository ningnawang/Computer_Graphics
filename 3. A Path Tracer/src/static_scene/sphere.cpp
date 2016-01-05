#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CMU462 { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO:
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.

  // at^2 + bt + c = 0
  float a = dot(r.d, r.d);
  float b = dot(2.0f * r.d, (r.o - this->o));
  float c = dot(this->o, this->o) + dot(r.o, r.o) - 2.0f * dot(r.o, this->o) - r2;
  float D = b*b + (-4.0f)*a*c;

  // ray can not intersect
  if (D < 0)
    return false;

  D=sqrtf(D);
  // Ray can intersect the sphere, update intersection
  // float t = (-0.5f) * (b + D) / a;
  t1 = (0.5f) * (-b - D) / a; // t1 is smaller than t2
  t2 = (0.5f) * (-b + D) / a;

  return true;

}

bool Sphere::intersect(const Ray& r) const {

  // TODO:
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.

  double t1, t2;
  if (test(r, t1, t2)) { // has intersection
    return ((t1 >= r.min_t && t1 <= r.max_t) || 
            (t2 >= r.min_t && t2 <= r.max_t));
  }
  return false;

}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO:
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.

  double t1, t2;
  if (test(r, t1, t2)) { // has intersection

    float t = t1;

    // not only consider one t
    // if the ray is inside of the spere, then the t1 is negative and behind the ray origin, 
    // we should use t2
    if (t < r.min_t || t > r.max_t) t = t2;
    if (t < r.min_t || t > r.max_t) return false;
      
    // distance = sqrtf(a)*t;
    Vector3D p = r.o + t * r.d;

    // compute normal based on barycentric coordinate
    Vector3D n = (p - this->o) / this->r;

    // we shouldn't ignore the back side, coz ray may goes into the sphere(glass)
    // we hit the back face
    // if (dot(n, r.d) > 0) {
    //   return false;
    // }
    r.max_t = t;
    i->t = t;
    i->primitive = this;
    i->n = n;
    i->bsdf = object->get_bsdf();
    return true;
  }

  return false;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CMU462
