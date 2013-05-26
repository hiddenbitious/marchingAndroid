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

#include "bspNode.h"
#include "bspTree.h"
#include "bspHelperFunctions.h"
#include "bspNode.h"
#include "debug.h"

#define MINIMUMRELATION			0.5f
#define MINIMUMRELATIONSCALE	2.0f

#define NPOINTS_U 10
#define NPOINTS_V 10

C_BspNode::C_BspNode(void)
{
	isLeaf = false;
	nodeID = 0;
	nPolys = 0;
	frontNode = NULL;
	backNode = NULL;
	fatherNode = NULL;
	geometry = NULL;
	depth = 0;
	nTriangles = 0;
	triangles = NULL;
	checkedVisibilityWith = NULL;
	visibleFrom = NULL;
}


C_BspNode::C_BspNode(poly** geometry , int nPolys)
{
	this->geometry = geometry;
	isLeaf = false;
	nodeID = 0;
	this->nPolys = nPolys;
	frontNode = NULL;
	backNode = NULL;
	fatherNode = NULL;

	nTriangles = 0;
	triangles = NULL;
	depth = 0;
	checkedVisibilityWith = NULL;
	visibleFrom = NULL;
}


C_BspNode::~C_BspNode()
{
	delete[] frontNode;
	delete[] backNode;

	if(checkedVisibilityWith != NULL) {
		delete[] checkedVisibilityWith;
	}
	if(visibleFrom != NULL) {
		delete[] visibleFrom;
	}
}


int C_BspNode::ClassifyVertex(C_Plane *plane , C_Vertex *vertex)
{
	float dist = plane->distanceFromPoint(vertex);

//	if(dist > EPSILON) {
//		return FRONT;
//	} else if(dist < -EPSILON) {
//		return BACK;
//	} else if(dist >= -EPSILON && dist <= EPSILON) {
//		return COINCIDENT;
//	}

	if(dist >= -EPSILON && dist <= EPSILON) {
		return COINCIDENT;
	} else 	if(dist > EPSILON) {
		return FRONT;
	} else {
		return BACK;
	}
}


int C_BspNode::ClassifyPolygon(C_Plane* plane , poly* polygon)
{
	int front , back;
	front = back = 0;

	for(int i = 0 ; i < polygon->nVertices ; i++) {
		int whereIs = ClassifyVertex(plane , &(polygon->pVertices[i]));

		if(whereIs == FRONT) {
			front++;
		} else if(whereIs == BACK) {
			back++;
		}
	}

	if(front && !back) {
		return FRONT;
	} else if(!front && back) {
		return BACK;
	} else if(!front && !back) {
		return COINCIDENT;
	} else {
		return INTERSECTS;
	}
}


bool C_BspNode::IsConvex(poly** polys , int nPolys)
{
	C_Plane plane;

	for(int i = 0 ; i < nPolys ; i++) {
		plane = C_Plane(&polys[i]->pVertices[0] , &polys[i]->pVertices[1] , &polys[i]->pVertices[2]);

		for(int j = 0 ; j < nPolys ; j++) {
			if(i == j) {
				continue;
			}

			int type = ClassifyPolygon(&plane , polys[j]);
			if(type == BACK || type == INTERSECTS) {
				return false;
			}
		}
	}
	return true;
}


