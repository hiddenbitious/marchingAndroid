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

#include "bspTree.h"
#include "bspNode.h"
#include "vectors.h"
#include "bspHelperFunctions.h"
#include <stdlib.h>

int polyCount;
int leavesDrawn;
int nodesDrawn;
int nConvexRooms;

C_Array<C_Vertex> debug;

C_BspTree::C_BspTree(USHORT depth)
{
	nBrushes = 0;
	nPolys = 0;
	scaleFactor = 10.0f;
	pBrushes = NULL;
	headNode = NULL;
	pRawPolys = NULL;

	maxDepth = depth;

	nSplits = 0;
	nConvexRooms = 0;

	depthReached = -1;
	nLeaves = 0;
	nLeavesToDraw = 0;
	nNodesToDraw = 0;
	nNodes = 0;
}


C_BspTree::~C_BspTree(void)
{
	// Delete data
	for(int i = 0 ; i < nBrushes ; i++) {
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			delete[] pBrushes[i].pPolys[j].pVertices;
			delete[] pBrushes[i].pPolys[j].pNorms;
		}
		delete[] pBrushes[i].pPolys;
	}

	delete[] pBrushes;
	delete[] pRawPolys;
}


void C_BspTree::IncreaseLeavesDrawn()
{
	if(nLeavesToDraw < nLeaves) { nLeavesToDraw++; }
//	cout << "Leaves drawn: " << nLeavesToDraw << endl;
}
void C_BspTree::DecreaseLeavesDrawn()
{
	if(nLeavesToDraw > 0) { nLeavesToDraw--; }
//	cout << "Leaves drawn: " << nLeavesToDraw << endl;
}

void C_BspTree::IncreaseNodesDrawn()
{
	if(nNodesToDraw < nNodes) { nNodesToDraw++; }
//	cout << "Nodes drawn: " << nNodesToDraw << endl;
}
void C_BspTree::DecreaseNodesDrawn()
{
	if(nNodesToDraw > 0) { nNodesToDraw--; }
//	cout << "Node drawn: " << nNodesToDraw << endl;
}

//bool C_BspTree::ReadGeometryFile(const char* fileName)
//{
//	printf("%s\n", __FUNCTION__);
//
//	ifstream file(fileName , ios::in | ios::binary);
//
//	if(!file.is_open()) {
//		cout << "Couldn't find bsp file." << endl;
//		return false;
//	}
//
//	file.read((char*)&nPolys , sizeof(int));
//	cout << nPolys << endl;
//	pRawPolys = new(poly*[nPolys]);
//	int currentPoly = 0;
//
//	// Read number of brushes/meshes
//	file.read((char*)&nBrushes , sizeof(int));
//	cout << nBrushes << endl;
//
//	pBrushes = new brush[nBrushes];
//
//	// For each brush...
//	for(int i = 0 ; i < nBrushes ; i++) {
//		// ...read number of polys
//		file.read((char*)&pBrushes[i].nPolys , sizeof(int));
//
//		pBrushes[i].pPolys = new poly[pBrushes[i].nPolys];
//
//		// For each poly in brush
//		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
//			// Read number of vertices
//			file.read((char*)&pBrushes[i].pPolys[j].nVertices , sizeof(int));
//
//			pBrushes[i].pPolys[j].pVertices = new C_Vertex[pBrushes[i].pPolys[j].nVertices];
//			pBrushes[i].pPolys[j].pNorms = new C_Vertex[pBrushes[i].pPolys[j].nVertices];
//
//			pRawPolys[currentPoly] = &pBrushes[i].pPolys[j];
//			pRawPolys[currentPoly]->usedAsDivider = false;
//			currentPoly++;
//
//			// Read vertices
//			for(int k = 0 ; k < pBrushes[i].pPolys[j].nVertices ; k++) {
//				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].x , sizeof(float));
//				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].y , sizeof(float));
//				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].z , sizeof(float));
//
//				pBrushes[i].pPolys[j].pVertices[k].x /= scaleFactor;
//				pBrushes[i].pPolys[j].pVertices[k].y /= scaleFactor;
//				pBrushes[i].pPolys[j].pVertices[k].z /= scaleFactor;
//			}
////			TessellatePolygon ( &pBrushes[i].pPolys[j] );
//		}
//	}
//
//	file.close();
//
//	CalcNorms();
//	return true;
//}


