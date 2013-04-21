/****************************************
*	 ***************************	    *
*		 Diplomatiki Ergasia:			*
*									    *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*									    *
*	 ***************************	    *
*									    *
*			  Syggrafeas:			    *
*									    *
*		  Apostolou Panagiotis			*
*									    *
*	 ***************************	    *
****************************************/

#include <EGL/egl.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "globals.h"

#include "camera.h"
#include "vectors.h"
#include "timer.h"
#include "glsl/glsl.h"
#include "metaballs/cubeGrid.h"
#include "metaballs/metaball.h"
#include "mmath.h"

static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *) glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

void DumpMatrix(ESMatrix *matrix)
{
	LOGI("\t\t%f %f %f %f", matrix->m[0][0], matrix->m[0][1], matrix->m[0][2], matrix->m[0][3]);
	LOGI("\t\t%f %f %f %f", matrix->m[1][0], matrix->m[1][1], matrix->m[1][2], matrix->m[1][3]);
	LOGI("\t\t%f %f %f %f", matrix->m[2][0], matrix->m[2][1], matrix->m[2][2], matrix->m[2][3]);
	LOGI("\t\t%f %f %f %f", matrix->m[3][0], matrix->m[3][1], matrix->m[3][2], matrix->m[3][3]);
}

using namespace std;

/// Globals
ESMatrix globalModelviewMatrix, globalProjectionMatrix;

/// Statics
//static C_Camera camera;
static C_Frustum frustum;
static int metaballPolys;
static float speed, angle, angle2;
static bool frustumCulling;

typedef struct saved_state_t_ {
	float speed, angle, angle2;
	uint32_t metaballPolys;
	bool frustumCulling;
	ESMatrix globalModelviewMatrix, globalProjectionMatrix;
	bool firstExecution;
} saved_state_t;


C_CubeGrid grid(-20.0f , 0.0f , -80.0f);
typedef struct engine_t_ {
	struct android_app* app;

//	ASensorManager* sensorManager;
//	const ASensor* accelerometerSensor;
//	ASensorEventQueue* sensorEventQueue;

	C_Camera *camera;
	C_Frustum frustum;

	C_Metaball metaball[3];

	bool animating;
	int32_t width, height;

	/// Egl shit
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;

	saved_state_t state;
} engine_t;

static void restoreState(saved_state_t *state)
{
	FUN_ENTRY

	globalProjectionMatrix = state->globalProjectionMatrix;
	globalModelviewMatrix = state->globalModelviewMatrix;

	speed = state->speed;
	angle = state->angle;
	angle2 = state->angle2;
	frustumCulling = state->frustumCulling;
}

static void saveState(saved_state_t *state)
{
	FUN_ENTRY

	state->globalProjectionMatrix = globalProjectionMatrix;
	state->globalModelviewMatrix = globalModelviewMatrix;

	state->speed = speed;
	state->angle = angle;
	state->angle2 = angle2;
	state->metaballPolys = metaballPolys;
	state->frustumCulling = frustumCulling;
}

static void printState(saved_state_t *state)
{
	LOGI("Current state:\n");
	LOGI("\tspeed: %f\n", speed);
	LOGI("\tangle: %f\n", angle);
	LOGI("\tangle2: %f\n", angle2);
	LOGI("\t: metaballPolys%d\n", metaballPolys);

	LOGI("\tglobalProjectionMatrix:\n");
	DumpMatrix(&globalProjectionMatrix);

	LOGI("\tglobalModelviewMatrix:\n");
	DumpMatrix(&globalModelviewMatrix);
}

/// Timer vars
#ifdef ENABLE_TIMING
static void CountFps(void);
C_Timer timer;
float start = timer.GetTime ();
static float timeElapsed = 0.0f;
#else
static float timeElapsed = 0.2f;
#endif

