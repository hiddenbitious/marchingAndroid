
#include "cubeGrid.h"
#include "tables.h"
#include "../mmath.h"

//#define FIXED_PIPELINE

C_GLShaderManager C_CubeGrid::shaderManager;

struct grid_vertex {
	C_Vertex vertex;
	C_Vertex normal;
};


static const char vertexShaderSource [] = {
"attribute vec4 a_vertices;\n\
attribute vec4 a_normals;\n\
\n\
uniform mat4 u_modelviewMatrix;\n\
uniform mat4 u_projectionMatrix;\n\
\n\
varying mediump vec4 v_color;\n\
\n\
void main ( void )\n\
{\n\
	mat4 mvpMatrix = u_projectionMatrix * u_modelviewMatrix;\n\
	gl_Position = mvpMatrix * a_vertices;\n\
	v_color = a_normals;\n\
}\0"};

static const char fragmentShaderSource [] = {
"varying mediump vec4 v_color;\n\
\n\
void main (void)\n\
{\n\
	gl_FragColor = v_color;\n\
}\0" };

static grid_vertex edgeVertices[12];

inline float fieldFormula(float q , float r)
{
	return q / r * 5;
}

void C_CubeGrid::Constructor(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	geometry = new grid_triangle[MAX_TRIANGLES];

	nGridCubes = CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS;
	nGridCubeVertices = (CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1);
	nTriangles = 0;

	unsigned int cc = 0;

	/// Initialize vertices
	for(int x = 0 ; x < CUBES_PER_AXIS + 1 ; x++) {
		for(int y = 0 ; y < CUBES_PER_AXIS + 1 ; y++) {
			for(int z = 0 ; z < CUBES_PER_AXIS + 1 ; z++) {
				gridCubeVertices[cc].position.x = x * CUBE_SIZE;
				gridCubeVertices[cc].position.y = y * CUBE_SIZE;
				gridCubeVertices[cc].position.z = z * CUBE_SIZE;

				gridCubeVertices[cc].value = 0.0f;

				++cc;
			}
		}
	}

	/// Initialize cubes by setting the pointers to the appropriate cube vertices
	cc = 0;
	for(int x = 0 ; x < CUBES_PER_AXIS ; x++) {
		for(int y = 0 ; y < CUBES_PER_AXIS ; y++) {
			for(int z = 0 ; z < CUBES_PER_AXIS ; z++) {
				gridCubes[cc].vertices[0] = &gridCubeVertices[(x * (CUBES_PER_AXIS + 1) + y) * (CUBES_PER_AXIS + 1) + z];
				gridCubes[cc].vertices[1] = &gridCubeVertices[(x * (CUBES_PER_AXIS + 1) + y) * (CUBES_PER_AXIS + 1) + z + 1];
				gridCubes[cc].vertices[2] = &gridCubeVertices[(x * (CUBES_PER_AXIS + 1) + (y + 1)) * (CUBES_PER_AXIS + 1) + z + 1];
				gridCubes[cc].vertices[3] = &gridCubeVertices[(x * (CUBES_PER_AXIS + 1) + (y + 1)) * (CUBES_PER_AXIS + 1) + z];

				gridCubes[cc].vertices[4] = &gridCubeVertices[((x + 1) * (CUBES_PER_AXIS + 1) + y) * (CUBES_PER_AXIS + 1) + z];
				gridCubes[cc].vertices[5] = &gridCubeVertices[((x + 1) * (CUBES_PER_AXIS + 1) + y) * (CUBES_PER_AXIS + 1) + z + 1];
				gridCubes[cc].vertices[6] = &gridCubeVertices[((x + 1) * (CUBES_PER_AXIS + 1) + (y + 1)) * (CUBES_PER_AXIS + 1) + z + 1];
				gridCubes[cc].vertices[7] = &gridCubeVertices[((x + 1) * (CUBES_PER_AXIS + 1) + (y + 1)) * (CUBES_PER_AXIS + 1) + z];

				++cc;
			}
		}
	}

	/// Initialize shader
	#ifndef FIXED_PIPELINE
	#ifndef JNI_COMPATIBLE
	shader = shaderManager.LoadShaderProgram("shaders/metaballs_shader.vert", "shaders/metaballs_shader.frag");
	#else
	shader = shaderManager.LoadShaderProgram(vertexShaderSource, fragmentShaderSource);
	#endif
	/// Get attribute locations
	verticesAttribLocation = shader->getAttribLocation("a_vertices");
	//assert(verticesAttribLocation >= 0);

	normalsAttribLocation = shader->getAttribLocation("a_normals");
	//assert(normalsAttribLocation >= 0);
	#endif

	bbox.SetMin(position.x , position.y , position.z);
	bbox.SetMax(position.x + CUBES_PER_AXIS * CUBE_SIZE ,
				position.y + CUBES_PER_AXIS * CUBE_SIZE ,
				position.z + CUBES_PER_AXIS * CUBE_SIZE);
	bbox.SetVertices();
}


