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

float3 Phong::shade(const Ray& r, HitInfo& hit, bool emit) const{
	float3 rho_d = get_diffuse(hit);
	float3 rho_s = get_specular(hit);
	float s = get_shininess(hit);
	auto Lr = make_float3(0.0f);
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
	auto result = make_float3(0.0f);
	auto rho_s_const = rho_s * (s + 2) / (2 * M_PIf);

	for each (Light * light in lights) {
		auto Li = float3();
		auto accum = make_float3(0.0f);
		auto dir = hit.shading_normal;
		auto Lr = float3();
		
		for (size_t i = 0; i < light->get_no_of_samples(); i++) {
			if (light->sample(hit.position, dir, Li)) {
				auto n = normalize(hit.shading_normal);
				auto cosine = dot(dir, n);
				auto Wo = -r.direction;
				auto Wi = dir;
				auto Wr = 2 * dot(Wi, n) * n - Wi;

				Lr = rho_s_const * pow(fmax(dot(Wo, Wr),0), s);
				accum += (rho_d * M_1_PIf + Lr) * Li * cosine;
			}
		}
		result += accum / light->get_no_of_samples();
	}
	return result + Emission::shade(r, hit, emit);
}