void C_BspNode::BuildBspTree(C_BspNode* node , C_BspTree *tree)
{
	static int ID = 0;
	static int maxDepth = tree->maxDepth;
	node->tree = tree;

	C_Plane tempPlane;

	/// If the geometry is convex
	/// or if this node reaches the tree's node limit
	/// then mark this node as a leaf
	if(C_BspNode::IsConvex(node->geometry , node->nPolys) || node->depth >= maxDepth) {
		if(C_BspNode::IsConvex(node->geometry , node->nPolys)) {
			tree->nConvexRooms++;
		}

		node->isLeaf = true;
		tree->nLeaves++;
		tree->leaves.push_back(node);

		if(node->depth > tree->depthReached) {
			tree->depthReached = node->depth;
		}
		if(node->nPolys < tree->lessPolysInNodeFound) {
			tree->lessPolysInNodeFound = node->nPolys;
		}

		/// Break recursion
		return;
	}

	/// Search for a partitioning plane
	SelectPartitionfromList(node->geometry , node->nPolys , &tempPlane);
	node->partitionPlane.setPlane(&tempPlane);

	c_assert(!node->frontNode);
	node->frontNode = new C_BspNode();
	node->frontNode->depth = node->depth + 1;
	node->frontNode->fatherNode = node;

	c_assert(!node->backNode);
	node->backNode = new C_BspNode();
	node->backNode->depth = node->depth + 1;
	node->backNode->fatherNode = node;

	int result , nFront , nBack;
	nFront = nBack = 0;

	/// Classify all polygons in this node
	for(int i = 0 ; i < node->nPolys ; i++) {
		result = C_BspNode::ClassifyPolygon(&node->partitionPlane , node->geometry[i]);

		if(result == FRONT) {
			nFront++;
		} else if(result == BACK) {
			nBack++;
		} else if(result == INTERSECTS) {
			nFront++;
			nBack++;
		} else if(result == COINCIDENT) {
			nFront++;
		}
	}

	/// Allocate memory
	node->nodeID = ID++;

	node->backNode->geometry = new(poly*[nBack]);
	node->backNode->nPolys = nBack;
	node->backNode->nodeID = ID++;

	node->frontNode->geometry = new(poly*[nFront]);
	node->frontNode->nPolys = nFront;
	node->frontNode->nodeID = ID++;

	tree->nNodes = ID;

	nFront = nBack = 0;

	for(int i = 0 ; i < node->nPolys ; i++) {
		result = C_BspNode::ClassifyPolygon(&node->partitionPlane , node->geometry[i]);

		if(result == FRONT) {
			node->frontNode->geometry[nFront++] = node->geometry[i];
		} else if(result == BACK) {
			node->backNode->geometry[nBack++] = node->geometry[i];
		} else if(result == INTERSECTS) {
			C_BspNode::SplitPolygon(&node->partitionPlane , node->geometry[i] , &node->frontNode->geometry[nFront++] , &node->backNode->geometry[nBack++]);
			tree->nSplits++;
		} else if(result == COINCIDENT) {
			node->frontNode->geometry[nFront++] = node->geometry[i];
		}
	}

	node->CalculateBBox();

	if(nFront) {
		C_BspNode::BuildBspTree(node->frontNode , tree);

		node->frontNode->CalculateBBox();
		if(node->frontNode->isLeaf == false) {
			delete[] node->frontNode->geometry;
			node->frontNode->geometry = NULL;
		}
	}

	if(nBack) {
		C_BspNode::BuildBspTree(node->backNode , tree);

		node->backNode->CalculateBBox();
		if(node->backNode->isLeaf == false) {
			delete[] node->backNode->geometry;
			node->backNode->geometry = NULL;
		}
	}
}

bool C_BspNode::SelectPartitionfromList(poly** geometry , int nPolys , C_Plane* finalPlane)
{
	unsigned int nFront , nBack , nSplits , bestPlane = 0, bestSplits = INT_MAX;
	C_Plane tempPlane;
	bool found = false;

	float relation , bestRelation , minRelation;
	bestRelation = 0.0f;
	minRelation = MINIMUMRELATION;

	c_assert(nPolys);

	while(!found) {
		for(int currentPlane = 0 ; currentPlane < nPolys ; currentPlane++) {
			if(geometry[currentPlane]->usedAsDivider == true) {
				continue;
			}

			nBack = nFront = nSplits = 0;
			tempPlane = C_Plane(&(geometry[currentPlane]->pVertices[0]),
								&(geometry[currentPlane]->pVertices[1]),
								&(geometry[currentPlane]->pVertices[2]));

			for(int i = 0; i < nPolys; i++) {
				if(i == currentPlane) {
					continue;
				}

				int result = C_BspNode::ClassifyPolygon(&tempPlane , geometry[i]);

				if(result == FRONT) {
					nFront++;
				} else if(result == BACK) {
					nBack++;
				} else if(result == INTERSECTS) {
					nSplits++;
				}
			}

			relation = (float)MIN(nFront, nBack) / (float)MAX(nFront, nBack);
//			printf("bestSplits: %u\n", bestSplits);
//			printf("nFront: %u\n", nFront);
//			printf("nBack: %u\n", nBack);
//			printf("nSplits: %u\n", nSplits);

			if((relation > minRelation && nSplits < bestSplits) || (nSplits == bestSplits && relation > bestRelation)) {
				finalPlane->setPlane(&tempPlane);
				bestSplits = nSplits;
				bestRelation = relation;
				bestPlane = currentPlane;
				found = true;
//				printf("****\n");
			}
		}

		/// An ehoun dokimastei ola ta polygona kai den ehei brethei akoma epipedo diahorismou
		/// halarose ligo ta kritiria kai ksanapsakse
		minRelation /= MINIMUMRELATIONSCALE;
	}

	geometry[bestPlane]->usedAsDivider = true;
	/// Keep plane's information so we can draw it
	debug.push_back(finalPlane->points[0]);
	debug.push_back(finalPlane->points[1]);
	debug.push_back(finalPlane->points[2]);

	return found;
}

