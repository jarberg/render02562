// 02562 Rendering Framework
// Written by Jeppe Revall Frisvad, 2011
// Copyright (c) DTU Informatics 2011

#include <optix_world.h>
#include "HitInfo.h"
#include "Triangle.h"

using namespace optix;

bool intersect_triangle(const Ray& ray, 
                        const float3& v0, 
                        const float3& v1, 
                        const float3& v2, 
                        float3& n,
                        float& t,
                        float& v,
                        float& w)
{
  // Implement ray-triangle intersection here (see Listing 1 in the lecture note).
  // Note that OptiX also has an implementation, so you can get away
  // with not implementing this function. However, I recommend that
  // you implement it for completeness.
    auto e1 = v1 - v0;
    auto e2 = v0 - v2;
    auto a = v0 - ray.origin;
    auto aa = cross(a, ray.direction);
    auto vecN = cross(e1, e2);

    float det = dot(ray.direction, vecN);
    if (det == 0) return false;

    t = dot(a, vecN) / det;
    v = dot(aa, e2) / det;
    w = dot(aa, e1) / det;

  return v >= 0 && w >= 0 && (v + w) <= 1;;
}


bool Triangle::intersect(const Ray& r, HitInfo& hit, unsigned int prim_idx) const
{
  // Implement ray-triangle intersection here.
  //
  // Input:  r                    (the ray to be checked for intersection)
  //         prim_idx             (index of the primitive element in a collection, not used here)
  //
  // Output: hit.has_hit          (set true if the ray intersects the triangle)
  //         hit.dist             (distance from the ray origin to the intersection point)
  //         hit.position         (coordinates of the intersection point)
  //         hit.geometric_normal (the normalized normal of the triangle)
  //         hit.shading_normal   (the normalized normal of the triangle)
  //         hit.material         (pointer to the material of the triangle)
  //        (hit.texcoord)        (texture coordinates of intersection point, not needed for Week 1)
  //
  // Return: True if the ray intersects the triangle, false otherwise
  //
  // Relevant data fields that are available (see Triangle.h)
  // r                            (the ray)
  // v0, v1, v2                   (triangle vertices)
  // (t0, t1, t2)                 (texture coordinates for each vertex, not needed for Week 1)
  // material                     (material of the triangle)
  //
  // Hint: Use the function intersect_triangle(...) to get the hit info.
  //       Note that you need to do scope resolution (optix:: or just :: in front
  //       of the function name) to choose between the OptiX implementation and
  //       the function just above this one.

    float3 n;
    float t;
    float v;
    float w;

    auto intersect = optix::intersect_triangle(r, v0, v1, v2, n, t, v, w);
    if (!intersect) return false;

    hit.dist = t;
    hit.has_hit = true;
    hit.geometric_normal = n;
    hit.shading_normal = n;
    hit.material = &material;
    hit.position = r.origin + t*r.direction;
    
    return true;
}

void Triangle::transform(const Matrix4x4& m) 
{ 
  v0 = make_float3(m*make_float4(v0, 1.0f)); 
  v1 = make_float3(m*make_float4(v1, 1.0f)); 
  v2 = make_float3(m*make_float4(v2, 1.0f)); 
}

Aabb Triangle::compute_bbox() const
{
  Aabb bbox;
  bbox.include(v0);
  bbox.include(v1);
  bbox.include(v2);
  return bbox;
}
