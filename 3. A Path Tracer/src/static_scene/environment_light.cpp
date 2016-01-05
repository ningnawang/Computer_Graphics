#include "environment_light.h"

#include <iostream>


namespace CMU462 { namespace StaticScene {

EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
  // TODO: initialize things here as needed
    this->envMap = envMap;

  // // compute p(theta, phi) based on area of unit sphere (sin_theta * d(theta) * d(phi))
  // double sum_area;
  // for (int h = 0; h < envMap->h; h++) {
  //   for (int w = 0; w < envMap->w; w++) {
  //     sum_area += envMap->data[w + h * envMap->w].illum() * sin(w);
  //   }
  // }
  // // HDRImageBuffer* envmap_w;
  // for (int h = 0; h < envMap->h; h++) {
  //   for (int w = 0; w < envMap->w; w++) {
  //     this->envMap->data[w + h * envMap->w] = envMap->data[w + h * envMap->w] * (1.0 / sum_area);
  //   }
  // }

}


Spectrum EnvironmentLight::sample_L(const Vector3D& p, Vector3D* wi,
                                    float* distToLight,
                                    float* pdf) const {
  // TODO: Implement

	// uniformly sample the sphere
  double u1 = (double)(std::rand()) / RAND_MAX;
  double u2 = (double)(std::rand()) / RAND_MAX;

  double theta = acos(2 * u1 - 1);
  double phi = 2.0 * PI * u2;

  double xs = sinf(theta) * cosf(phi);
  double ys = sinf(theta) * sinf(phi);
  double zs = cosf(theta);

  Vector3D dir = Vector3D(xs, ys, zs);

  Matrix3x3 sampleToWorld;
  sampleToWorld[0] = Vector3D(1,  0,  0);
  sampleToWorld[1] = Vector3D(0,  0, -1);
  sampleToWorld[2] = Vector3D(0,  1,  0);
 
  *wi = sampleToWorld * dir;
  (*wi).normalize();
  *distToLight = INF_D;
  *pdf = 1.0 / (4.0 * PI);

  // look up the appropriate radiance value 
  // in the texture map using bilinear interpolation
  Vector3D wi_temp = *wi;

  double phi_w = atan2(wi_temp.x, -wi_temp.z) + PI; // width of env map
  double theta_w = acos(wi_temp.y); // height of env map

  // mapping from normalized space to texture space
  float su = (phi_w / (2.f * PI)) * envMap->w;
  float sv = (theta_w / PI) * envMap->h;

  // interpolate the 4 nearest texels indices
  int u00_index = (int) (floor(su - 0.5) < 0.0f ? 0.0f : floor(su - 0.5));
  int v00_index = (int) (floor(sv - 0.5) < 0.0f ? 0.0f : floor(sv - 0.5));
  // 4 nearest sample points in texture space
  double u00 = (double) u00_index + 0.5;
  double v00 = (double) v00_index + 0.5;


  double u_ratio = std::abs(su - u00);
  double v_ratio = std::abs(sv - v00);
  double u_opposite = 1 - u_ratio;
  double v_opposite = 1 - v_ratio;

  // Get color of each texel
  Spectrum c00, c10, c01, c11, c_weighted;
  // MipLevel& mip = tex.mipmap[level];
  c00 = envMap->data[ u00_index      +  v00_index      * envMap->w];
	c10 = envMap->data[(u00_index + 1) +  v00_index      * envMap->w];
	c01 = envMap->data[ u00_index      + (v00_index + 1) * envMap->w];
	c11 = envMap->data[(u00_index + 1) + (v00_index + 1) * envMap->w];

  // return the weighted-average color
  c_weighted = (c00 * u_opposite + c10 * u_ratio) * v_opposite +
    (c01 * u_opposite + c11 * u_ratio) * v_ratio;

  return c_weighted;

}

Spectrum EnvironmentLight::sample_dir(const Ray& r) const {
  // TODO: Implement

  Vector3D new_p = r.d;

  double phi = atan2(new_p.x, -new_p.z) + PI; //width (0, 2PI)
  double theta = acos(new_p.y); //height (0, PI)

  // mapping from normalized space to texture space
  float su = (phi / (2.f * PI)) * envMap->w;
  float sv = (theta / PI) * envMap->h;

  Spectrum c = envMap->data[(int)(su) + (int)(sv) * envMap->w];

  return c;
  // return Spectrum(0, 0, 0);
}

} // namespace StaticScene
} // namespace CMU462