void C_BspNode::SplitPolygon(C_Plane* plane , poly* polygon , poly** front , poly** back)
{
	C_Array<C_Vertex> newFront;
	C_Array<C_Vertex> newBack;
	C_Vertex intersectionPoint;

	C_Vertex ptA , ptB;
	float sideA , sideB;

	int i;
	int count = polygon->nVertices;

	ptA = polygon->pVertices[count - 1];
	sideA = plane->distanceFromPoint(&ptA);

	for(i = 0 ; i < count ; i++) {
		ptB = polygon->pVertices[i];
		sideB = plane->distanceFromPoint(&ptB);

		if(sideB > 0.0f) {
			if(sideA < 0.0f) {
				intersectionPoint = FindIntersectionPoint(&ptA , &ptB , plane);

				newFront.push_back(intersectionPoint);
				newBack.push_back(intersectionPoint);
			}
			newFront.push_back(ptB);
		} else if(sideB < 0.0f) {
			if(sideA > 0.0f) {
				intersectionPoint = FindIntersectionPoint(&ptA , &ptB , plane);

				newFront.push_back(intersectionPoint);
				newBack.push_back(intersectionPoint);
			}
			newBack.push_back(ptB);
		} else {
			newFront.push_back(ptB);
			newBack.push_back(ptB);
		}

		ptA = ptB;
		sideA = sideB;
	}

	*front = new poly();
	(*front)->nVertices = newFront.size();
	(*front)->pNorms = new C_Vertex[(*front)->nVertices];
	(*front)->pVertices = new C_Vertex[(*front)->nVertices];
	(*front)->usedAsDivider = polygon->usedAsDivider;

	for(i = 0 ; i < (*front)->nVertices ; i++) {
		(*front)->pVertices[i].x = newFront[i].x;
		(*front)->pVertices[i].y = newFront[i].y;
		(*front)->pVertices[i].z = newFront[i].z;

		(*front)->pNorms[i].x = polygon->pNorms[0].x;
		(*front)->pNorms[i].y = polygon->pNorms[0].y;
		(*front)->pNorms[i].z = polygon->pNorms[0].z;
	}

	*back = new poly();
	(*back)->nVertices = newBack.size();
	(*back)->pNorms = new C_Vertex[(*back)->nVertices];
	(*back)->pVertices = new C_Vertex[(*back)->nVertices];
	(*back)->usedAsDivider = polygon->usedAsDivider;

	for(i = 0 ; i < (*back)->nVertices ; i++) {
		(*back)->pVertices[i].x = newBack[i].x;
		(*back)->pVertices[i].y = newBack[i].y;
		(*back)->pVertices[i].z = newBack[i].z;

		(*back)->pNorms[i].x = polygon->pNorms[0].x;
		(*back)->pNorms[i].y = polygon->pNorms[0].y;
		(*back)->pNorms[i].z = polygon->pNorms[0].z;
	}

	return;
}


void C_BspNode::Draw(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree)
{
	float side;
	if(!node) {
		return;
	}

	if(!node->isLeaf) {
		side = node->partitionPlane.distanceFromPoint(cameraPosition);

//		node->partitionPlane.Draw ();
//		node->bbox.Draw ( 1.0f , 0.0f , 0.0f );
//		node->DrawPointSet ();

		if(side > 0.0f) {
			C_BspNode::Draw(cameraPosition , node->backNode  , tree);
			C_BspNode::Draw(cameraPosition , node->frontNode , tree);
		} else {
			C_BspNode::Draw(cameraPosition , node->frontNode , tree);
			C_BspNode::Draw(cameraPosition , node->backNode  , tree);
		}
	} else {
		node->Draw();
		polyCount += node->nPolys;
	}
}


void C_BspNode::Draw_PVS(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree)
{
	float side;
	if(!node) {
		return;
	}

	if(!node->isLeaf) {
		side = node->partitionPlane.distanceFromPoint(cameraPosition);

		if(side > 0.0f) {
			C_BspNode::Draw_PVS(cameraPosition , node->frontNode , tree);
		} else {
			C_BspNode::Draw_PVS(cameraPosition , node->backNode  , tree);
		}
	} else {
		if(node->drawn) {
			return;
		}

		node->drawn = true;
		node->Draw();

		for(unsigned int i = 0 ; i < node->PVS.size() ; i++) {
			if(node->PVS[i]->drawn) {
				continue;
			}

			node->PVS[i]->drawn = true;

			node->PVS[i]->Draw();
//			node->PVS[i]->bbox.Draw ( 0.0f , 1.0f , 0.0f );
//			node->PVS[i]->DrawPointSet ();
			polyCount += node->PVS[i]->nPolys;
		}
	}
}


