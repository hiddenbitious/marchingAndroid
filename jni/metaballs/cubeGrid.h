#ifndef _CUBEGRID_H_
#define _CUBEGRID_H_

#include "../glsl/glsl.h"
#include "../globals.h"
#include "../bbox.h"
#include "metaball.h"
#include "../frustum.h"

#define CUBES_PER_AXIS	50
#define CUBE_SIZE		1
#define THRESHOLD		1.0f
#define MAX_TRIANGLES	5000

struct grid_triangle {
	C_Vertex vertex0;
	C_Vertex normal0;
	C_Vertex vertex1;
	C_Vertex normal1;
	C_Vertex vertex2;
	C_Vertex normal2;
};

struct grid_cube_vertex {
	float value;
	C_Vertex position;
	C_Vertex normal;
};

struct grid_cube {
	grid_cube_vertex *vertices[8];
};

class C_CubeGrid {
public:
	void Constructor(float x , float y , float z);

	/// Grid's position
	C_Vertex position;
	C_BBox bbox;

	/// Number of triangles drawn
	unsigned int nTriangles;

	/// Array with all the cubes consisting the cube
	grid_cube gridCubes[CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS];
	grid_cube_vertex gridCubeVertices[(CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1)];

	unsigned int nGridCubes;
	unsigned int nGridCubeVertices;

	/// Actual geometry
	grid_triangle *geometry;

	/// Updates ball positions
	void Update(C_Metaball *metaballs , int nBalls , C_Frustum *frustum);

	/// Drawing functions
	int Draw(C_Frustum *frustum);
	void DrawGridCube(void);

private:
	static C_GLShaderManager shaderManager;
	C_GLShader* shader;
	GLint verticesAttribLocation, normalsAttribLocation;
};

#endif
