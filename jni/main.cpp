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

#include "globals.h"

#include "camera.h"
#include "vectors.h"
#include "glsl/glsl.h"
#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"

#include "mmath.h"

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

using namespace std;

/// Global variables
ESMatrix globalModelviewMatrix, globalProjectionMatrix;

/// Camera and frustum
static C_Camera camera;
static C_Frustum frustum;

/// window stuff
static int winID;
static int windowWidth = 800;
static int windowHeight = 500;
static int windowPositionX = 100;
static int windowPositionY = 200;

/// movement vars
static float speed = 7.0f;
static float angle = 0.5f;
static float angle2 = 0.5f;

static int metaballPolys = 0;
static bool frustumCulling = true;

static C_Vector3 center(0.0f , 0.0f , 0.0f);

/// Timer vars
static float timeElapsed = 0.2f;
static float fps;

/// Metaballs
static C_CubeGrid grid;
static C_Metaball metaball[3];

// Sinartisi arhikpoiiseon
static void Initializations(GLint w , GLint h)
{
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);

   	CheckGLSL();

	/// Set clear color
	glClearColor(0.3671875f , 0.15234375f , 0.8359375f , 1.0f);

	/// Backface culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
//	glShadeModel(GL_SMOOTH);

	/// XXX: Disable normalizing ???
//	glDisable(GL_NORMALIZE);
//	glEnable(GL_LIGHTING);

	/// Lighting parameters
//	float ambient[] = { 1.0f , 1.0f , 1.0f , 1.0f };
//	float diffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
//	float specular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
//	glLightfv(GL_LIGHT0 , GL_AMBIENT , ambient);
//	glLightfv(GL_LIGHT0 , GL_DIFFUSE , diffuse);
//	glLightfv(GL_LIGHT0 , GL_SPECULAR , specular);
//
//	glEnable(GL_LIGHT0);

	// Enose tin camera me to frustum kai dose times gia tin proboli
	camera.frustum = &frustum;
	camera.fov = 50.0f;
	camera.zFar = 800.0f;
	camera.zNear = 1.0f;

	/// Load shaders
//	basicShader_texture = shaderManager.LoadShaderProgram("basic_withSingleTexture.vert" , "basic_withSingleTexture.frag");

	/// metaballs initialization
	grid.Constructor(-50.0f , 0.0f , -150.0f);

	metaball[0].Constructor();
	metaball[0].position.x = 10.0f;
	metaball[0].position.y = 10.0f;
	metaball[0].position.z = 10.0f;
	metaball[0].radius = 5.0f;

	metaball[1].Constructor();
	metaball[1].position.x = 10.0f;
	metaball[1].position.y = 10.0f;
	metaball[1].position.z = 10.0f;
	metaball[1].radius = 8.0f;

	metaball[2].Constructor();
	metaball[2].position.x = 15.0f;
	metaball[2].position.y = 15.0f;
	metaball[2].position.z = 15.0f;
	metaball[2].radius = 3.0f;

	windowWidth = w;
	windowHeight = h;
    glViewport(0, 0, w, h);
    checkGlError("glViewport");

	/// timer initialization
//	timer.Initialize ();
}

static void Draw(void)
{
	C_Vector3 cameraPosition = camera.GetPosition();

	/// Make the angle rotation independant of the cpu speed
	angle += .2f * timeElapsed;
	if(angle >= 360.0f) { angle = 0.0f; }

	angle2 += .05f * timeElapsed;
	if(angle2 >= 360.0f) { angle2 = 0.0f; }

	/// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	esMatrixLoadIdentity(&globalModelviewMatrix);
	camera.Look();
	metaballPolys = 0;

	/// Draw metaballs
	metaball[0].position.y = 20.0f + 5 * cosf(angle2);
	metaball[0].position.x = 20.0f + 10 * cosf(angle2);

	metaball[1].position.x = 20.0f + 8.0f * cosf(angle);
	metaball[1].position.z = 20.0f + 5.0f * cosf(angle);

	metaball[2].position.z = 15.0f + 10.0f * cosf(angle);


	grid.Update(metaball , 3 , NULL);
	grid.Draw(NULL);

	/// Print text on screen
//	int line = 1;
//	int lineHeight = 18;
//	camera.PrintText(0 , lineHeight * line++ ,
//					 1.0f , 1.0f , 0.0f , 0.6f ,
//					 "FPS: %d" , (int)fps);
//	camera.PrintText(0, lineHeight * line++,
//					 1.0f, 1.0f, 0.0f, 0.6f,
//					 "Metaball polys: %d" , metaballPolys);

	/// Update timer
//	timer.Update ();
//	timeElapsed = timer.GetDelta () / 1000.0f;

//	CountFPS ();

//	_DrawFrameBuffer();
//	glutSwapBuffers();
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_marchingcubes_MarchingCubesLib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_marchingcubes_MarchingCubesLib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_marchingcubes_MarchingCubesLib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
	Initializations(width, height);
}

JNIEXPORT void JNICALL Java_com_android_marchingcubes_MarchingCubesLib_step(JNIEnv * env, jobject obj)
{
	Draw();
}
