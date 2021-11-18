// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "mt_random.h"
#include "sampler.h"
#include "HitInfo.h"
#include "MCGlossy.h"

using namespace optix;

#ifndef M_1_PIf
#define M_1_PIf 0.31830988618379067154
#endif

float3 MCGlossy::shade(const Ray& r, HitInfo& hit, bool emit) const
{
  if(hit.trace_depth >= max_depth)
    return make_float3(0.0f);

  float3 rho_d = get_diffuse(hit);
  float3 result = make_float3(0.0f);

  // Implement a path tracing shader here.
  //
  // Input:  r          (the ray that hit the material)
  //         hit        (info about the ray-surface intersection)
  //         emit       (passed on to Emission::shade)
  //
  // Return: radiance reflected to where the ray was coming from
  //
  // Relevant data fields that are available (see Mirror.h and HitInfo.h):
  // max_depth          (maximum trace depth)
  // tracer             (pointer to ray tracer)
  // hit.trace_depth    (number of surface interactions previously suffered by the ray)
  //
  // Hint: Use the function shade_new_ray(...) to pass a newly traced ray to
  //       the shader for the surface it hit.

    auto reflec_prop = (rho_d.x, rho_d.y, rho_d.z) / 3;
    
    if (reflec_prop <= mt_random_half_open()) {
        auto newRay = Ray();
        auto newHit = HitInfo();

        newRay.origin = hit.position;
        newRay.direction = sample_cosine_weighted(hit.shading_normal);
        newRay.tmin = 1e-4;
        newRay.tmax = RT_DEFAULT_MAX;

        newHit.trace_depth = hit.trace_depth + 1;

        tracer->trace_to_closest(newRay, newHit);

        auto f = shade_new_ray(newRay, newHit, false);
        result = rho_d * f;
    }
  return result + Phong::shade(r, hit, emit);
}
