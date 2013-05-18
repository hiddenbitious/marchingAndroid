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

#ifndef _BSPHELPERFUNCTIONS_H_
#define _BSPHELPERFUNCTIONS_H_

#include "bspNode.h"
#include "bspTree.h"
#include "array.hpp"

void CalculateUV(C_Plane* plane , C_Vertex* P , float* u , float* v);
C_Array<C_Vertex> FindBBoxPlaneIntersections(C_BBox* bbox , C_Plane* plane);

/// If we know that the line formed from ptA and ptB intersects the plane
/// this function finds the intersection point and returns it.
C_Vertex FindIntersectionPoint(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane);
bool FindIntersectionPoint_withCheck(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane , C_Vertex* intePoint);

bool PointInTriangle(C_Vertex* points , poly* triangle);
bool RayTriangleIntersection(C_Vertex* p1 , C_Vertex* p2 , poly* triangle);

#endif