//void C_BspTree::Draw(void)
//{
//	for(int np = 0 ; np < nPolys ; np++) {
//		glBegin(GL_POLYGON);
//		for(int k = 0 ; k < pRawPolys[np]->nVertices ; k++) {
//			glNormal3f(pRawPolys[np]->pNorms[k].x , pRawPolys[np]->pNorms[k].y , pRawPolys[np]->pNorms[k].z);
//			glVertex3f(pRawPolys[np]->pVertices[k].x , pRawPolys[np]->pVertices[k].y , pRawPolys[np]->pVertices[k].z);
//
//			//glNormal3f ( rawPolys[np].pNorms[k].x , rawPolys[np].pNorms[k].y , rawPolys[np].pNorms[k].z );
//			//glVertex3f ( rawPolys[np].pVertices[k].x , rawPolys[np].pVertices[k].y , rawPolys[np].pVertices[k].z );
//		}
//		glEnd();
//	}
//}


void C_BspTree::CalcNorms(void)
{
//	printf("%s\n", __FUNCTION__);
	C_Vector3 norm;

	// For each brush
	for(int i = 0 ; i < nBrushes ; i++) {
		// For each poly in brush
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			norm = C_Vector3::CrossProduct2(&pBrushes[i].pPolys[j].pVertices[0] , &pBrushes[i].pPolys[j].pVertices[1] , &pBrushes[i].pPolys[j].pVertices[2]);
			norm.Normalize();
			for(int k = 0 ; k < pBrushes[i].pPolys[j].nVertices ; k++) {
				pBrushes[i].pPolys[j].pNorms[k].x = norm.x;
				pBrushes[i].pPolys[j].pNorms[k].y = norm.y;
				pBrushes[i].pPolys[j].pNorms[k].z = norm.z;
			}
//			rawPolys.push_back ( pBrushes[i].pPolys[j] );
		}
	}
}


void C_BspTree::BuildPVS(void)
{
//	printf("%s\n", __FUNCTION__);

	// An iparhei arheio me tin pliroforia diabase apo ekei
	bool pvsFileFound;
	pvsFileFound = this->ReadPVSFile("pvs_.txt");

//	ULONG start = timeGetTime ();

//	cout << "Building PVS..." << endl;
//	cout << "\tDistributing sample points...";
//	C_BspNode::DistributeSamplePoints(headNode , headNode->pointSet);
//	cout << "Done!" << endl;

//	cout << "\tFinding conected leaves...";
	if(pvsFileFound) {
//		cout << "Found in file. Skipping calculations." << endl;
	} else {
		C_BspTree::FindConnectedLeaves();
//		cout << "Done!" << endl;
	}

//	cout << "\tTracing Visibility...";
	if(pvsFileFound) {
//		cout << "Found in file. Skipping calculations." << endl;
	} else {
		C_BspTree::TraceVisibility();
//		cout << "Done!" << endl << endl;
	}

//	cout << "Done!" << endl << endl;

//	ULONG time = timeGetTime () - start;

//	cout << "Time elapsed: " << (float)(time/1000.0f) << " seconds.\n\n" << endl;

	// An den eihe brethei arheio apothikeuse gia tin epomeni ektelesi
	if(!pvsFileFound) {
//		WritePVSFile("pvs.txt");
	}
}

