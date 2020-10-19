#pragma once

#include <GL/glew.h>

#include <array>

class Screen_Vertex_Array
{
public:
	Screen_Vertex_Array();
	~Screen_Vertex_Array();

	void render();

private:
	const std::array<float, 12> vertices{
		 1,  1,
		 1, -1,
		-1, -1,
		 1,  1,
		-1, -1,
		-1,  1
	};

	GLuint vertex_buffer = 0;
	GLuint vertex_array = 0;

	GLint   get_number_of_triangles() const;
	GLsizei get_vertices_byte_size() const;
	GLsizei get_vertex_stride() const;
};