void C_BspNode::Draw(void)
{
//	glBegin(GL_TRIANGLES);
//	for(int i = 0 ; i < nTriangles ; i++) {
//		glNormal3f(triangles[i].pNorms[0].x , triangles[i].pNorms[0].y , triangles[i].pNorms[0].z);
//		glVertex3f(triangles[i].pVertices[0].x , triangles[i].pVertices[0].y , triangles[i].pVertices[0].z);
//
//		glNormal3f(triangles[i].pNorms[1].x , triangles[i].pNorms[1].y , triangles[i].pNorms[1].z);
//		glVertex3f(triangles[i].pVertices[1].x , triangles[i].pVertices[1].y , triangles[i].pVertices[1].z);
//
//		glNormal3f(triangles[i].pNorms[2].x , triangles[i].pNorms[2].y , triangles[i].pNorms[2].z);
//		glVertex3f(triangles[i].pVertices[2].x , triangles[i].pVertices[2].y , triangles[i].pVertices[2].z);
//	}
//	glEnd();
}


void C_BspNode::CalculateBBox(void)
{
	if(geometry == NULL) { return; }
	if(!nPolys) { return; }

	float minX , minY , minZ , maxX , maxY , maxZ;

	maxX = maxY = maxZ = SMALLEST_FLOAT;
	minX = minY = minZ = GREATEST_FLOAT;

	for(int i = 0 ; i < nPolys ; i++) {
		for(int k = 0 ; k < geometry[i]->nVertices; k++) {
			maxX = MAX(maxX , geometry[i]->pVertices[k].x);
			maxY = MAX(maxY , geometry[i]->pVertices[k].y);
			maxZ = MAX(maxZ , geometry[i]->pVertices[k].z);

			minX = MIN(minX , geometry[i]->pVertices[k].x);
			minY = MIN(minY , geometry[i]->pVertices[k].y);
			minZ = MIN(minZ , geometry[i]->pVertices[k].z);
		}
	}

	bbox.SetMax(maxX , maxY , maxZ);
	bbox.SetMin(minX , minY , minZ);
	bbox.SetVertices();
}


void C_BspNode::TessellatePolygonsInLeaves(C_BspNode* node)
{
	if(!node->isLeaf) {
		TessellatePolygonsInLeaves(node->backNode);
		TessellatePolygonsInLeaves(node->frontNode);

		return;
	}

	int i;

	node->nTriangles = 0;
	for(i = 0 ; i < node->nPolys ; i++) {
		node->nTriangles += node->geometry[i]->nVertices - 2;
	}

	node->triangles = new poly[node->nTriangles];

	int currentTriangle = 0;
	for(i = 0 ; i < node->nPolys ; i++) {
		switch(node->geometry[i]->nVertices) {
			case 3:
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[1];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[2];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[1];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[2];

				currentTriangle++;
				break;

			case 4:
				// Triangle 1
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[1];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[2];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[1];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[2];

				currentTriangle++;

				// Triangle 2
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[2];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[3];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[0];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[2];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[3];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[0];

				currentTriangle++;
				break;

			case 5:
				// Triangle 1
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[1];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[2];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[1];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[2];

				currentTriangle++;

				// Triangle 2
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[2];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[3];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[4];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[2];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[3];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[4];

				currentTriangle++;

				// Triangle 3
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[4];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[2];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[4];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[2];

				currentTriangle++;
				break;

			case 6: case 7:
				// Triangle 1
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[1];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[2];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[1];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[2];

				currentTriangle++;

				// Triangle 2
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[2];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[3];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[4];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[2];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[3];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[4];

				currentTriangle++;

				// Triangle 3
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[4];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[5];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[0];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[4];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[5];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[0];

				currentTriangle++;

				// Triangle 4
				node->triangles[currentTriangle].nVertices = 3;
				node->triangles[currentTriangle].pNorms = new C_Vertex[3];
				node->triangles[currentTriangle].pVertices = new C_Vertex[3];

				node->triangles[currentTriangle].pNorms[0] = node->geometry[i]->pNorms[0];
				node->triangles[currentTriangle].pNorms[1] = node->geometry[i]->pNorms[2];
				node->triangles[currentTriangle].pNorms[2] = node->geometry[i]->pNorms[4];

				node->triangles[currentTriangle].pVertices[0] = node->geometry[i]->pVertices[0];
				node->triangles[currentTriangle].pVertices[1] = node->geometry[i]->pVertices[2];
				node->triangles[currentTriangle].pVertices[2] = node->geometry[i]->pVertices[4];

				currentTriangle++;

				break;
		}
	}

	if(node->nTriangles != currentTriangle) {
		node->nTriangles = currentTriangle;
	}

	delete[] node->geometry;
	node->geometry = NULL;
}