void C_BspTree::TraceVisibility(void)
{
	float step = 20.0f / (float)leaves.size() ;
	float progress = step;

//	cout << "\n\t0%|---------50---------|100%\n\t   ";

	for(unsigned int l1 = 0 ; l1 < leaves.size() ; l1++) {
		for(unsigned int l2 = 0 ; l2 < leaves[l1]->PVS.size() ; l2++) {
			if(leaves[l1]->nodeID == leaves[l1]->PVS[l2]->nodeID) {
				continue;
			}

			for(unsigned int l3 = 0 ; l3 < leaves[l1]->PVS[l2]->PVS.size() ; l3++) {
				if(leaves[l1]->PVS[l2]->PVS[l3]->nodeID == leaves[l1]->PVS[l2]->nodeID) {
					continue;
				}

				if(leaves[l1]->PVS[l2]->PVS[l3]->visibleFrom[leaves[l1]->nodeID] ||
						leaves[l1]->visibleFrom[leaves[l1]->PVS[l2]->PVS[l3]->nodeID]) {
					continue;
				}

				if(leaves[l1]->checkedVisibilityWith[leaves[l1]->PVS[l2]->PVS[l3]->nodeID] ||
						leaves[l1]->PVS[l2]->PVS[l3]->checkedVisibilityWith[leaves[l1]->nodeID]) {
					continue;
				}

				if(C_BspTree::CheckVisibility(leaves[l1] , leaves[l1]->PVS[l2]->PVS[l3])) {
					leaves[l1]->PVS.push_back(leaves[l1]->PVS[l2]->PVS[l3]);
					leaves[l1]->PVS[l2]->PVS[l3]->visibleFrom[leaves[l1]->nodeID] = true;

					leaves[l1]->PVS[l2]->PVS[l3]->PVS.push_back(leaves[l1]);
					leaves[l1]->visibleFrom[leaves[l1]->PVS[l2]->PVS[l3]->nodeID] = true;
				}

				leaves[l1]->checkedVisibilityWith[leaves[l1]->PVS[l2]->PVS[l3]->nodeID] = true;
				leaves[l1]->PVS[l2]->PVS[l3]->checkedVisibilityWith[leaves[l1]->nodeID] = true;
			}
		}

		progress += step;
//		cout << "\r\t   ";
//		for(int k = 0 ; k < (int)progress ; k++) {
//			cout << "*";
//		}
	}
//	cout << "   ";
}


bool C_BspTree::CheckVisibility(C_BspNode *node1 , C_BspNode *node2)
{
	for(unsigned int p1 = 0 ; p1 < node1->pointSet.size() ; p1++) {
		for(unsigned int p2 = 0 ; p2 < node2->pointSet.size() ; p2++) {
			if(false == C_BspTree::RayIntersectsSomethingInTree(headNode , &node1->pointSet[p1] , &node2->pointSet[p2])) {
				return true;
			}
		}
	}
	return false;
}


bool C_BspTree::RayIntersectsSomethingInTree(C_BspNode *node , C_Vertex *start , C_Vertex *end)
{
	if(node->isLeaf) {
		for(int cp = 0 ; cp < node->nTriangles ; cp++) {
			if(RayTriangleIntersection(start , end , &node->triangles[cp])) {
				return true;
			}
		}
		return false;
	}

	int startSide = C_BspNode::ClassifyVertex(&node->partitionPlane , start);
	int endSide = C_BspNode::ClassifyVertex(&node->partitionPlane , end);

	if((startSide == COINCIDENT && endSide == COINCIDENT) ||
			(startSide != endSide && startSide != COINCIDENT && endSide != COINCIDENT)) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->backNode , start , end)) {
			return true;
		}
		if(C_BspTree::RayIntersectsSomethingInTree(node->frontNode , start , end)) {
			return true;
		}
	}

	if(startSide == FRONT || endSide == FRONT) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->frontNode , start , end)) {
			return true;
		}
	}
	if(startSide == BACK || endSide == BACK) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->backNode , start , end)) {
			return true;
		}
	}

	return false;
}


void C_BspTree::BuildBspTree(void)
{
//	cout << "***********************************************" << endl;
//	cout << "Building bsp tree...";

//	ULONG start = timeGetTime ();

	headNode = new C_BspNode(pRawPolys , nPolys);
	C_BspNode::BuildBspTree(headNode , this);

	TessellatePolygons();

//	ULONG time = timeGetTime () - start;

//	cout << "Done!" << endl;
//
//	// Print out statistics
//	cout << "\tTotal polygons in tree before splits: " << nPolys << endl;
//	cout << "\tPolygons splitted: " << nSplits << endl;
//	cout << "\tTotal polygons in tree after splits: " << nPolys + nSplits << endl;
//	cout << "\tNumber of leaves in tree: " << nLeaves << endl;
//	cout << "\tMinimun number of polys assigned in node: " << lessPolysInNodeFound << endl;
//	cout << "\tMaximum depth allowed: " << maxDepth << endl;
//	cout << "\tDepth reached: " << depthReached << endl;
////	cout << "Time elapsed: " << (float)(time/1000.0f) << " seconds." << endl;
//	cout << "***********************************************\n\n" << endl;
}


