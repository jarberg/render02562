// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "ObjMaterial.h"
#include "fresnel.h"
#include "RayTracer.h"

using namespace optix;

bool RayTracer::trace_reflected(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit) const
{
	const auto n = in_hit.shading_normal;
	const auto inWi = normalize(in.direction);
	auto r = -inWi;

	out.direction = r-2 * dot(r, n) * n;
	out.origin = in_hit.position;
	out.tmin = 1e-5;
	out.tmax = RT_DEFAULT_MAX;

	out_hit.trace_depth = in_hit.trace_depth + 1;
	out_hit.ray_ior = in_hit.ray_ior;

	trace_to_closest(out, out_hit);

	return out_hit.has_hit;

	// Initialize the reflected ray and trace it.
	//
	// Input:  in         (the ray to be reflected)
	//         in_hit     (info about the ray-surface intersection)
	//
	// Output: out        (the reflected ray)
	//         out_hit    (info about the reflected ray)
	//
	// Return: true if the reflected ray hit anything
	//
	// Hints: (a) There is a reflect function available in the OptiX math library.
	//        (b) Set out_hit.ray_ior and out_hit.trace_depth.


}

bool RayTracer::trace_refracted(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit) const
{
  // Initialize the refracted ray and trace it.
  //
  // Input:  in         (the ray to be refracted)
  //         in_hit     (info about the ray-surface intersection)
  //
  // Output: out        (the refracted ray)
  //         out_hit    (info about the refracted ray)
  //
  // Return: true if the refracted ray hit anything
  //
  // Hints: (a) There is a refract function available in the OptiX math library.
  //        (b) Set out_hit.ray_ior and out_hit.trace_depth.
  //        (c) Remember that the function must handle total internal reflection.
	
	float3 normal;
	out_hit.ray_ior = get_ior_out(in, in_hit, normal);
	auto ior_ratio = in_hit.ray_ior / out_hit.ray_ior;

	auto wi = normalize(-in.direction);
	auto cosÿi = dot(wi, normal);
	auto sinÿi = sqrt(1 - cosÿi * cosÿi);
	auto cosÿt = sqrt(1 - (ior_ratio * ior_ratio) * (1 - cosÿi * cosÿi));
	auto sinÿt = ior_ratio * sinÿi;
		
	auto not_TIR = cosÿt * cosÿt < 0;
	if (not_TIR) { return false; }

	auto t = (cosÿi * normal - wi) / sinÿi;
	auto wt = t * sinÿt - normal * cosÿt;

	out.origin = in_hit.position;
	out.direction = wt;
	out.tmin = 1e-4;
	out.tmax = RT_DEFAULT_MAX;
	out_hit.trace_depth = in_hit.trace_depth + 1;
	trace_to_closest(out, out_hit);

	return out_hit.has_hit;
}

bool RayTracer::trace_refracted(const Ray& in, const HitInfo& in_hit, Ray& out, HitInfo& out_hit, float& R) const
{
  // Initialize the refracted ray and trace it.
  // Compute the Fresnel reflectance (see fresnel.h) and return it in R.
  //
  // Input:  in         (the ray to be refracted)
  //         in_hit     (info about the ray-surface intersection)
  //
  // Output: out        (the refracted ray)
  //         out_hit    (info about the refracted ray)
  //
  // Return: true if the refracted ray hit anything
  //
  // Hints: (a) There is a refract function available in the OptiX math library.
  //        (b) Set out_hit.ray_ior and out_hit.trace_depth.
  //        (c) Remember that the function must handle total internal reflection.
  R = 0.1f;
  return trace_refracted(in, in_hit, out, out_hit);
}

float RayTracer::get_ior_out(const Ray& in, const HitInfo& in_hit, float3& normal) const
{
  normal = in_hit.shading_normal;
	if(dot(normal, in.direction) > 0.0)
	{
    normal = -normal;
    return 1.0f;
  }
  const ObjMaterial* m = in_hit.material;
  return m ? m->ior : 1.0f;
}
