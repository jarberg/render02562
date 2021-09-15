// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Phong.h"

using namespace optix;

#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154
#endif

float3 Phong::shade(const Ray& r, HitInfo& hit, bool emit) const
{
  float3 rho_d = get_diffuse(hit);
  float3 rho_s = get_specular(hit);
  float s = get_shininess(hit);

  // Implement Phong reflection here.
  //
  // Input:  r          (the ray that hit the material)
  //         hit        (info about the ray-surface intersection)
  //         emit       (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Lambertian.h, Ray.h, and above):
  // lights             (vector of pointers to the lights in the scene)
  // hit.position       (position where the ray hit the material)
  // hit.shading_normal (surface normal where the ray hit the material)
  // rho_d              (difuse reflectance of the material)
  // rho_s              (specular reflectance of the material)
  // s                  (shininess or Phong exponent of the material)
  //
  // Hint: Call the sample function associated with each light in the scene.
  float3 pos = hit.position;
  float3 dir;
  float3 result;

  for each (auto light in lights) {
      float3 L = make_float3(0.0);
      float3 accum = make_float3(0.0);
      auto samplers = light->get_no_of_samples();
      bool notOccluded = light->sample(pos, dir, L);
      if (notOccluded) {
          
          for (int x = 0; x < samplers; ++x) {
              auto angle = dot(dir, hit.shading_normal);
              auto wr = 2 * angle * hit.shading_normal;
              if (angle > 0) {
                  auto t1 = rho_s * (s + 2);
                  auto t2 = t1 / 2 * M_PIf;
                  auto t3 = t2 * pow(dot(r.origin, wr), s);
                  auto t4 = t3 * angle*L;
                  accum += rho_d*M_1_PIf+t4;
              }
              accum /= samplers;
          }
          result += accum;
      }
  }
  return result + Lambertian::shade(r, hit, emit);
}
