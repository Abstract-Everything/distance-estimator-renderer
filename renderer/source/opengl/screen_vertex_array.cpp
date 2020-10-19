#include "screen_vertex_array.hpp"

Screen_Vertex_Array::Screen_Vertex_Array()
{
	glGenVertexArrays (1, &vertex_array);
	glBindVertexArray (vertex_array);

	glGenBuffers (1, &vertex_buffer);
	glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData (
		GL_ARRAY_BUFFER,
		get_vertices_byte_size(),
		vertices.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray (0);
	glVertexAttribPointer (
		0,
		get_number_of_triangles(),
		GL_FLOAT,
		GL_FALSE,
		get_vertex_stride(),
		0);

	glBindVertexArray (0);
}

Screen_Vertex_Array::~Screen_Vertex_Array()
{
	glDeleteBuffers (1, &vertex_buffer);
	glDeleteVertexArrays (1, &vertex_array);
}

void Screen_Vertex_Array::render()
{
	glBindVertexArray (vertex_array);
	glDrawArrays (GL_TRIANGLES, 0, 6);
	glBindVertexArray (0);
}

GLint Screen_Vertex_Array::get_number_of_triangles() const
{
	const size_t floats_in_vertex     = 2;
	const size_t vertices_in_triangle = 3;
	const size_t floats_in_triangle   = floats_in_vertex * vertices_in_triangle;
	return static_cast<GLint> (vertices.size() / floats_in_triangle);
}

GLsizei Screen_Vertex_Array::get_vertices_byte_size() const
{
	return static_cast<GLsizei>(vertices.size() * sizeof (float));
}

GLsizei Screen_Vertex_Array::get_vertex_stride() const
{
	return sizeof (float) * 2;
}