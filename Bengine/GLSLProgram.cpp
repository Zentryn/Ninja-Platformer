#include "GLSLProgram.h"
#include "BengineErrors.h"
#include <fstream>
#include <vector>
#include "IOManager.h"

namespace Bengine {

GLSLProgram::GLSLProgram() : _numAttributes(0), _programID(0), _vertexShaderID(0), _fragmentShaderID(0)
{
}


GLSLProgram::~GLSLProgram()
{
}


void GLSLProgram::compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
    std::string vertSource;
    std::string fragSource;

    IOManager::readFileToBuffer(vertexShaderFilePath, vertSource);
    IOManager::readFileToBuffer(fragmentShaderFilePath, fragSource);

    compileShadersFromSource(vertSource.c_str(), fragSource.c_str());
}


void GLSLProgram::compileShadersFromSource(const char* vertexSource, const char* fragmentSource)
{
    // Get a program object
    _programID = glCreateProgram();

    _vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    if (_vertexShaderID == 0) {
        fatalError("Vertex shader failed to be created");
    }

    _fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    if (_fragmentShaderID == 0) {
        fatalError("Fragment shader failed to be created");
    }

    compileShader(vertexSource, "Vertex Shader", _vertexShaderID);
    compileShader(fragmentSource, "Fragment Shader", _fragmentShaderID);
}


void GLSLProgram::linkShaders()
{
	// Attach our shaders to our program
	glAttachShader(_programID, _vertexShaderID);
	glAttachShader(_programID, _fragmentShaderID);
     
	// Link our program
	glLinkProgram(_programID);

	// Check if the linking failed
	GLint isLinked = 0;
	glGetProgramiv(_programID, GL_LINK_STATUS, (int *)&isLinked);

	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(_programID, maxLength, &maxLength, &errorLog[0]);

		// We don't need to program anymore
		glDeleteProgram(_programID);

		std::printf("%s\n", &errorLog[0]);
		fatalError("Linking the shaders failed");
	}

	// Always detach the shaders after a successful link
	glDetachShader(_programID, _vertexShaderID);
	glDetachShader(_programID, _fragmentShaderID);

	// Don't leak the shaders either
	glDeleteShader(_vertexShaderID);
	glDeleteShader(_fragmentShaderID);
}


void GLSLProgram::addAttribute(const std::string& attributeName)
{
	glBindAttribLocation(_programID, _numAttributes++, attributeName.c_str());
}


void GLSLProgram::compileShader(const char* source, const std::string& name, GLuint id)
{
    // Tell OpenGL that we want to use source as the contents of the shader
	glShaderSource(id, 1, &source, nullptr);

    // Compile the shader
	glCompileShader(id);

    // Check for errors
	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint maxLenght = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLenght);

		// The maxLength includes the NULL character
		std::vector<char> errorLog(maxLenght);
		glGetShaderInfoLog(id, maxLenght, &maxLenght, &errorLog[0]);

		glDeleteShader(id);

		std::printf("%s\n", &errorLog[0]);
		fatalError("Shader " + name + " failed to compile");
	}
}


GLint GLSLProgram::getUniformLocation(const std::string& uniformName)
{
	GLint location = glGetUniformLocation(_programID, uniformName.c_str());

	if (location == GL_INVALID_INDEX) {
		fatalError("Uniform " + uniformName + " not found in the shader");
	}

	return location;
}


void GLSLProgram::use()
{
	glUseProgram(_programID);

	for (int i = 0; i < _numAttributes; i++) {
		glEnableVertexAttribArray(i);
	}
}


void GLSLProgram::unuse()
{
	glUseProgram(0);

	for (int i = 0; i < _numAttributes; i++) {
		glDisableVertexAttribArray(i);
	}
}

void GLSLProgram::dispose()
{
    if (_programID) glDeleteProgram(_programID);
    _numAttributes = 0;
}

}