static void Initializations(engine_t *engine)
{
	FUN_ENTRY

	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

   	CheckGLSL();

   	if(engine->state.firstExecution) {
   		speed = 7.0f;
		angle = 0.5f;
		angle2 = 0.5f;
		metaballPolys = 0;
		frustumCulling = true;

		glViewport(0, 0, engine->width, engine->height);
   	} else {
		restoreState(&engine->state);
   	}

	engine->animating = true;

	/// Initialize GL state.
	glClearColor(0.3671875f , 0.15234375f , 0.8359375f , 1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	// Enose tin camera me to frustum kai dose times gia tin proboli
	engine->camera = new C_Camera();
	engine->camera->frustum = &frustum;
	engine->camera->fov = 50.0f;
	engine->camera->zFar = 200.0f;
	engine->camera->zNear = 1.0f;
   	engine->camera->setProjection(engine->width , engine->height);
	checkGlError("glViewport");

	/// metaballs initialization
	grid.Constructor();

	engine->metaball[0].Constructor();
	engine->metaball[0].position.x = 10.0f;
	engine->metaball[0].position.y = 10.0f;
	engine->metaball[0].position.z = 10.0f;
	engine->metaball[0].radius = 5.0f;

	engine->metaball[1].Constructor();
	engine->metaball[1].position.x = 10.0f;
	engine->metaball[1].position.y = 10.0f;
	engine->metaball[1].position.z = 10.0f;
	engine->metaball[1].radius = 8.0f;

	engine->metaball[2].Constructor();
	engine->metaball[2].position.x = 15.0f;
	engine->metaball[2].position.y = 15.0f;
	engine->metaball[2].position.z = 15.0f;
	engine->metaball[2].radius = 3.0f;

	/// timer initialization
	#ifdef ENABLE_TIMING
	timer.Initialize ();
	#endif
}

///Initialize an EGL context for the current display.
static int engine_init_display(engine_t *engine)
{
	FUN_ENTRY

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 8,
			EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	const EGLint attrib_list [] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGE("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	return 0;
}

static void Draw(engine_t *engine)
{
	/// Make the angle rotation independant of the cpu speed
	angle += .2f * timeElapsed;
	if(angle >= 360.0f) { angle = 0.0f; }

	angle2 += .05f * timeElapsed;
	if(angle2 >= 360.0f) { angle2 = 0.0f; }

	float cos_angle = cosf(angle);
	float cos_angle2 = cosf(angle2);

	/// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT);

	esMatrixLoadIdentity(&globalModelviewMatrix);
	engine->camera->Look();
	metaballPolys = 0;

	/// Draw metaballs
	engine->metaball[0].position.y = 20.0f + 5.0f * cos_angle2;
	engine->metaball[0].position.x = 20.0f + 10.0f * cos_angle2;

	engine->metaball[1].position.x = 20.0f + 8.0f * cos_angle;
	engine->metaball[1].position.z = 20.0f + 5.0f * cos_angle;

	engine->metaball[2].position.z = 15.0f + 10.0f * cos_angle;

	grid.Rotate(5.0f * timeElapsed, 0.0f, 0.0f);

	grid.Update(engine->metaball, 3, NULL);
	grid.Draw(NULL);

	eglSwapBuffers(engine->display, engine->surface);

	/// Update timer
	#ifdef ENABLE_TIMING
	timer.Update ();
	timeElapsed = timer.GetDelta () / 1000.0f;
	CountFps();
	#endif
}

#ifdef ENABLE_TIMING
static void CountFps(void)
{
	static int count = 0;
	float delta = timer.GetTime () - start;
	count++;

	if(delta >= 1000.0f) {
		LOGI("fps: %d\n", count);
		start = timer.GetTime ();
		count = 0;
	}
}
#endif

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_terminate_display(engine_t *engine)
{
	FUN_ENTRY

	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}

		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}

	engine->animating = false;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	static float old_x = 0.0f;
	static float d_x = 0.0f;
	static float old_y = 0.0f;
	static float d_y = 0.0f;

	engine_t * engine = (engine_t *)app->userData;

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		size_t pointerCount = AMotionEvent_getPointerCount(event);

		for (size_t i = 0; i < pointerCount; ++i) {
//			LOGI("Received motion event from pointer %zu: (%.2f, %.2f)", i, AMotionEvent_getX(event, i), AMotionEvent_getY(event, i));
			float x = AMotionEvent_getX(event, i);
			float y = AMotionEvent_getY(event, i);

			if (d_x != 0.0f && d_y != 0.0f) {
				d_x = x - old_x;
				d_y = y - old_y;
				float xx = 90.0f * d_x / (float)engine->width;
				float yy = 90.0f * d_y / (float)engine->height;

				LOGI("x: %3.1f y:%3.1f\n", xx, yy);
				engine->camera->Rotate(yy, xx);
			} else {
				d_x = old_x - x;
				d_y = old_y - y;
			}

			old_x = x;
			old_y = y;
		}

		return 1;
	} else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
