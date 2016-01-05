#include "triangle.h"

#include "CMU462/CMU462.h"
#include "GL/glew.h"

namespace CMU462 { namespace StaticScene {

Triangle::Triangle(const Mesh* mesh, size_t v1, size_t v2, size_t v3) :
    mesh(mesh), v1(v1), v2(v2), v3(v3) { }

BBox Triangle::get_bbox() const {
  
  // TODO: 
  // compute the bounding box of the triangle
  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];

  double x_max = max(max(p0.x, p1.x), p2.x);
  double y_max = max(max(p0.y, p1.y), p2.y);
  double z_max = max(max(p0.z, p1.z), p2.z);
  
  double x_min = min(min(p0.x, p1.x), p2.x);
  double y_min = min(min(p0.y, p1.y), p2.y);
  double z_min = min(min(p0.z, p1.z), p2.z);

  Vector3D max = Vector3D(x_max, y_max, z_max);
  Vector3D min = Vector3D(x_min, y_min, z_min);

  return BBox(min, max);
}


bool Triangle::intersect(const Ray& r) const {

  // TODO: implement ray-triangle intersection
  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;
  Vector3D s = r.o - p0;
  Vector3D d = r.d;

  double frag = dot(cross(e1, d), e2);
  if (frag < 0.000001) {
    return false;
  } 

  double u = -dot(cross(s, e2), d) / frag;
  if (u < 0.0 || u > 1.0)
      return false;

  double v = dot(cross(e1, d), s) / frag;
  if (v < 0.0 || u + v > 1.0)
      return false;

  double t = -dot(cross(s, e2), e1) / frag;

  // this means that there is a line intersection
  // but not a ray intersection    
  if (t > r.max_t || t < r.min_t) 
      return false;

  return true;
  
}

bool Triangle::intersect(const Ray& r, Intersection *isect) const {
  
  // TODO: 
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly

  
  Vector3D p0 = mesh->positions[v1];
  Vector3D p1 = mesh->positions[v2];
  Vector3D p2 = mesh->positions[v3];
  Vector3D e1 = p1 - p0;
  Vector3D e2 = p2 - p0;
  Vector3D s = r.o - p0;
  Vector3D d = r.d;


  double frag = dot(cross(e1, d), e2);
  if (fabs(frag) < 0.000001) {
    return false;
  } 

  double u = -dot(cross(s, e2), d) / frag;

  // cout << "u: " << u << endl;

  if (u < 0.0 || u > 1.0)
      return false;


  double v = dot(cross(e1, d), s) / frag;
  if (v < 0.0 || u + v > 1.0)
      return false;


  double t = -dot(cross(s, e2), e1) / frag;

  // this means that there is a line intersection
  // but not a ray intersection  
  if (t > r.max_t || t < r.min_t) 
    return false;

  // compute normal based on barycentric coordinate
  Vector3D n = mesh->normals[v1] + u * (mesh->normals[v2] - mesh->normals[v1]) + v * (mesh->normals[v3] - mesh->normals[v1]);
  n.normalize();
  // we hit the back face
  if (dot(n, r.d) > 0) 
    n *= -1.0f;
  //     return false;

  // update intersection 
  isect->t = t;
  r.max_t = t;
  isect->n = n;
  isect->primitive = this;
  isect->bsdf = mesh->get_bsdf();

  return true;

  
}

void Triangle::draw(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_TRIANGLES);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}

void Triangle::drawOutline(const Color& c) const {
  glColor4f(c.r, c.g, c.b, c.a);
  glBegin(GL_LINE_LOOP);
  glVertex3d(mesh->positions[v1].x,
             mesh->positions[v1].y,
             mesh->positions[v1].z);
  glVertex3d(mesh->positions[v2].x,
             mesh->positions[v2].y,
             mesh->positions[v2].z);
  glVertex3d(mesh->positions[v3].x,
             mesh->positions[v3].y,
             mesh->positions[v3].z);
  glEnd();
}



} // namespace StaticScene
} // namespace CMU462
