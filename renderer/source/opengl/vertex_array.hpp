#pragma once

#include <GL/glew.h>

class Screen_Vertex_Array
{
public:
	Screen_Vertex_Array();
	~Screen_Vertex_Array();

private:
	GLuint vertex_buffer;
	GLuint vertex_array;
};