//		LOGI("Received key event: %d", AKeyEvent_getKeyCode(event));
//		return 1;
	}

	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	FUN_ENTRY

	engine_t *engine = (engine_t *)app->userData;

	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			/// The system has asked us to save our current state.  Do so.
			LOGE("Saving state...\n");
			engine->app->savedState = malloc(sizeof(saved_state_t));
			if(!engine->app->savedState) {
				LOGE("Error saving state. Not enough memory.\n");
			}

			saveState(&engine->state);
			*((saved_state_t *)engine->app->savedState) = engine->state;
			engine->app->savedStateSize = sizeof(saved_state_t);
			break;

		case APP_CMD_INIT_WINDOW:
			LOGE("The window is being shown, get it ready.\n");
			if (engine->app->window != NULL) {
				engine_init_display(engine);
				Initializations(engine);
				engine->camera->setProjection(engine->width , engine->height);
				printState(&engine->state);
				Draw(engine);
			}
			break;

		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			engine_terminate_display(engine);
			break;

		case APP_CMD_GAINED_FOCUS:
			// When our app gains focus, we start monitoring the accelerometer.
//			if (engine->accelerometerSensor != NULL) {
//				ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
//				// We'd like to get 60 events per second (in us).
//				ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor, (1000L/60)*1000);
//			}
			break;

		case APP_CMD_LOST_FOCUS:
			// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
//			if (engine->accelerometerSensor != NULL) {
//				ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
//			}

			// Also stop animating.
			engine->animating = false;
			Draw(engine);
			break;
	}
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state)
{
	FUN_ENTRY

	engine_t engine;
	memset(&engine, 0, sizeof(engine_t));
//	engine.camera->xVec.SetVector(1.0f, 0.0f, 0.0f);

	// Make sure glue isn't stripped.
	app_dummy();

	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	/// Prepare to monitor accelerometer
//	engine.sensorManager = ASensorManager_getInstance();
//	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
//	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
//		LOGE("restoring...\n");
		/// We are starting with a previous saved state; restore from it.
		engine.state = *(saved_state_t *)state->savedState;
		engine.state.firstExecution = false;
	} else {
//		LOGE("1st run\n");
		engine.state.firstExecution = true;
	}

	/// loop waiting for stuff to do.
	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		/// If not animating, we will block forever waiting for events.
		/// If animating, we loop until all events are read, then continue
		/// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events, (void **)&source)) >= 0) {
			/// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			/// If a sensor has data, process it now.
//			if (ident == LOOPER_ID_USER) {
//				if (engine.accelerometerSensor != NULL) {
//					ASensorEvent event;
//
////					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,&event, 1) > 0) {
////						LOGI("accelerometer: x=%f y=%f z=%f",
////							  event.acceleration.x, event.acceleration.y,
////							  event.acceleration.z);
////					}
//				}
//			}

			/// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_terminate_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			Draw(&engine);
		}
	}
}
