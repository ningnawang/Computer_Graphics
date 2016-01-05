#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CMU462 {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return albedo * (1.0 / PI);
}

/**
  * wo and wi should both be defined in the local coordinate
  * system at the point of intersection.
**/
Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // cosine-weighted hemisphere sampler
  *wi = sampler.get_sample(pdf);
  (*wi).normalize();
  
  // UniformHemisphereSampler3D * uniformHemisphereSampler = new UniformHemisphereSampler3D();
  // *wi = uniformHemisphereSampler->get_sample();
  // *pdf = 1.f / (2 * PI);

  return f(wo, *wi);
}


// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // f() will be called by direct lighting
  // the possibility that the dirct light wi and wo are perfect mirror is 0
  // so we just return 0 for direct light
  // or we can also have a if else statement here to decide if wi and wo are perfect mirror
  return Spectrum(0, 0, 0);
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement MirrorBSDF

  // sample_f() function will be called by indirect lighting,
  // since we only consider the direction wi that is perfect mirror of wo,
  // the pdf should be 1 and brdf should be reflectance for this case
  reflect(wo, wi);
  *pdf = 1.f;
  
  // since when doing direct/indirect lighting in trace_ray() function,
  // we use f * L * cos_theta
  // but we need to keep f * cos_theta constant, 
  // so we do (f / cos_theta) * cos_theta = f
  // return reflectance * (1.f / max(0.0, (*wi)[2]));

  // Attension: no need to divide by cos_theta
  // has already handled this case in pathtracer.pp trace_ray() function.
  return reflectance; 
}

// Glossy BSDF //

/*
Spectrum GlossyBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlossyBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0f;
  return reflect(wo, wi, reflectance);
}
*/

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement RefractionBSDF

  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Compute Fresnel coefficient and either reflect or refract based on it.
  Vector3D n = Vector3D(0, 0, 1);
  double ior_i;
  double ior_t;
  double cos_theta_o;
  double cos_theta_i;

  // refract(wo, wi, ior);
  // total internal refraction
  if(!refract(wo, wi, ior)) { 
      *pdf = 1.0;
      return reflectance;
      // return Spectrum();
  }

  // the fraction of reflected light 
  double Fr = 0.0;

  // the ray is currently in vacuum
  if (dot(wo, n) > 0) {
    ior_i = 1.f;
    ior_t = ior;
    cos_theta_o = dot(wo, n);
    cos_theta_i = dot(*wi, -n);
  }
  else { // the ray is leaving the surface and entering vacuum
    ior_i = ior;
    ior_t = 1.f;
    cos_theta_o = dot(wo, -n);
    cos_theta_i = dot(*wi, n);
  }

  double r_paral = (ior_t * cos_theta_o - ior_i * cos_theta_i) / (ior_t * cos_theta_o + ior_i * cos_theta_i);
  double r_perpen = (ior_i * cos_theta_o - ior_t * cos_theta_i) / (ior_i * cos_theta_o + ior_t * cos_theta_i);

  Fr = (r_paral * r_paral + r_perpen * r_perpen) / 2.f;

  double RAN = (double)(std::rand()) / RAND_MAX;
  if (RAN > Fr) { // refraction 
    *pdf = 1;
    // Attension: no need to divide by cos_theta
    // has already handled this case in pathtracer.pp trace_ray() function.
    return transmittance * (pow(ior_t, 2) / pow(ior_i, 2));  
  }
  else { // reflection
    reflect(wo, wi);
    *pdf = 1;
    // Attension: no need to divide by cos_theta
    // has already handled this case in pathtracer.pp trace_ray() function.
    return reflectance;
  }

}

void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {

  // TODO:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.

  // wi + wo = 2(wo * n)n 
  // => wi = -wo + 2(wo * n)n 
  Vector3D temp(-wo.x, -wo.y, wo.z);
  temp.normalize();
  *wi = temp;
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {

  // TODO:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  
  Vector3D n = Vector3D(0, 0, 1);
  // Vector3D wo_temp = -wo;
  double ior_o; // index of fraction of current ray 
  double ior_i; // index of fraction of fractive ray
  double cos_theta_o;
  double cos_theta_i;
  double sin_theta_o;
  double sin_theta_i;

  // the ray is currently in vacuum
  if (dot(wo, n) > 0) {
    ior_o = 1.f;
    ior_i = ior;
    cos_theta_o = dot(wo, n);
  }
  else { // the ray is leaving the surface and entering vacuum
    // std::cout << "b" << std::endl;
    ior_o = ior;
    ior_i = 1.f;
    cos_theta_o = dot(wo, -n);
  }
  sin_theta_o = sqrt(fmax(0.0, 1.0 - cos_theta_o * cos_theta_o));
  double frag = 1 - pow((ior_o / ior_i), 2) * pow(sin_theta_o, 2);

  // total interal refraction
  if (frag < 0) {
      // std::cout << "happen" << std::endl;
      Vector3D temp(-wo.x, -wo.y, wo.z);
      temp.normalize();
      *wi = temp;
      return false;
  }

  sin_theta_i = ior_o * sin_theta_o / ior_i;
  cos_theta_i = sqrt(frag);

  // since three coordinates of wi should be flip of wo
  double xs = -sin_theta_i  * cos_phi(wo);
  double ys = -sin_theta_i  * sin_phi(wo);
  double zs;
  if (dot(wo, n) > 0) {
    // std::cout << "a" << std::endl;
    zs = -cos_theta_i;
  }
  else {
    // std::cout << "c" << std::endl;
    zs = cos_theta_i;
  }
  

  *wi = Vector3D(xs, ys, zs);

  return true;
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CMU462
