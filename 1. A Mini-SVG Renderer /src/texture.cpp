#include "texture.h"

#include <assert.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace CMU462 {

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



void Sampler2DImp::generate_mips(Texture& tex, int startLevel) {

  cout << "------------generate_mips-----------------" << endl;

  // NOTE(sky): 
  // The starter code allocates the mip levels and generates a level 
  // map simply fills each level with a color that differs from its
  // neighbours'. The reference solution uses trilinear filtering
  // and it will only work when you have mipmaps.

  // Task 7: Implement this

  // check start level
  if ( startLevel >= tex.mipmap.size() ) {
    std::cerr << "Invalid start level"; 
  }

  // allocate sublevels
  int baseWidth  = tex.mipmap[startLevel].width;
  int baseHeight = tex.mipmap[startLevel].height;
  int numSubLevels = (int)(log2f( (float)max(baseWidth, baseHeight)));

  numSubLevels = min(numSubLevels, kMaxMipLevels - startLevel - 1);
  tex.mipmap.resize(startLevel + numSubLevels + 1);

  int width  = baseWidth;
  int height = baseHeight;
  for (int i = 1; i <= numSubLevels; i++) {

    MipLevel& level = tex.mipmap[startLevel + i];

    // handle odd size texture by rounding down
    width  = max( 1, width  / 2); assert(width  > 0);
    height = max( 1, height / 2); assert(height > 0);

    level.width = width;
    level.height = height;
    level.texels = vector<unsigned char>(4 * width * height);
  }

  // fill all 0 sub levels with interchanging colors
  Color colors[3] = { Color(1,0,0,1), Color(0,1,0,1), Color(0,0,1,1) };
  for(size_t i = 1; i < tex.mipmap.size(); ++i) {

    Color c = colors[i % 3];
    MipLevel& mip = tex.mipmap[i];
    MipLevel& pre_mip = tex.mipmap[i - 1];

    // populate the contents of level i from the previous texture data in level (i-1)
    // Attention: Working under the condition that width & height of level (i+1) is 2 times less than level i
    Color c00, c10, c01, c11, c_final;
    int mip_index_x = 0, mip_index_y = 0;
    for (int y = 0; y < pre_mip.height; y += 2) {
      for (int x = 0; x < pre_mip.width; x += 2) {
        uint8_to_float(&c00.r, &pre_mip.texels[4 * ( x      +  y      * pre_mip.width)]);
        uint8_to_float(&c10.r, &pre_mip.texels[4 * ((x + 1) +  y      * pre_mip.width)]);
        uint8_to_float(&c01.r, &pre_mip.texels[4 * ( x      + (y + 1) * pre_mip.width)]);
        uint8_to_float(&c11.r, &pre_mip.texels[4 * ((x + 1) + (y + 1) * pre_mip.width)]);

        c_final = (float) 1 / 4 * (c00 + c10 + c01 + c11);

        float_to_uint8(&mip.texels[4 * (mip_index_x + mip_index_y * mip.width)], &c_final.r);
        mip_index_x++;
      }
      mip_index_x = 0;
      mip_index_y++;
    }

    // for(size_t i = 0; i < 4 * mip.width * mip.height; i += 4) {
    //   float_to_uint8( &mip.texels[i], &c.r );
    // }
  }

}




Color Sampler2DImp::sample_nearest(Texture& tex, 
                                   float u, float v, 
                                   int level) {

  // Task ?: Implement nearest neighbour interpolation

  // return magenta for invalid level
  if (level > kMaxMipLevels) {
    return Color(1,0,1,1);
  }

  // mapping from normalized space to texture space
  int su = u * tex.mipmap[level].width;
  int sv = v * tex.mipmap[level].height;

  // interpolate to the nearest neighbour
  su = round(su);
  sv = round(sv);

  // return the color
  Color c;
  MipLevel& mip = tex.mipmap[level];
  uint8_to_float(&c.r, &mip.texels[4 * (su + sv * tex.mipmap[level].width)]);
  return c;

}



Color Sampler2DImp::sample_bilinear(Texture& tex, 
                                    float u, float v, 
                                    int level) {

  // Task ?: Implement bilinear filtering

  // return magenta for invalid level
  if (level > kMaxMipLevels) {
    return Color(1,0,1,1);
  }

  // mapping from normalized space to texture space
  int su = u * tex.mipmap[level].width;
  int sv = v * tex.mipmap[level].height;

  // cout << tex.mipmap[level].width << endl;

  // interpolate the 4 nearest texels indices
  int u00_index = (int) floor(su - 0.5);
  int v00_index = (int) floor(sv - 0.5);
  // 4 nearest sample points in texture space
  double u00 = (double) u00_index + 0.5;
  double v00 = (double) v00_index + 0.5;

  double u_ratio = su - u00;
  double v_ratio = sv - v00;
  double u_opposite = 1 - u_ratio;
  double v_opposite = 1 - v_ratio;

  // Get color of each texel
  Color c00, c10, c01, c11, c_weighted;
  MipLevel& mip = tex.mipmap[level];
  uint8_to_float(&c00.r, &mip.texels[4 * ( u00_index      +  v00_index      * tex.mipmap[level].width)]);
  uint8_to_float(&c10.r, &mip.texels[4 * ((u00_index + 1) +  v00_index      * tex.mipmap[level].width)]);
  uint8_to_float(&c01.r, &mip.texels[4 * ( u00_index      + (v00_index + 1) * tex.mipmap[level].width)]);
  uint8_to_float(&c11.r, &mip.texels[4 * ((u00_index + 1) + (v00_index + 1) * tex.mipmap[level].width)]);

  // return the weighted-average color
  c_weighted = (c00 * u_ratio + c10 * u_opposite) * v_ratio +
               (c01 * u_ratio + c11 * u_opposite) * v_opposite;

  return c_weighted;
}




Color Sampler2DImp::sample_trilinear(Texture& tex, 
                                     float u, float v, 
                                     float u_scale, float v_scale) {

  // Task 8: Implement trilinear filtering

  // return magenta for invalid level
  // if (level > kMaxMipLevels) {
  //   return Color(1,0,1,1);
  // }
  int level_low, level_high;
  float L = sqrt(pow(u_scale * tex.width, 2) + pow(v_scale * tex.height, 2));
  float d = log2(L);
  if (d < 0.0f) {
    return sample_bilinear(tex, u, v, 0);
  }

  




  // return Color(1,0,1,1);

}

} // namespace CMU462
