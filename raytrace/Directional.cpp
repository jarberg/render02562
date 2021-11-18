// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <sstream>
#include <string>
#include <optix_world.h>
#include "HitInfo.h"
#include "Directional.h"

using namespace std;
using namespace optix;

bool Directional::sample(const float3& pos, float3& dir, float3& L) const
{
	L = make_float3(0.0f);

	dir = -light_dir;
	dir = normalize(dir);

	if (shadows) {
		// Test if the geometry is in shadow from the point light
		float err = 0.0001;
		auto ray = Ray(pos, dir, 0, err, RT_DEFAULT_MAX);
		HitInfo hitInfo;
		tracer->trace_to_any(ray, hitInfo);

		if (hitInfo.has_hit)
			return false;
	}

	float visibility = 1;
	L = visibility * emission;
	return true;
  // Compute output and return value given the following information.
  //
  // Input:  pos (the position of the geometry in the scene)
  //
  // Output: dir (the direction toward the light)
  //         L   (the radiance received from the direction dir)
  //
  // Return: true if not in shadow
  //
  // Relevant data fields that are available (see Directional.h and Light.h):
  // shadows    (on/off flag for shadows)
  // tracer     (pointer to ray tracer)
  // light_dir  (direction of the emitted light)
  // emission   (radiance of the emitted light)

  return false;
}

string Directional::describe() const
{
  ostringstream ostr;
  ostr << "Directional light (emitted radiance " << emission << ", direction " << light_dir << ").";
  return ostr.str();
}
