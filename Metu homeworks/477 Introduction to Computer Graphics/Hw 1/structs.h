#ifndef STRUCTS
#define STRUCTS

#include <cmath>
#include "parser.h"
using namespace parser;

typedef struct Ray
{
    Vec3f e ;
    Vec3f r_t; // s-e
    bool shadow;
} ray; 

/*
typedef struct Ray
{
    Vec3f origin;
    Vec3f direction;
    bool shadowRay;
} ray;*/

enum ObjectType{
    SPHERE, TRIANGLE, MESH 
};

typedef struct Intersect
{
	bool intersect;
	Vec3f Point;
	Vec3f normal;
	int material_id;
	ObjectType obje;
	int obje_id;
    float t;

} I_sect;



Vec3f Add(const Vec3f &a, const Vec3f &b){
    Vec3f res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res; 
}

Vec3f Sub( const Vec3f &a, const Vec3f &b){
    Vec3f res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res; 
}

Vec3f Multiply(Vec3f &a, Vec3f &b){
    Vec3f res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;
    res.z = a.z * b.z;
    return res; 
}


Vec3f Negate(Vec3f &a){
    Vec3f res;
    res.x = (-1) * a.x;
    res.y = (-1) * a.y;
    res.z = (-1) * a.z;
    return res; 
}

Vec3f Scale (Vec3f & a, float b){
    Vec3f res;
    res.x = (b) * a.x;
    res.y = (b) * a.y;
    res.z = (b) * a.z;
    return res; 
}

//dot product
float DotP(Vec3f &a, Vec3f &b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


float Length( Vec3f &a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
float Distance( Vec3f &a,  Vec3f &b)
{
	return sqrt(pow(a.x - b.x,2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

Vec3f Normalize(Vec3f &a){
    Vec3f res;
    res.x = a.x / Length(a);
    res.y = a.y / Length(a);
    res.z = a.z / Length(a);
    return res;
}

Vec3f CrossP(const Vec3f &a, const Vec3f &b){
    Vec3f result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;

    return result;
}

//intersection point calculation
Vec3f int_P( Ray &ray, float t)
{
	Vec3f result;
	result.x = ray.e.x + t*ray.r_t.x;
	result.y = ray.e.y + t*ray.r_t.y;
	result.z = ray.e.z + t*ray.r_t.z;

	return result;
}

float Det( Vec3f &a,  Vec3f &b,  Vec3f &c)
{
	return a.x * (b.y*c.z - c.y*b.z) + a.y * (c.x*b.z - b.x*c.z) + a.z * (b.x*c.y - b.y*c.x);
}

#endif // TINYXML2_INCLUDED