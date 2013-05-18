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

#ifndef _C_BSPNODE_H_
#define _C_BSPNODE_H_

#include "bspCommon.h"

class C_BspNode {
		friend class C_BspTree;

	public:
		// Node's ID
		ULONG nodeID;

		USHORT depth;
		bool *visibleFrom;
		bool *checkedVisibilityWith;

		// Plane used to classify the polygons
		C_Plane partitionPlane;

		// If this node is leaf
		bool isLeaf;

		// Number of polys in node
		int nPolys;

		// Actual polygon data
		poly** geometry;

		// Same poly tesselated into tirangles
		int nTriangles;
		poly* triangles;


		// Children nodes
		C_BspNode* frontNode;
		C_BspNode* backNode;
		C_BspNode* fatherNode;
		C_BspTree* tree;

		// Node's bounding box.
		C_BBox bbox;

		// Algorithm that chooses a polygon from a given geometry set and returns it as a splitting plane
		static bool SelectPartitionfromList(poly** geometry , int nPolys , C_Plane* finalPlane);
		// Tests if polygon is in front, back, coincident or it spans the given plane
		static int ClassifyPolygon(C_Plane* plane , poly* polygon);
		// Tests if a vertex against a plane
		static int ClassifyVertex(C_Plane* plane , C_Vertex* vertex);
		// Splits the given polygon in two new polygon. The polygon must be spanning the plane given
		static void SplitPolygon(C_Plane* plane , poly* polygon , poly** front , poly** back);

		static bool IsConvex(poly** polys , int nPolys);

		C_BspNode();
		C_BspNode(poly** geom , int nPolys);
		~C_BspNode();

		// Recursively builds the tree
		static void BuildBspTree(C_BspNode* node , C_BspTree* tree);

		// Recursively calculates the bbox on leaves. Uses the polygon data. Not the triangles.
		static void CalculateBBoxes(C_BspNode* node);

		// Calculate node's bbox
		void CalculateBBox(void);

		void DistributePointsAlongPartitionPlane(void);
		static void DistributeSamplePoints(C_BspNode* node , C_Array<C_Vertex>& points);
		static void CleanUpPointSet(C_BspNode* node , C_Array<C_Vertex>& points);

		C_Array<C_BspNode*> connectedLeaves;
		C_Array<C_BspNode*> PVS;
		bool drawn;

		C_Array<C_Vertex> pointSet;

		void DrawPointSet(void);


		static void Draw(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree);
		static void Draw_PVS(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree);
		void Draw(void);

		/// Brakes down CONVEX polygon of 4 or 5 vertices into triangles.
		/// Also it frees any memory reserved for the previous polygons
		/// (not the data itself but the pointers).
		/// New pointers are stored at the triangles var.
		static void TessellatePolygonsInLeaves(C_BspNode* node);
};

#endif

