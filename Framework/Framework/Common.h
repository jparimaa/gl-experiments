#pragma once

#include "Framework.h"
#include "Input.h"
#include "Model.h"
#include "imgui/imgui.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ostream>
#include <iterator>
#include <vector>

namespace fw
{

template <typename T>
inline int executeGenericMain()
{
	fw::Framework framework;
	bool success = framework.initialize();
	if (success) {
		T app;
		success = framework.setApplication(&app);
		if (success) {
			framework.execute();
		}
		framework.uninitialize();
	}

	if (!success) {
		std::cout << "Press any key to exit\n";
		std::cin.get();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

inline void toggleRelativeMouseMode(SDL_Keycode k = SDLK_LSHIFT)
{
	if (fw::Input::isKeyPressed(k)) {
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	if (fw::Input::isKeyReleased(k)) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}

inline void displayFps()
{
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

inline void displayVec3(const std::string& text, const glm::vec3& v)
{
	ImGui::Text(text.c_str(), v.x, v.y, v.z);
}

inline GLubyte getRandomColor()
{
	return static_cast<GLubyte>(std::rand() % 255);
}

inline bool hasExtension(const std::string& ext)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (GLint i = 0; i < numExtensions; ++i) {
		if (ext == std::string((char const*)glGetStringi(GL_EXTENSIONS, i))) {
			return true;
		}
	}
	return false;
}

inline void printExtensions(std::ostream& stream)
{
	GLint numExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (GLint i = 0; i < numExtensions; ++i) {
		stream << glGetStringi(GL_EXTENSIONS, i) << "\n";
	}
}

inline bool isShaderCompiled(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {		
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
		std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
		return false;
	}
	return true;
}

inline bool isProgramLinked(GLuint program)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::copy(infoLog.begin(), infoLog.end(), std::ostream_iterator<GLchar>(std::cerr, ""));
		return false;
	}
	return true;
}

inline void printSystemInfo()
{
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "GFX: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";
}

void inline mapBufferData(const Model& model, float* vertexData, unsigned int* elementData)
{
	unsigned int vertexIndex = 0;
	unsigned int elementIndex = 0;
	unsigned int indexOffset = 0;
	for (const auto& mesh : model.getMeshes()) {
		for (unsigned int i = 0; i < mesh.vertices.size(); ++i) {
			vertexData[vertexIndex++] = mesh.vertices[i].x;
			vertexData[vertexIndex++] = mesh.vertices[i].y;
			vertexData[vertexIndex++] = mesh.vertices[i].z;
			vertexData[vertexIndex++] = mesh.uvs[i].x;
			vertexData[vertexIndex++] = mesh.uvs[i].y;
			vertexData[vertexIndex++] = mesh.normals[i].x;
			vertexData[vertexIndex++] = mesh.normals[i].y;
			vertexData[vertexIndex++] = mesh.normals[i].z;
		}
		for (const auto& index : mesh.indices) {
			elementData[elementIndex++] = index + indexOffset;
		}
		indexOffset += mesh.vertices.size();
	}
}

} // fw
