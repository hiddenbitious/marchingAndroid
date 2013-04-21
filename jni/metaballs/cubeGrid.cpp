
#include "cubeGrid.h"
#include "tables.h"
#include "../mmath.h"
#include <pthread.h>

C_GLShaderManager C_CubeGrid::shaderManager;

#define GRIDCUBE(x,y,z)			gridCubes[((x) * CUBES_PER_AXIS + (y)) * CUBES_PER_AXIS + (z)]
#define GRIDCUBEVERTEX(x,y,z)	gridCubeVertices[((x) * VERTICES_PER_AXIS  + (y)) * VERTICES_PER_AXIS  + (z)]

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
"varying highp vec4 v_color;\n\
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

C_CubeGrid::C_CubeGrid(float x, float y, float z)
{
	geometry = new grid_triangle[MAX_TRIANGLES];
	if(geometry == NULL) {
		LOGE("%s:%s Cannot allocate memory for metaballs!\n", __FILE__, __FUNCTION__);
		return;
	}

	position.x = x;
	position.y = y;
	position.z = z;

	nGridCubes = CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS;
	nGridCubeVertices = (CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1) * (CUBES_PER_AXIS + 1);
	nTriangles = 0;

	/// Initialize vertices
	for(int x = 0 ; x < CUBES_PER_AXIS + 1 ; x++) {
		for(int y = 0 ; y < CUBES_PER_AXIS + 1 ; y++) {
			for(int z = 0 ; z < CUBES_PER_AXIS + 1 ; z++) {
				GRIDCUBEVERTEX(x,y,z).position.x = x * CUBE_SIZE;
				GRIDCUBEVERTEX(x,y,z).position.y = y * CUBE_SIZE;
				GRIDCUBEVERTEX(x,y,z).position.z = z * CUBE_SIZE;

				GRIDCUBEVERTEX(x,y,z).value = 0.0f;
			}
		}
	}

	/// Initialize cubes by setting the pointers to the appropriate cube vertices
	for(int x = 0 ; x < CUBES_PER_AXIS ; x++) {
		for(int y = 0 ; y < CUBES_PER_AXIS ; y++) {
			for(int z = 0 ; z < CUBES_PER_AXIS ; z++) {
				GRIDCUBE(x,y,z).vertices[0] = &GRIDCUBEVERTEX(x,y,z);
				GRIDCUBE(x,y,z).vertices[1] = &GRIDCUBEVERTEX(x,y,z+1);
				GRIDCUBE(x,y,z).vertices[2] = &GRIDCUBEVERTEX(x,y+1,z+1);
				GRIDCUBE(x,y,z).vertices[3] = &GRIDCUBEVERTEX(x,y+1,z);

				GRIDCUBE(x,y,z).vertices[4] = &GRIDCUBEVERTEX(x+1,y,z);
				GRIDCUBE(x,y,z).vertices[5] = &GRIDCUBEVERTEX(x+1,y,z+1);
				GRIDCUBE(x,y,z).vertices[6] = &GRIDCUBEVERTEX(x+1,y+1,z+1);
				GRIDCUBE(x,y,z).vertices[7] = &GRIDCUBEVERTEX(x+1,y+1,z);
			}
		}
	}

	bbox.SetMin(position.x , position.y , position.z);
	bbox.SetMax(position.x + CUBES_PER_AXIS * CUBE_SIZE ,
				position.y + CUBES_PER_AXIS * CUBE_SIZE ,
				position.z + CUBES_PER_AXIS * CUBE_SIZE);
	bbox.SetVertices();
}

C_CubeGrid::~C_CubeGrid()
{
	delete[] geometry;
}

void C_CubeGrid::Constructor()
{
	/// Initialize shader
	shader = shaderManager.LoadShaderProgram(vertexShaderSource, fragmentShaderSource);

	/// Get attribute locations
	verticesAttribLocation = shader->getAttribLocation("a_vertices");
	normalsAttribLocation = shader->getAttribLocation("a_normals");
}

//static grid_cube_t *cubesToInspect[CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS];

static inline int findCubeFieldIntersections(struct grid_cube *cube)
{
	int cubeIndex = 0;

	if(cube->vertices[0]->value < THRESHOLD) {
			cubeIndex |= 1;
		}
		if(cube->vertices[1]->value < THRESHOLD) {
			cubeIndex |= 2;
		}
		if(cube->vertices[2]->value < THRESHOLD) {
			cubeIndex |= 4;
		}
		if(cube->vertices[3]->value < THRESHOLD) {
			cubeIndex |= 8;
		}
		if(cube->vertices[4]->value < THRESHOLD) {
			cubeIndex |= 16;
		}
		if(cube->vertices[5]->value < THRESHOLD) {
			cubeIndex |= 32;
		}
		if(cube->vertices[6]->value < THRESHOLD) {
			cubeIndex |= 64;
		}
		if(cube->vertices[7]->value < THRESHOLD) {
			cubeIndex |= 128;
		}

	return cubeIndex;
}

