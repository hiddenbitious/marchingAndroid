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

#include "glsl.h"

#define JNI_COMPATIBLE

#ifndef JNI_COMPATIBLE
#	include <fstream>
#	include <assert.h>
#endif

bool extensions_init = false;
bool glslAvailable = false;

C_GLShaderObject::C_GLShaderObject(void)
{
	FUN_ENTRY

	type = NO_SHADER;
	shaderObject = 0;
	isCompiled = false;
	shaderSource = NULL;
	compilerLog = NULL;
}

C_GLShaderObject::~C_GLShaderObject(void)
{
	FUN_ENTRY

	if(shaderSource != NULL) {
		delete[] shaderSource;
	}

	if(compilerLog != NULL) {
		delete[] compilerLog;
	}

	glDeleteShader(shaderObject);
}

#ifndef JNI_COMPATIBLE
static unsigned long getFileLength(ifstream& file)
{
	if(!file.good()) {
		return 0;
	}

	file.seekg(0 , ios::end);
	unsigned long len = file.tellg();
	file.seekg(ios::beg);

	return len;
}
#endif

#ifndef JNI_COMPATIBLE
bool C_GLShaderObject::LoadShaderProgram(const char* filename)
{
	ifstream file;
	file.open(filename, ios::in);
	if(!file) {
		return false;
	}

	unsigned long len = getFileLength(file);

	// "Empty File"
	if(!len) {
		return false;
	}

	// there is already a source loaded, free it!
	if(shaderSource != NULL) {
		delete[] shaderSource;
	}

	// can't reserve memory
	shaderSource = (GLubyte*) new char[len + 1];
	if(!shaderSource) {
		return false;
	}

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value...
	shaderSource[len] = '\0';

	unsigned int i = 0;
	while(file.good()) {
		shaderSource[i++] = file.get();

		// coding guidelines...
		if(i > len) {
			i = len;
		}
	}

	// 0 terminate it. Also delete the last char (255)
	shaderSource[i - 1] = '\0';
//	this->fileName = filename;
	file.close();

	return true;
}
#else
bool C_GLShaderObject::LoadShaderProgram(const char* shaderSource)
{
	FUN_ENTRY

	int shaderSourceLength = strlen(shaderSource);

	if(!shaderSourceLength)
		return false;

	if(this->shaderSource) {
		delete[] this->shaderSource;
	}

	this->shaderSource = (GLubyte*) new char[shaderSourceLength + 1];
	if(!this->shaderSource) {
		return false;
	}

	memcpy((void *)this->shaderSource, (void *)shaderSource, shaderSourceLength);
	this->shaderSource[shaderSourceLength] = '\0';

	return true;
}
#endif
//-------------------------------------------------------------
bool C_GLShaderObject::compile(bool printSource)
{
	FUN_ENTRY

	if(!glslAvailable) {
		return false;
	}

	#ifndef JNI_COMPATIBLE
	if(printSource) {
		cout << endl << endl << shaderSource << endl << endl;
	}
	#endif

	LOGI("about to compile:\n");
	LOGI("%s\n", shaderSource);
	glShaderSource(shaderObject , 1 , (const GLchar **)&shaderSource , NULL);

	//Compile shader
	int success = 0;
	glCompileShader(shaderObject);
	glGetShaderiv(shaderObject , GL_COMPILE_STATUS , &success);

	//Get log size
	int logSize = 0;
	glGetShaderiv(shaderObject , GL_INFO_LOG_LENGTH , &logSize);
	compilerLog = new char[logSize];

	glGetShaderInfoLog(shaderObject, logSize, NULL, compilerLog);

	isCompiled = true;

	return (bool)!!success;
}

C_GLVertexShader::C_GLVertexShader(void)
{
	FUN_ENTRY

	type = VERTEX_SHADER;

	if(glslAvailable) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
}

C_GLVertexShader::~C_GLVertexShader(void)
{
	FUN_ENTRY

}