void C_CubeGrid::Update(C_Metaball *metaballs , int nBalls , C_Frustum *frustum)
{
	if(frustum != NULL) {
		if(!frustum->cubeInFrustum(&bbox)) {
			return;
		}
	}

	float rad, dist, normalScale;
	C_Vertex pos;
	C_Vertex ballToPoint;

	/// Initialize
	/// TODO: Replace loop with memset. Struct has to change
	for(unsigned int i = 0 ; i < nGridCubeVertices ; i++) {
		gridCubeVertices[i].value = 0.0f;
		gridCubeVertices[i].normal.x = gridCubeVertices[i].normal.y = gridCubeVertices[i].normal.z = 0.0f;
	}
	//memset ( gridCubeVertices , 0 , nGridCubeVertices * sizeof ( grid_cube_vertex ) );

	/// Calculate field values and norms for each grid vertex
	for(int cb = 0 ; cb < nBalls ; cb++) {
		rad = metaballs[cb].radius;
		pos.x = metaballs[cb].position.x; pos.y = metaballs[cb].position.y; pos.z = metaballs[cb].position.z;

		for(unsigned int cv = 0 ; cv < nGridCubeVertices ; cv++) {
			ballToPoint.x = gridCubeVertices[cv].position.x - pos.x;
			ballToPoint.y = gridCubeVertices[cv].position.y - pos.y;
			ballToPoint.z = gridCubeVertices[cv].position.z - pos.z;

			dist = ballToPoint.x * ballToPoint.x + ballToPoint.y * ballToPoint.y + ballToPoint.z * ballToPoint.z;

			if(dist < 0.0001f) {
				dist = 0.0001f;
			}

			gridCubeVertices[cv].value += fieldFormula(rad, dist);

			//normal = (r^2 * v)/d^4
			normalScale = rad / (dist * dist);

			gridCubeVertices[cv].normal.x += ballToPoint.x * normalScale;
			gridCubeVertices[cv].normal.y += ballToPoint.y * normalScale;
			gridCubeVertices[cv].normal.z += ballToPoint.z * normalScale;
		}
	}

	// Gia kathe kibo...
	nTriangles = 0;
	for(unsigned int cb = 0; cb < nGridCubes && nTriangles < MAX_TRIANGLES; cb++) {
		int cubeIndex = 0x00;

		if(gridCubes[cb].vertices[0]->value < THRESHOLD) {
			cubeIndex |= 1;
		}
		if(gridCubes[cb].vertices[1]->value < THRESHOLD) {
			cubeIndex |= 2;
		}
		if(gridCubes[cb].vertices[2]->value < THRESHOLD) {
			cubeIndex |= 4;
		}
		if(gridCubes[cb].vertices[3]->value < THRESHOLD) {
			cubeIndex |= 8;
		}
		if(gridCubes[cb].vertices[4]->value < THRESHOLD) {
			cubeIndex |= 16;
		}
		if(gridCubes[cb].vertices[5]->value < THRESHOLD) {
			cubeIndex |= 32;
		}
		if(gridCubes[cb].vertices[6]->value < THRESHOLD) {
			cubeIndex |= 64;
		}
		if(gridCubes[cb].vertices[7]->value < THRESHOLD) {
			cubeIndex |= 128;
		}

		/// This look up table tells which of the cube's edges intersect with the field's surface
		int usedEdges = edgeTable[cubeIndex];

		/// if the cube is entirely within/outside surface, no faces are produced so move to the next cube
		if(usedEdges == 0 || usedEdges == 255) {
			continue;
		}

		/// Interpolate vertex positions and normal vectors...
		for(int currentEdge = 0; currentEdge < 12; currentEdge++) {
			if(usedEdges && 1 << currentEdge) {
				grid_cube_vertex *v1 = gridCubes[cb].vertices[verticesAtEndsOfEdges[currentEdge * 2    ]];
				grid_cube_vertex *v2 = gridCubes[cb].vertices[verticesAtEndsOfEdges[currentEdge * 2 + 1]];

				float delta = (THRESHOLD - v1->value) / (v2->value - v1->value);

				edgeVertices[currentEdge].vertex.x = v1->position.x + delta * (v2->position.x - v1->position.x);
				edgeVertices[currentEdge].vertex.y = v1->position.y + delta * (v2->position.y - v1->position.y);
				edgeVertices[currentEdge].vertex.z = v1->position.z + delta * (v2->position.z - v1->position.z);

				edgeVertices[currentEdge].normal.x = v1->normal.x + delta * (v2->normal.x - v1->normal.x);
				edgeVertices[currentEdge].normal.y = v1->normal.y + delta * (v2->normal.y - v1->normal.y);
				edgeVertices[currentEdge].normal.z = v1->normal.z + delta * (v2->normal.z - v1->normal.z);
			}
		}

		for(int k = 0; triTable[cubeIndex][k] != 127 && nTriangles < MAX_TRIANGLES; k += 3) {
			geometry[nTriangles].vertex0.x = edgeVertices[triTable[cubeIndex][k  ]].vertex.x;
			geometry[nTriangles].vertex0.y = edgeVertices[triTable[cubeIndex][k  ]].vertex.y;
			geometry[nTriangles].vertex0.z = edgeVertices[triTable[cubeIndex][k  ]].vertex.z;

			geometry[nTriangles].normal0.x = edgeVertices[triTable[cubeIndex][k  ]].normal.x;
			geometry[nTriangles].normal0.y = edgeVertices[triTable[cubeIndex][k  ]].normal.y;
			geometry[nTriangles].normal0.z = edgeVertices[triTable[cubeIndex][k  ]].normal.z;

			math::Normalize(&geometry[nTriangles].normal0.x,
							&geometry[nTriangles].normal0.y,
							&geometry[nTriangles].normal0.z);
			/// ----------
			geometry[nTriangles].vertex1.x = edgeVertices[triTable[cubeIndex][k + 2]].vertex.x;
			geometry[nTriangles].vertex1.y = edgeVertices[triTable[cubeIndex][k + 2]].vertex.y;
			geometry[nTriangles].vertex1.z = edgeVertices[triTable[cubeIndex][k + 2]].vertex.z;

			geometry[nTriangles].normal1.x = edgeVertices[triTable[cubeIndex][k + 2]].normal.x;
			geometry[nTriangles].normal1.y = edgeVertices[triTable[cubeIndex][k + 2]].normal.y;
			geometry[nTriangles].normal1.z = edgeVertices[triTable[cubeIndex][k + 2]].normal.z;

			math::Normalize(&geometry[nTriangles].normal1.x,
							&geometry[nTriangles].normal1.y,
							&geometry[nTriangles].normal1.z);
			/// ----------
			geometry[nTriangles].vertex2.x = edgeVertices[triTable[cubeIndex][k + 1]].vertex.x;
			geometry[nTriangles].vertex2.y = edgeVertices[triTable[cubeIndex][k + 1]].vertex.y;
			geometry[nTriangles].vertex2.z = edgeVertices[triTable[cubeIndex][k + 1]].vertex.z;

			geometry[nTriangles].normal2.x = edgeVertices[triTable[cubeIndex][k + 1]].normal.x;
			geometry[nTriangles].normal2.y = edgeVertices[triTable[cubeIndex][k + 1]].normal.y;
			geometry[nTriangles].normal2.z = edgeVertices[triTable[cubeIndex][k + 1]].normal.z;

			math::Normalize(&geometry[nTriangles].normal2.x,
							&geometry[nTriangles].normal2.y,
							&geometry[nTriangles].normal2.z);
			nTriangles++;
		}
	}
}

