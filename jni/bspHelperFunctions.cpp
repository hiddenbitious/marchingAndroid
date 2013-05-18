/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#include "bspHelperFunctions.h"

bool PointInTriangle(C_Vertex* point , poly* triangle)
{
	C_Plane plane(&triangle->pVertices[0] , &triangle->pVertices[1] , &triangle->pVertices[2]);

	// This is cheaper. Calculate it first.
	float dist = plane.distanceFromPoint(point);
	if(FLOAT_EQ(dist, 0.0f) == false) {
		return false;
	}

	float u , v;
	CalculateUV(&plane , point , &u , &v);
	return ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f));
}


bool RayTriangleIntersection(C_Vertex* p1 , C_Vertex* p2 , poly* triangle)
{
	C_Plane plane(&triangle->pVertices[0] , &triangle->pVertices[1] , &triangle->pVertices[2]);

	// First check ray with plane
	C_Vertex interPoint;
	if(FindIntersectionPoint_withCheck(p1 , p2 , &plane , &interPoint) == false) {
		return false;
	}

	// If ray hits plane check if the iPoint is in Triangle
	float u , v;
	CalculateUV(&plane , &interPoint , &u , &v);
	return ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f));
}


void CalculateUV(C_Plane* plane , C_Vertex* point , float* u , float* v)
{
	float dot00 , dot01 , dot02 , dot11 , dot12;
	C_Vector3 A(&plane->points[0]);
	C_Vector3 B(&plane->points[1]);
	C_Vector3 C(&plane->points[2]);
	C_Vector3 P(point);

	C_Vector3 v0 = C - A;
	C_Vector3 v1 = B - A;
	C_Vector3 v2 = P - A;

	dot00 = C_Vector3::DotProduct(&v0 , &v0);
	dot01 = C_Vector3::DotProduct(&v0 , &v1);
	dot02 = C_Vector3::DotProduct(&v0 , &v2);
	dot11 = C_Vector3::DotProduct(&v1 , &v1);
	dot12 = C_Vector3::DotProduct(&v1 , &v2);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	*u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	*v = (dot00 * dot12 - dot01 * dot02) * invDenom;
}


// The intersection polygon can vary from 3 to 6 vertices
// I will consider the bbox as 12 lines with which i will test the plane for intersection
C_Array<C_Vertex> FindBBoxPlaneIntersections(C_BBox* bbox , C_Plane* plane)
{
	C_Array<C_Vertex> points;
	C_Vertex tmpPoint;
	C_Vertex bboxVertices[8];

	bbox->GetVertices(bboxVertices);

// BOTTOM
	if(FindIntersectionPoint_withCheck(&bboxVertices[0] , &bboxVertices[1] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[1] , &bboxVertices[2] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint))	{ points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[2] , &bboxVertices[3] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[3] , &bboxVertices[0] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

// TOP
	if(FindIntersectionPoint_withCheck(&bboxVertices[4] , &bboxVertices[5] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[5] , &bboxVertices[6] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[6] , &bboxVertices[7] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[7] , &bboxVertices[4] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

// CIRCUMFERENCE
	for(USHORT i = 0 ; i < 4 ; i++) {
		if(FindIntersectionPoint_withCheck(&bboxVertices[i] , &bboxVertices[i + 4] , plane , &tmpPoint))
			if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }
	}

	return points;
}


C_Vertex FindIntersectionPoint(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane)
{
	C_Vertex intersectionPoint, planeNormal;

	C_Vertex v;
	v.x = ptB->x - ptA->x;
	v.y = ptB->y - ptA->y;
	v.z = ptB->z - ptA->z;
	planeNormal = plane->getPlaneNormal();
	float sect = -plane->distanceFromPoint(ptA) / C_Vector3::DotProduct(&planeNormal , &v);
	intersectionPoint.x = ptA->x + v.x * sect;
	intersectionPoint.y = ptA->y + v.y * sect;
	intersectionPoint.z = ptA->z + v.z * sect;

	return intersectionPoint;
}


bool FindIntersectionPoint_withCheck(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane , C_Vertex* intersectionPoint)
{
	C_Vertex v, planeNormal;

	v.x = ptB->x - ptA->x;
	v.y = ptB->y - ptA->y;
	v.z = ptB->z - ptA->z;
	planeNormal = plane->getPlaneNormal();
	float denominator = C_Vector3::DotProduct(&planeNormal, &v);

	// If denominator is 0 then line and plane are parallel so they don't intersect
	if(!denominator) {
		return false;
	}

	float sect = -plane->distanceFromPoint(ptA) / denominator;
	intersectionPoint->x = ptA->x + v.x * sect;
	intersectionPoint->y = ptA->y + v.y * sect;
	intersectionPoint->z = ptA->z + v.z * sect;

	return true;
}