C_GLFragmentShader::C_GLFragmentShader(void)
{
	FUN_ENTRY

	type = FRAGMENT_SHADER;

	if(glslAvailable) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}
}

C_GLFragmentShader::~C_GLFragmentShader(void)
{
	FUN_ENTRY

}
//-------------------------------------------------------------

//-------------------------------------------------------------
C_GLShader::C_GLShader(void)
{
	FUN_ENTRY

	programObject = 0;
	isLinked = false;
	inUse = false;
	nShaders = 0;
	linkerLog = NULL;

	if(glslAvailable) {
		programObject = glCreateProgram();
	}
}
//-------------------------------------------------------------
C_GLShader::~C_GLShader(void)
{
	FUN_ENTRY

	if(glslAvailable) {
		for(unsigned int i = 0 ; i < nShaders ; i++) {
			glDetachShader(programObject , shaderList[i]->shaderObject);
			delete shaderList[i];
		}
		nShaders = 0;
	}

	glDeleteProgram(programObject);
}
//-------------------------------------------------------------

//-------------------------------------------------------------

void C_GLShader::AddShader(C_GLShaderObject* shader)
{
	FUN_ENTRY

	if(!glslAvailable) {
		return;
	}

	if(shader == NULL) {
		return;
	}

	if(!shader->isCompiled) {
		LOGI("Warning. Shader is not compiled. Attempting to compile now.\n");
		if(!shader->compile(false)) {
			return;
		}
	}

	shaderList[nShaders++] = shader;
}

bool C_GLShader::Link(void)
{
	FUN_ENTRY

	if(!glslAvailable || !nShaders) {
		return false;
	}

	int i;

	if(isLinked) {
		LOGI("Object program already linked. Attempting to link again.\n");

		for(i = 0 ; i < nShaders; i++) {
			glDetachShader(programObject , shaderList[i]->shaderObject);
		}
	}

	// Attach all the shaders
	for(i = 0 ; i < nShaders; i++) {
		glAttachShader(programObject , shaderList[i]->shaderObject);
	}

	// Link the shader programs
	glLinkProgram(programObject);

	int success;
	glGetProgramiv(programObject , GL_LINK_STATUS , &success);

	if(!success) {
		//Get log size
		int logSize = 0;
		glGetShaderiv(programObject , GL_INFO_LOG_LENGTH , &logSize);
		if(linkerLog) {
			delete[] linkerLog;
		}
		linkerLog = new char[logSize];

		glGetProgramInfoLog(programObject , logSize , NULL , linkerLog);
		return false;
	}

	isLinked = true;
	return true;
}

void C_GLShader::Begin(void)
{
	if(!glslAvailable)	{ return; }
	if(!programObject)	{ return; }
	if(!isLinked) { return; }
	if(inUse) { return; }

	inUse = true;

	glUseProgram(programObject);
}

void C_GLShader::End(void)
{
	if(!glslAvailable)	{ return; }
	if(!programObject)	{ return; }
	if(!inUse) { return; }

	inUse = false;

	glUseProgram(0);
}

GLenum C_GLShader::bindAttribLocation(const char *varname, const GLint position)
{
	if(!glslAvailable) {
		return false;
	}

//	GLint numActiveAttribs;
//	glGetProgramiv(programObject, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
//
//	cout << "~!~ " << numActiveAttribs << endl;

	GLenum glError;

	glBindAttribLocation(programObject, position, varname);
	glError = glGetError();
	if(glError != GL_NO_ERROR) {
		return glError;
	}

	/// Binding must be done before linking. Link again!
	if(!this->Link()) {
		LOGE("Failed to link shader.\n");
		return -1;
	}

	return GL_NO_ERROR;
}

GLint C_GLShader::getAttribLocation(const char *varname)
{
	if(!glslAvailable || !isLinked) {
		return -2;
	}

	GLint tmp = glGetAttribLocation(programObject, varname);
	return tmp;
}