int C_CubeGrid::Draw(C_Frustum *frustum)
{
	if(frustum != NULL) {
		if(frustum->cubeInFrustum(&bbox) == false) {
			return 0;
		}
	}

#ifdef FIXED_PIPELINE
	glPushMatrix();
	glTranslatef(position.x , position.y , position.z);
	glColor3f(1.0f , 1.0f , 1.0f);

	glBegin(GL_TRIANGLES);
	for(unsigned int i = 0 ; i < nTriangles ; i++) {
		glNormal3f(geometry[i].normal0.x , geometry[i].normal0.y , geometry[i].normal0.z);
		glVertex3f(geometry[i].vertex0.x , geometry[i].vertex0.y , geometry[i].vertex0.z);

		glNormal3f(geometry[i].normal1.x , geometry[i].normal1.y , geometry[i].normal1.z);
		glVertex3f(geometry[i].vertex1.x , geometry[i].vertex1.y , geometry[i].vertex1.z);

		glNormal3f(geometry[i].normal2.x , geometry[i].normal2.y , geometry[i].normal2.z);
		glVertex3f(geometry[i].vertex2.x , geometry[i].vertex2.y , geometry[i].vertex2.z);
	}
	glEnd();

	glPopMatrix();
#else
	shader->Begin();

	/// Pass matrices to shader
	/// Combine modelview and projection transformations
	ESMatrix mat;
	/// Transform
	esTranslate(&globalModelviewMatrix, position.x , position.y , position.z);
	/// Concatenate transforms
//	esMatrixMultiply(&mat, &globalModelviewMatrix, &globalProjectionMatrix);
//	shader->setUniformMatrix4fv("u_mvpMatrix", 1, GL_FALSE, &mat.m[0][0]);

	shader->setUniformMatrix4fv("u_modelviewMatrix", 1, GL_FALSE, (GLfloat *)&globalModelviewMatrix.m[0][0]);
	shader->setUniformMatrix4fv("u_projectionMatrix", 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

	/// Vertices
	glEnableVertexAttribArray(verticesAttribLocation);
	glVertexAttribPointer(verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), geometry);

	/// Normals
	glEnableVertexAttribArray(normalsAttribLocation);
	glVertexAttribPointer(normalsAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), (char *)geometry + 3 * sizeof(float));

	glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

	shader->End();
#endif

	return nTriangles;
}

void C_CubeGrid::DrawGridCube(void)
{
	/*
		glColor3f ( 1.0f , 1.0f , 1.0f );

		glDisable ( GL_LIGHTING );
			glPushMatrix ();
				glTranslatef ( CUBES_PER_AXIS * CUBE_SIZE / 2 , CUBES_PER_AXIS * CUBE_SIZE / 2 , CUBES_PER_AXIS * CUBE_SIZE / 2 );
				glutWireCube ( CUBES_PER_AXIS * CUBE_SIZE );
			glPopMatrix ();
		glEnable ( GL_LIGHTING );
	*/
	GLboolean culling;
	glGetBooleanv(GL_CULL_FACE , &culling);

	if(culling) {
		glDisable(GL_CULL_FACE);
	}

	//bbox.Draw();

	if(culling) {
		glEnable(GL_CULL_FACE);
	}
}
