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
		shaderSource = NULL;
	}

	if(compilerLog != NULL) {
		delete[] compilerLog;
		compilerLog = NULL;
	}

	glDeleteShader(shaderObject);
}

bool C_GLShaderObject::LoadShaderProgram(const char* shaderSource)
{
	FUN_ENTRY

	int shaderSourceLength = strlen(shaderSource);

	if(!shaderSourceLength)
		return false;

	if(this->shaderSource) {
		delete[] this->shaderSource;
		this->shaderSource = NULL;
	}

	this->shaderSource = (GLubyte*) new char[shaderSourceLength + 1];
	if(!this->shaderSource) {
		LOGE("%s: Not enough memory.\n", __FUNCTION__);
		return false;
	}

	memcpy((void *)this->shaderSource, (void *)shaderSource, shaderSourceLength);
	this->shaderSource[shaderSourceLength] = '\0';

	return true;
}

//-------------------------------------------------------------
bool C_GLShaderObject::compile(bool printSource)
{
	FUN_ENTRY

	if(!shaderSource)
		return false;

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
	if(logSize) {
		compilerLog = new char[logSize];
		glGetShaderInfoLog(shaderObject, logSize, NULL, compilerLog);
	}

	isCompiled = true;

	return (bool)success;
}

C_GLVertexShader::C_GLVertexShader(void)
{
	FUN_ENTRY

	type = VERTEX_SHADER;
	shaderObject = glCreateShader(GL_VERTEX_SHADER);
}

C_GLVertexShader::~C_GLVertexShader(void)
{
	FUN_ENTRY
}

C_GLFragmentShader::C_GLFragmentShader(void)
{
	FUN_ENTRY

	type = FRAGMENT_SHADER;
	shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
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

	programObject = glCreateProgram();
}

C_GLShader::~C_GLShader(void)
{
	FUN_ENTRY

	for(unsigned int i = 0 ; i < nShaders ; i++) {
		glDetachShader(programObject, shaderList[i]->shaderObject);
		delete shaderList[i];
	}
	nShaders = 0;

	glDeleteProgram(programObject);
}

void C_GLShader::AddShader(C_GLShaderObject* shader)
{
	FUN_ENTRY

	if(shader == NULL) {
		return;
	}

	if(!shader->isCompiled) {
		LOGI("Warning. Shader is not compiled. Attempting to compile now.\n");
		if(!shader->compile(false)) {
			return;
		}
	}

	shaderList[nShaders] = shader;
	nShaders++;
}

bool C_GLShader::Link(void)
{
	FUN_ENTRY

	if(isLinked) {
		LOGI("Object program already linked. Attempting to link again.\n");

		for(int i = 0 ; i < nShaders; i++) {
			glDetachShader(programObject , shaderList[i]->shaderObject);
		}
	}

	/// Attach all shaders again
	for(int i = 0 ; i < nShaders; i++) {
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

		if(logSize) {
			linkerLog = new char[logSize];
			glGetProgramInfoLog(programObject , logSize , NULL , linkerLog);
		}

		return false;
	}

	isLinked = true;
	return true;
}

void C_GLShader::Begin(void)
{
	if(!programObject || !isLinked || inUse) {
		return;
	}

	glUseProgram(programObject);
	inUse = true;
}

void C_GLShader::End(void)
{
	if(!inUse) {
		return;
	}

	glUseProgram(0);
	inUse = false;
}

GLenum C_GLShader::bindAttribLocation(const char *varname, const GLint position)
{
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
	if(!isLinked) {
		return -2;
	}

	return glGetAttribLocation(programObject, varname);
}

void C_GLShader::printAttribInfo(GLint attrib)
{
	if(attrib < 0)
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
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1f(loc, v0);

	return true;
}

bool C_GLShader::setUniform2f(char* varname, GLfloat v0, GLfloat v1)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2f(loc, v0, v1);

	return true;
}

bool C_GLShader::setUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3f(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4f(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1i(char* varname, GLint v0)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1i(loc, v0);

	return true;
}

bool C_GLShader::setUniform2i(char* varname, GLint v0, GLint v1)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2i(loc, v0, v1);


	return true;
}

bool C_GLShader::setUniform3i(char* varname, GLint v0, GLint v1, GLint v2)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3i(loc, v0, v1, v2);

	return true;
}

bool C_GLShader::setUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4i(loc, v0, v1, v2, v3);

	return true;
}

bool C_GLShader::setUniform1fv(char* varname, GLsizei count, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2fv(char* varname, GLsizei count, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3fv(char* varname, GLsizei count, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4fv(char* varname, GLsizei count, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4fv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform1iv(char* varname, GLsizei count, GLint *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform1iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform2iv(char* varname, GLsizei count, GLint *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform2iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform3iv(char* varname, GLsizei count, GLint *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform3iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniform4iv(char* varname, GLsizei count, GLint *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniform4iv(loc, count, value);

	return true;
}

bool C_GLShader::setUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix2fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
	GLint loc = GetUniLoc(varname);
	if(loc == -1) { return false; } // can't find variable

	glUniformMatrix3fv(loc, count, transpose, value);

	return true;
}

bool C_GLShader::setUniformMatrix4fv(const char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
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
	GLint loc;

	loc = glGetUniformLocation(programObject, name);
	if(loc == -1) {
		LOGE("Error: can't find uniform variable \"%s\"\n", name);
	}

	glGetUniformfv(programObject, loc, values);
}

void C_GLShader::GetUniformiv(char* name, GLint* values)
{
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

C_GLShader *C_GLShaderManager::LoadShaderProgram(const char *vertexSource , const char *fragmentSource)
{
	C_GLShader *shaderObject;
	C_GLVertexShader *tVertexShader;
	C_GLFragmentShader *tFragmentShader;

	LOGI("\n***********************************************\n\n");
	LOGI("Loading shader . . .\n");

	/// Load vertex shader
	tVertexShader = new C_GLVertexShader();
	shaderObject = new C_GLShader();
	if(!tVertexShader || !shaderObject) {
		LOGE("%s: Not enough memory.\n", __FUNCTION__);
		return NULL;
	}

	LOGI("Loading vertex shader...\n");
	if(!tVertexShader->LoadShaderProgram(vertexSource)) {
		LOGE("\nCan't load vertex shader \n");
		delete tVertexShader;
		return shaderObject;
	}
	LOGI("done!\n");

	/// Load fragment shader
	tFragmentShader = new C_GLFragmentShader();
	if(!tFragmentShader) {
		LOGE("%s: Not enough memory.\n", __FUNCTION__);
		return NULL;
	}

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

bool CheckGLSL(void)
{
	glslAvailable = true;
	return true;
}
