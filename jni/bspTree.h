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

#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#include "globals.h"
#include "plane.h"
#include "bspCommon.h"
#include "array.hpp"

class C_BspTree {
		friend class C_BspNode;

	private:
		C_BspNode* headNode;

		brush* pBrushes;
		int nBrushes;
		int nPolys;

		// Scale the map
		float scaleFactor;

		// Calculate normal vectors
		void CalcNorms(void);

		C_Array<poly*> tessellatedPolys;
		poly** pRawPolys;

		USHORT nLeavesToDraw;
		USHORT nNodesToDraw;

	public:
		C_BspTree(USHORT depth);
		~C_BspTree();

		// Read geometry file
		bool ReadGeometryFile(const char* fileName);

		void WritePVSFile(const char *fileName);
		bool ReadPVSFile(const char *fileName);

		void BuildBspTree(void);
		void BuildPVS(void);

		void TraceVisibility(void);
		bool CheckVisibility(C_BspNode *node1 , C_BspNode *node2);
		bool RayIntersectsSomethingInTree(C_BspNode *node , C_Vertex *start , C_Vertex *end);

		void TessellatePolygons(void);

		void CalculateBBoxes(void);

		void Draw(void);
		int Draw2(C_Vector3* cameraPosition);
		void Draw3(void);
		int Draw_PVS(C_Vector3* cameraPosition);

		// Max depth allowed
		USHORT maxDepth;
		// Number of polygon splits happen while building the tree
		ULONG nSplits;

		// Limits reached
		int depthReached;
		int lessPolysInNodeFound;

		USHORT nLeaves;
		USHORT nNodes;
		USHORT nConvexRooms;

		// Keep all the leaves for easy reference
		C_Array<C_BspNode*> leaves;

		void FindConnectedLeaves(void);

		void IncreaseLeavesDrawn(); // { if ( nLeavesToDraw < nLeaves ) nLeavesToDraw++; cout << nLeavesToDraw << endl;}
		void DecreaseLeavesDrawn(); // { if ( nLeavesToDraw > 0 ) nLeavesToDraw--; cout << nLeavesToDraw << endl;}

		void IncreaseNodesDrawn(); // { if ( nLeavesToDraw < nLeaves ) nLeavesToDraw++; cout << nLeavesToDraw << endl;}
		void DecreaseNodesDrawn(); // { if ( nLeavesToDraw > 0 ) nLeavesToDraw--; cout << nLeavesToDraw << endl;}

		inline USHORT GetnLeavesToDraw() { return nLeavesToDraw; }
		inline USHORT GetnNodesToDraw() { return nNodesToDraw; }
};

#endif