void C_GLShader::printAttribInfo(GLint attrib)
{
	if(!glslAvailable || attrib < 0)
		return;

	char name[64];
	GLsizei length;
	GLint size;
	GLenum type;

	glGetActiveAttrib(programObject, attrib, 64, &length, &size, &type, name);

	LOGI("Attribute: %d info:\n", attrib);
	LOGI("\tName length: %d\n", length);
	LOGI("\tsize: %d\n", size);
	LOGI("\ttype: 0x%x\n", type);
	LOGI("\tname: %s\n", name);
}

bool C_GLShader::setUniform1f(char* varname, GLfloat v0)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1f(loc, v0);

	return true;
}

bool C_GLShader::setUniform2f(char* varname, GLfloat v0, GLfloat v1)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//   if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2f(loc, v0, v1);

	return true;
}

bool C_GLShader::setUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3f(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4f(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1i(char* varname, GLint v0)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1i(loc, v0);

	return true;
}

bool C_GLShader::setUniform2i(char* varname, GLint v0, GLint v1)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2i(loc, v0, v1);


	return true;
}

bool C_GLShader::setUniform3i(char* varname, GLint v0, GLint v1, GLint v2)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3i(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4i(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1fv(char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2fv(char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3fv(char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4fv(char* varname, GLsizei count, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform1iv(char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2iv(char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3iv(char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4iv(char* varname, GLsizei count, GLint *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix2fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix3fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix4fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	if(!glslAvailable) { return false; }  // GLSL not available
//    if (!_noshader) return true;

	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix4fv(loc, count, transpose, value);

	return true;
}

GLint C_GLShader::GetUniLoc(const GLchar *name)
{
	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}
//    CHECK_GL_ERROR();
	return loc;
}

void C_GLShader::GetUniformfv(char* name, GLfloat* values)
{
	if(!glslAvailable) { return; }
	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}

	glGetUniformfv(programObject, loc, values);
}

void C_GLShader::GetUniformiv(char* name, GLint* values)
{
	if(!glslAvailable) { return; }


	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}

	glGetUniformiv(programObject, loc, values);
}

C_GLShaderManager::C_GLShaderManager(void)
{
	for(int i = 0; i < MAX_PROGRAMS; i++)
		programList[i] = NULL;
	nPrograms = 0;
}

void C_GLShaderManager::CleanUp()
{
	for(int i = 0; i < nPrograms; i++) {
		delete programList[i];
		programList[i] = NULL;
	}
	nPrograms = 0;
}

C_GLShaderManager::~C_GLShaderManager(void)
{
	CleanUp();
}

#ifndef JNI_COMPATIBLE
C_GLShader* C_GLShaderManager::LoadShaderProgram(const char *vertexFile , const char *fragmentFile)
{
	C_GLShader* shaderObject;
	C_GLVertexShader* tVertexShader;
	C_GLFragmentShader* tFragmentShader;

	cout << "\n********************************************************************************\n\n";
	cout << "Loading shader . . ." << endl;


	// Load vertex shader
	tVertexShader = new C_GLVertexShader();
	shaderObject = new C_GLShader();
//	cout << "----------------------------------------------------------" << endl;
	cout << "Loading vertex shader from file:   \"" << vertexFile << "\" ... ";
	if(!tVertexShader->LoadShaderProgram(vertexFile)) {
		cout << "\nCan't load vertex shader file: \"" << vertexFile << "\"." << endl;
		delete tVertexShader;
		return shaderObject;
	}
	cout << "done!" << endl;

	// Load fragment shader
	tFragmentShader = new C_GLFragmentShader();
//	cout << "----------------------------------------------------------" << endl;
	cout << "Loading fragment shader from file: \"" << fragmentFile << "\" ... ";
	if(!tFragmentShader->LoadShaderProgram(fragmentFile)) {
		cout << "\nCan't load fragment shader file: " << fragmentFile << "." << endl;
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	cout << "done!" << endl;

	// Compile vertex shader
//	cout << "----------------------------------------------------------" << endl;
	cout << "Compiling vertex shader...";
	if(!tVertexShader->compile(true)) {
		cout << "Error compiling vertex shader in file: " << vertexFile << "\n\n";
		cout << "Compiler log: " << endl;
		cout << tVertexShader->compilerLog << endl;
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	cout << "...done!" << endl;
	if(strlen(tVertexShader->compilerLog)) {
		cout << "Compiler log: " << endl;
		cout << tVertexShader->compilerLog << endl;
	}

	// Compile fragment shader
//	cout << "----------------------------------------------------------" << endl;
	cout << "Compiling fragment shader...";
	if(!tFragmentShader->compile(true)) {
		cout << "Error compiling fragment shader in file: " << fragmentFile << "\n\n";
		cout << "Compiler log: " << endl;
		cout << tFragmentShader->compilerLog << endl;
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	cout << "...done!" << endl;
	if(strlen(tFragmentShader->compilerLog)) {
		cout << "Compiler log: " << endl;
		cout << tFragmentShader->compilerLog << endl;
	}

	// Add shaders to shader object
	shaderObject->AddShader(tVertexShader);
	shaderObject->AddShader(tFragmentShader);

	// Link shader object
//	cout << "----------------------------------------------------------" << endl;
	cout << "Linking shaders into shader object...";
	if(!shaderObject->Link()) {
		cout << "\nError linking shader programs." << endl;
		cout << shaderObject->linkerLog << "\n\n";
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	cout << "done!" << endl;
	cout << "\n********************************************************************************\n\n";
//	shaderList.push_back(shaderObject);
	if(!shaderList)
		delete shaderList;
	shaderList = shaderObject;


	return shaderObject;
}
#else
C_GLShader* C_GLShaderManager::LoadShaderProgram(const char *vertexSource , const char *fragmentSource)
{
	C_GLShader* shaderObject;
	C_GLVertexShader* tVertexShader;
	C_GLFragmentShader* tFragmentShader;

	CleanUp();

	LOGI("\n***********************************************\n\n");
	LOGI("Loading shader . . .\n");

	/// Load vertex shader
	tVertexShader = new C_GLVertexShader();
	shaderObject = new C_GLShader();

	LOGI("Loading vertex shader...\n");
	if(!tVertexShader->LoadShaderProgram(vertexSource)) {
		LOGE("\nCan't load vertex shader \n");
		delete tVertexShader;
		return shaderObject;
	}
	LOGI("done!\n");

	/// Load fragment shader
	tFragmentShader = new C_GLFragmentShader();

	LOGI("Loading fragment shader\n");
	if(!tFragmentShader->LoadShaderProgram(fragmentSource)) {
		LOGE("\nCan't load fragment shader\n");
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("done!\n");

	/// Compile vertex shader
	LOGI("Compiling vertex shader...\n");
	if(!tVertexShader->compile(true)) {
		LOGE("Error compiling vertex shader\n");
		LOGE("Compiler log:\n");
		if(tVertexShader->compilerLog)
			LOGE("%s\n", tVertexShader->compilerLog);
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("...done!\n");
//	if(tVertexShader->compilerLog) {
//		LOGI("Compiler log: %s\n", tVertexShader->compilerLog);
//		delete[] tVertexShader->compilerLog;
//		tVertexShader->compilerLog = NULL;
//	}

	// Compile fragment shader
	LOGI("Compiling fragment shader...\n");
	if(!tFragmentShader->compile(true)) {
		LOGE("Error compiling fragment shader\n");
		LOGE("Compiler log:\n");
		if(tFragmentShader->compilerLog)
			LOGE("%s\n", tFragmentShader->compilerLog);
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	LOGI("...done!\n");
//	if(tFragmentShader->compilerLog) {
//		LOGI("Compiler log: %s\n", tFragmentShader->compilerLog);
//		delete[] tFragmentShader->compilerLog;
//		tFragmentShader->compilerLog = NULL;
//	}

	/// Add shaders to shader object
	shaderObject->AddShader(tVertexShader);
	shaderObject->AddShader(tFragmentShader);

	/// Link shader object
	LOGI("Linking shader objects...");
	if(!shaderObject->Link()) {
		LOGE("\nError linking shader programs.\n");
		LOGE("Linker log:\n%s\n", shaderObject->linkerLog);
		delete tVertexShader;
		delete tFragmentShader;
		return shaderObject;
	}
	if(shaderObject->linkerLog) {
		LOGI("Linker log:\n%s\n", shaderObject->linkerLog);
		delete[] shaderObject->linkerLog;
		shaderObject->linkerLog = NULL;
	}

	LOGI("done!\n");
	LOGI("\n***********************************************\n\n");


	programList[nPrograms++] = shaderObject;

	return shaderObject;
}
#endif

#ifndef JNI_COMPATIBLE
bool InitGLExtensions(void)
{
	if(extensions_init) {
		return true;
	}

	//Init GLEW
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		//Problem: glewInit failed, something is seriously wrong.
		cout << "Error initizing GLEW." << endl << "Error: " << glewGetErrorString(err) << endl;
		extensions_init = false;
		return false;
	} else {
		cout << "GLEW initialized" << endl;
	}

	const GLubyte* oglVendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* oglVersion = glGetString(GL_VERSION);

	cout << "OpenGL Version: " << oglVersion << endl;
	cout << "OpenGL Vendor: " << oglVendor << endl;
	cout << "Renderer: " << renderer << endl;

	return true;
}

bool CheckGLSL(void)
{
	if(glslAvailable) {
		return true;
	}

	glslAvailable = true;

	cout << "\n\n";

	if(GLEW_VERSION_4_1) {
		cout << "OpenGL 4.1 is available!" << endl;
	}else if(GLEW_VERSION_4_0) {
		cout << "OpenGL 4.0 is available!" << endl;
	} else if(GLEW_VERSION_3_3) {
		cout << "OpenGL 3.3 is available!" << endl;
	} else if(GLEW_VERSION_3_2) {
		cout << "OpenGL 3.2 is available!" << endl;
	} else if(GLEW_VERSION_3_1) {
		cout << "OpenGL 3.1 is available!" << endl;
	} else if(GLEW_VERSION_3_0) {
		cout << "OpenGL 3.0 is available!" << endl;
	} else if(GLEW_VERSION_2_1) {
		cout << "OpenGL 2.1 is available!" << endl;
	} else if(GLEW_VERSION_2_0) {
		cout << "OpenGL 2.0 is available!" << endl;
	} else if(GLEW_VERSION_1_5) {
		cout << "OpenGL 1.5 core functions are available" << endl;
	} else if(GLEW_VERSION_1_4) {
		cout << "OpenGL 1.4 core functions are available" << endl;
	} else if(GLEW_VERSION_1_3) {
		cout << "OpenGL 1.3 core functions are available" << endl;
	} else if(GLEW_VERSION_1_2) {
		cout << "OpenGL 1.2 core functions are available" << endl;
	}

	if(GL_TRUE != glewGetExtension("GL_ARB_fragment_shader")) {
		cout << "GL_ARB_fragment_shader extension is not available!";
		glslAvailable = false;
	} else {
		cout << "GL_ARB_fragment_shader extension is available!" << endl;
	}

	if(GL_TRUE != glewGetExtension("GL_ARB_vertex_shader")) {
		cout << "GL_ARB_vertex_shader extension is not available!" << endl;
		glslAvailable = false;
	} else {
		cout << "GL_ARB_vertex_shader extension is available!" << endl;
	}

	if(GL_TRUE != glewGetExtension("GL_ARB_shader_objects")) {
		cout << "GL_ARB_shader_objects extension is not available!" << endl;
		glslAvailable = false;
	} else {
		cout << "GL_ARB_shader_objects extension is available!" << endl;
	}

	if(glslAvailable) {
		cout << "OpenGL Shading Language is available!\n\n";
	} else {
		cout << "OpenGL Shading Language is not available :(\n\n";
	}

	return glslAvailable;
}
#else
bool CheckGLSL(void)
{
	glslAvailable = true;
	return true;
}
#endif
