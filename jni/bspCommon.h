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

#ifndef _BSPCOMMON_H_
#define _BSPCOMMON_H_

#include "bbox.h"
#include "globals.h"
#include "plane.h"
#include "bspCommon.h"
#include "array.hpp"


class C_BspNode;
class C_BspTree;

#define BACK		0
#define FRONT		1
#define INTERSECTS	2
#define COINCIDENT	3

extern int polyCount;
extern int leavesDrawn;
extern int nodesDrawn;
extern int nConvexRooms;

extern C_Array<C_Vertex> debug;

struct poly {
	int nVertices;
	C_Vertex* pVertices;
	C_Vertex* pNorms;

	bool usedAsDivider;
};

struct brush {
	int nPolys;
	poly* pPolys;
};

#endif