int C_BspTree::Draw2(C_Vector3* cameraPosition)
{
	polyCount = 0;
	leavesDrawn = 0;
	nodesDrawn = 0;

	glFrontFace(GL_CW);
	C_BspNode::Draw(cameraPosition , headNode , this);
	glFrontFace(GL_CCW);

// Sxediase to epipedo diahorismou tis rizas tou dendrou
	/*	glBegin ( GL_TRIANGLES );
			glVertex3f ( headNode->partitionPlane.points[0].x , headNode->partitionPlane.points[0].y , headNode->partitionPlane.points[0].z );
			glVertex3f ( headNode->partitionPlane.points[1].x , headNode->partitionPlane.points[1].y , headNode->partitionPlane.points[1].z );
			glVertex3f ( headNode->partitionPlane.points[2].x , headNode->partitionPlane.points[2].y , headNode->partitionPlane.points[2].z );
		glEnd ();
	*/

	/*
		glDisable ( GL_LIGHTING );
		glColor3f ( .5 , 0.5 , 0.0 );
		for ( int i = 0 ; i < debug.size () ; i += 3 )
		{
			glBegin ( GL_TRIANGLES );
				glVertex3f ( debug[i  ].x , debug[i  ].y , debug[i  ].z );
				glVertex3f ( debug[i+1].x , debug[i+1].y , debug[i+1].z );
				glVertex3f ( debug[i+2].x , debug[i+2].y , debug[i+2].z );
			glEnd ();
		}
		glColor3f ( 1.0 , 1.0 , 1.0 );
		glEnable ( GL_LIGHTING );
	*/


//	glDisable ( GL_COLOR_MATERIAL );
	return polyCount;
}


//void C_BspTree::Draw3(void)
//{
//	glColor3f(1.0f , 0.0f , 0.0f);
//	leaves[nLeavesToDraw]->Draw();
//	glColor3f(1.0f , 1.0f , 1.0f);
//
//	for(unsigned int j = 0 ; j < leaves[nLeavesToDraw]->connectedLeaves.size() ; j++) {
//		leaves[nLeavesToDraw]->connectedLeaves[j]->Draw();
//	}
//}


//int C_BspTree::Draw_PVS(C_Vector3* cameraPosition)
//{
//	polyCount = 0;
//	leavesDrawn = 0;
//	nodesDrawn = 0;
//
//	for(unsigned int i = 0 ; i < leaves.size() ; i++) {
//		leaves[i]->drawn = false;
//	}
//	/*
//	// Sxediase to epipedo diahorismou tis rizas tou dendrou
//		glBegin ( GL_TRIANGLES );
//			glVertex3f ( headNode->partitionPlane.points[0].x , headNode->partitionPlane.points[0].y , headNode->partitionPlane.points[0].z );
//			glVertex3f ( headNode->partitionPlane.points[1].x , headNode->partitionPlane.points[1].y , headNode->partitionPlane.points[1].z );
//			glVertex3f ( headNode->partitionPlane.points[2].x , headNode->partitionPlane.points[2].y , headNode->partitionPlane.points[2].z );
//		glEnd ();
//	*/
//	glFrontFace(GL_CW);
//	C_BspNode::Draw_PVS(cameraPosition , headNode , this);
//	glFrontFace(GL_CCW);
//	/*
//		glDisable ( GL_LIGHTING );
//		glColor3f ( 1.0f , 0.0f , 0.0f );
//		leaves[nLeavesToDraw]->Draw ();
//		leaves[nLeavesToDraw]->DrawPointSet ();
//		glColor3f ( 1.0f , 1.0f , 1.0f );
//		for ( int i = 0 ; i < leaves[nLeavesToDraw]->PVS.size () ; i++ )
//		{
//			if ( leaves[nLeavesToDraw]->PVS[i]->drawn == true )
//				continue;
//
//			leaves[nLeavesToDraw]->PVS[i]->drawn = true;
//			leaves[nLeavesToDraw]->PVS[i]->Draw();
//			leaves[nLeavesToDraw]->PVS[i]->DrawPointSet ();
//		}
//		glEnable ( GL_LIGHTING );
//	*/
//
//	/*
//		glDisable ( GL_LIGHTING );
//		glColor3f ( .5 , 0.5 , 0.0 );
//		for ( int i = 0 ; i < debug.size () ; i += 3 )
//		{
//			glBegin ( GL_TRIANGLES );
//				glVertex3f ( debug[i  ].x , debug[i  ].y , debug[i  ].z );
//				glVertex3f ( debug[i+1].x , debug[i+1].y , debug[i+1].z );
//				glVertex3f ( debug[i+2].x , debug[i+2].y , debug[i+2].z );
//			glEnd ();
//		}
//		glColor3f ( 1.0 , 1.0 , 1.0 );
//		glEnable ( GL_LIGHTING );
//	*/
//
//	return polyCount;
//}


