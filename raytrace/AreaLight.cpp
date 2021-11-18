// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "IndexedFaceSet.h"
#include "ObjMaterial.h"
#include "mt_random.h"
#include "cdf_bsearch.h"
#include "HitInfo.h"
#include "AreaLight.h"

using namespace optix;

bool AreaLight::sample(const float3& pos, float3& dir, float3& L) const {

	int i = mt_random() * mesh->normals.no_faces() - 1;

	float visibility = 1;

	const IndexedFaceSet& normals = mesh->normals;
	const std::vector<float>& areas = mesh->face_areas;

	float3 faceSum = make_float3(0); // Intensity

	float e1 = mt_random();
	float e2 = mt_random();

	float u = 1 - sqrt(e1);
	float v = (1 - e2) * sqrt(e1);
	float w = e2 * sqrt(e1);

	float3 gx = mesh->geometry.vertex(mesh->geometry.face(i).x);
	float3 gy = mesh->geometry.vertex(mesh->geometry.face(i).y);
	float3 gz = mesh->geometry.vertex(mesh->geometry.face(i).z);

	float3 nx = normals.vertex(normals.face(i).x);
	float3 ny = normals.vertex(normals.face(i).y);
	float3 nz = normals.vertex(normals.face(i).z);

	float3 nej = normalize(u * nx + v * ny + w * nz);
	float3 xej = u * gx + v * gy + w * gz;

	L = make_float3(0.0f);
	dir = xej-pos;
	float l = length(dir);
	dir = normalize(dir);


	if (shadows) {
		// Test if the geometry is in shadow from the point light
		float err = 1e-4;
		auto ray = Ray(pos, dir, 0, err, l - err);
		HitInfo hitInfo;
		tracer->trace_to_any(ray, hitInfo);

		if (hitInfo.has_hit)
			return false;
	}

	float3 wj = dir / l;

	float faceArea = areas[i];

	faceSum = mesh->geometry.no_faces()*get_emission(i)*dot(nej, -wj) * faceArea; 

	L = faceSum / (l * l); 

	return true;


	// Compute output and return value given the following information.
	//
	// Input:  pos  (the position of the geometry in the scene)
	//
	// Output: dir  (the direction toward the light)
	//         L    (the radiance received from the direction dir)
	//
	// Return: true if not in shadow
	//
	// Relevant data fields that are available (see Light.h and above):
	// shadows             (on/off flag for shadows)
	// tracer              (pointer to ray tracer)
	// normals             (indexed face set of vertex normals)
	// mesh->face_areas    (array of face areas in the light source)
	//
	// Hints: (a) Use mesh->compute_bbox().center() to get the center of 
	//        the light source bounding box.
	//        (b) Use the function get_emission(...) to get the radiance
	//        emitted by a triangle in the mesh.
}

bool AreaLight::emit(Ray& r, HitInfo& hit, float3& Phi) const
{
	// Generate and trace a ray carrying radiance emitted from this area light.
	//
	// Output: r    (the new ray)
	//         hit  (info about the ray-surface intersection)
	//         Phi  (the flux carried by the emitted ray)
	//
	// Return: true if the ray hits anything when traced
	//
	// Relevant data fields that are available (see Light.h and Ray.h):
	// tracer              (pointer to ray tracer)
	// geometry            (indexed face set of triangle vertices)
	// normals             (indexed face set of vertex normals)
	// no_of_faces         (number of faces in triangle mesh for light source)
	// mesh->surface_area  (total surface area of the light source)
	// r.origin            (starting position of ray)
	// r.direction         (direction of ray)

	// Get geometry info
	const IndexedFaceSet& geometry = mesh->geometry;
	const IndexedFaceSet& normals = mesh->normals;
	const float no_of_faces = static_cast<float>(geometry.no_faces());

	// Sample ray origin and direction

	// Trace ray

	// If a surface was hit, compute Phi and return true

	return false;
}

float3 AreaLight::get_emission(unsigned int triangle_id) const
{
	const ObjMaterial& mat = mesh->materials[mesh->mat_idx[triangle_id]];
	return make_float3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
}