typedef struct {
	C_Metaball *metaball;
	C_CubeGrid *grid;
} thread_data_t;

//pthread_mutex_t mutex;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *WorkerThread(void *ptr)
{
	thread_data_t *td = (thread_data_t *) ptr;
	C_Metaball *ball = td->metaball;
	C_CubeGrid *grid = td->grid;

	C_Vertex pos, ballToPoint;
	float rad = ball->radius;
	pos.x = ball->position.x; pos.y = ball->position.y; pos.z = ball->position.z;

	for(unsigned int cv = 0; cv < grid->nGridCubeVertices; cv++) {
		ballToPoint.x = grid->gridCubeVertices[cv].position.x - pos.x;
		ballToPoint.y = grid->gridCubeVertices[cv].position.y - pos.y;
		ballToPoint.z = grid->gridCubeVertices[cv].position.z - pos.z;

		float dist = ballToPoint.x * ballToPoint.x + ballToPoint.y * ballToPoint.y + ballToPoint.z * ballToPoint.z;

		if(dist < 0.0001f) {
			dist = 0.0001f;
		}

		//normal = (r^2 * v)/d^4
		float normalScale = rad / (dist * dist);

		pthread_mutex_lock(&mutex);
		grid->gridCubeVertices[cv].value += fieldFormula(rad, dist);
		grid->gridCubeVertices[cv].normal.x += ballToPoint.x * normalScale;
		grid->gridCubeVertices[cv].normal.y += ballToPoint.y * normalScale;
		grid->gridCubeVertices[cv].normal.z += ballToPoint.z * normalScale;
		pthread_mutex_unlock(&mutex);
	}

	pthread_exit(NULL);
}

void C_CubeGrid::Update(C_Metaball *metaballs , int nBalls , C_Frustum *frustum)
{
	pthread_t threads[3];
	thread_data_t thread_data[3];
//	pthread_mutex_init(&mutex, NULL);

	int x, y, z, cb, ret;
	float rad, dist, normalScale;
	unsigned int i;
	C_Vertex pos, ballToPoint;

	/// Initialize
	for(i = 0 ; i < nGridCubeVertices; i++) {
		gridCubeVertices[i].value = 0.0f;
		gridCubeVertices[i].normal.x = gridCubeVertices[i].normal.y = gridCubeVertices[i].normal.z = 0.0f;
	}

	/// Calculate field values and norms for each grid vertex
	for(cb = 0; cb < nBalls; cb++) {
		thread_data[cb].grid = this;
		thread_data[cb].metaball = &metaballs[cb];
		ret = pthread_create(&threads[cb], NULL, WorkerThread, (void *) &thread_data[cb]);
	}

	for(cb = 0; cb < nBalls; cb++) {
		pthread_join(threads[cb], NULL);
	}

	/// For each cube ...
	nTriangles = 0;
	for(unsigned int cb = 0; cb < nGridCubes && nTriangles < MAX_TRIANGLES; cb++) {
		int cubeIndex = findCubeFieldIntersections(&gridCubes[cb]);
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

//	LOGI("%s:%s: nTriangles: %d\n", __FILE__, __FUNCTION__, nTriangles);
//	LOGI("modelView matrix:\n");
//	for(int x = 0; x < 4; x++) {
//		for(int y = 0; y < 4; y++) {
//			LOGI("%f ", globalModelviewMatrix.m[x][y]);
//		}
//		LOGI("\n");
//	}
//
//	LOGI("projection matrix:\n");
//	for(int x = 0; x < 4; x++) {
//		for(int y = 0; y < 4; y++) {
//			LOGI("%f ", globalProjectionMatrix.m[x][y]);
//		}
//		LOGI("\n");
//	}

	shader->Begin();

	/// Transform
//	esTranslate(&globalModelviewMatrix, position.x , position.y , position.z);

//	ESMatrix ESrotMatrix;
//	rotationQuaternion.QuaternionToMatrix16(&ESrotMatrix);
//	esMatrixMultiply(&globalModelviewMatrix, &ESrotMatrix, &globalModelviewMatrix);


/// Concatenate transforms
//	ESMatrix mat;
//	esMatrixMultiply(&mat, &globalModelviewMatrix, &globalProjectionMatrix);
//	shader->setUniformMatrix4fv("u_mvpMatrix", 1, GL_FALSE, &mat.m[0][0]);

	/// Pass matrices to shader
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

void C_CubeGrid::Rotate(float anglex, float angley, float anglez)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	rotationQuaternion.Mult(&tempQuat);
}