void C_BspTree::TessellatePolygons(void)
{
	C_BspNode::TessellatePolygonsInLeaves(headNode);
}

void C_BspTree::FindConnectedLeaves(void)
{
	for(int i = 0 ; i < nLeaves ; i++) {
		leaves[i]->visibleFrom = new bool[nNodes];
		leaves[i]->checkedVisibilityWith = new bool[nNodes];

		memset(leaves[i]->visibleFrom , false , nNodes * sizeof(bool));
		memset(leaves[i]->checkedVisibilityWith , false , nNodes * sizeof(bool));
	}

	for(int i = 0 ; i < nLeaves ; i++) {
		for(int j = 0 ; j < nLeaves ; j++) {
			if(i == j) {
				continue;
			}

			for(unsigned int p1 = 0 ; p1 < leaves[i]->pointSet.size(); p1++) {
				for(unsigned int p2 = 0 ; p2 < leaves[j]->pointSet.size(); p2++) {
					if((leaves[i]->pointSet[p1].x == leaves[j]->pointSet[p2].x) &&
							(leaves[i]->pointSet[p1].y == leaves[j]->pointSet[p2].y) &&
							(leaves[i]->pointSet[p1].z == leaves[j]->pointSet[p2].z)) {
						if(leaves[j]->visibleFrom[leaves[i]->nodeID] == false) {
							leaves[i]->connectedLeaves.push_back(leaves[j]);
							leaves[i]->PVS.push_back(leaves[j]);
							leaves[j]->visibleFrom[leaves[i]->nodeID] = true;
						}

						if(leaves[i]->visibleFrom[leaves[j]->nodeID] == false) {
							leaves[j]->connectedLeaves.push_back(leaves[i]);
							leaves[j]->PVS.push_back(leaves[i]);
							leaves[i]->visibleFrom[leaves[j]->nodeID] = true;
						}

						// Termatise kai to for tou p1
						p1 = leaves[i]->pointSet.size();
						break;
					}
				}
			}
		}
	}
}


//void C_BspTree::WritePVSFile(const char *fileName)
//{
//	fstream filestr;
//	filestr.open("pvs.txt" , fstream::out);
//
//	filestr << nLeaves << endl;
//
//	for(int i = 0 ; i < nLeaves ; i++) {
//		filestr << leaves[i]->nodeID << " * " << leaves[i]->PVS.size();
//
//		for(unsigned int j = 0 ; j < leaves[i]->PVS.size() ; j++) {
//			filestr << " " << leaves[i]->PVS[j]->nodeID;
//		}
//
//		filestr << endl;
//	}
//
//	filestr.close();
//}

bool C_BspTree::ReadPVSFile(const char *fileName)
{
//	fstream filestr;
//	filestr.open(fileName , fstream::in);
//
//	if(filestr.fail()) {
//		return false;
//	}
//
//	filestr >> nLeaves;
//
//	for(int i = 0 ; i < nLeaves ; i++) {
//		ULONG nodeId;
//		int size;
//
//		filestr >> nodeId;
//		filestr.ignore(3);
//		filestr >> size;
//
//		for(int j  = 0 ; j < size ; j++) {
//			filestr >> nodeId;
//
//			int k = 0;
//			while(nodeId != leaves[k++]->nodeID);
//			k--;
//			leaves[i]->PVS.push_back(leaves[k]);
//		}
//	}
//
	return true;
}