void C_BspNode::CleanUpPointSet(C_BspNode* node , C_Array<C_Vertex>& points)
{
	unsigned int cPoint = 0;

	// Remove points outside the bbox
	while(cPoint < points.size()) {
		if(node->bbox.IsInside(&points[cPoint]) == false) {
			points.erase(cPoint);
			cPoint--;
		}

		cPoint++;
	}

	// Remove points coinciding with the triangles of the given node
	// NOTE: VERY BRUTE FORCE WAY. MUST FIND SOMETHING FASTER.
	for(int cTri = 0 ; cTri < node->nTriangles ; cTri++) {
		cPoint = 0;
		while(cPoint < points.size()) {
			if(PointInTriangle(&points[cPoint] , &node->triangles[cTri])) {
				points.erase(cPoint);
				cPoint--;
			}

			cPoint++;
		}
	}
}


void C_BspNode::DistributeSamplePoints(C_BspNode* node , C_Array<C_Vertex>& points)
{
	// CLEAN UP POINTS
	CleanUpPointSet(node , points);

	if(node->isLeaf == true) {
		node->pointSet = points;
	} else {
		float dist;
		C_Array<C_Vertex> frontPoints , backPoints;

		node->DistributePointsAlongPartitionPlane();
		frontPoints = node->pointSet;
		backPoints = node->pointSet;

		for(unsigned int i = 0 ; i < points.size() ; i++) {
			dist = node->partitionPlane.distanceFromPoint(&points[i]);

			if(dist > 0.0f) {
				frontPoints.push_back(points[i]);
			} else if(dist < 0.0f) {
				backPoints.push_back(points[i]);
			} else {
				frontPoints.push_back(points[i]);
				backPoints.push_back(points[i]);
			}
		}

		C_BspNode::DistributeSamplePoints(node->backNode , backPoints);
		C_BspNode::DistributeSamplePoints(node->frontNode , frontPoints);
	}
}


void C_BspNode::DistributePointsAlongPartitionPlane(void)
{
	C_Vertex min , max , tmp;
	min.x = min.y = min.z = GREATEST_FLOAT;
	max.x = max.y = max.z = SMALLEST_FLOAT;

	C_Array<C_Vertex> intersectionPoints = FindBBoxPlaneIntersections(&bbox , &partitionPlane);

	float maxU , maxV , minU , minV;
	float tmpU , tmpV;
	maxU = maxV = SMALLEST_FLOAT;
	minU = minV = GREATEST_FLOAT;
//	dist = SMALLEST_FLOAT;

	for(USHORT i = 0 ; i < intersectionPoints.size() ; i++) {
		CalculateUV(&partitionPlane , &intersectionPoints[i] , &tmpU , &tmpV);

		if(tmpU > maxU) { maxU = tmpU; }
		if(tmpV > maxV) { maxV = tmpV; }
		if(tmpU < minU) { minU = tmpU; }
		if(tmpV < minV) { minV = tmpV; }
	}

	float stepU = ABS(maxU - minU) / NPOINTS_U;
	float stepV = ABS(maxV - minV) / NPOINTS_V;

	C_Vector3 P;
	C_Vector3 A(&partitionPlane.points[0]);
	C_Vector3 B(&partitionPlane.points[1]);
	C_Vector3 C(&partitionPlane.points[2]);
	C_Vector3 v0 = C - A;
	C_Vector3 v1 = B - A;

	for(float uu = minU ; uu < maxU ; uu += stepU) {
		for(float vv = minV ; vv < maxV ; vv += stepV) {
			P = A + v0 * uu + v1 * vv;
			tmp.x = P.x; tmp.y = P.y; tmp.z = P.z;
			pointSet.push_back(tmp);
		}
	}
}


void C_BspNode::DrawPointSet(void)
{
	int n = pointSet.size();

//	glDisable(GL_LIGHTING);
//
//	glColor3f(0.0f , 1.0f , 0.0f);
//	glBegin(GL_POINTS);
//	for(int i = 0; i < n ; i++) {
//		glVertex3f(pointSet[i].x , pointSet[i].y , pointSet[i].z);
//	}
//	glEnd();
//
//	glEnable(GL_LIGHTING);
}
