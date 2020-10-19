#include "gl_interface.hpp"

#include <algorithm>
#include <functional>
#include <map>

namespace
{
enum class Check_Error_Type
{
	Shader,
	Program
};

using IV      = std::function<void (GLuint, GLenum, GLint*)>;
using Infolog = std::function<void (GLuint, GLsizei, GLsizei*, GLchar*)>;

std::pair<bool, std::string>
check_error (GLuint check_location, GLenum status, IV iv, Infolog log)
{
	int success = 0, length = 0;

	iv (check_location, status, &success);
	if (success)
	{
		return {true, ""};
	}

	iv (check_location, GL_INFO_LOG_LENGTH, &length);
	length = std::max (1024, length);

	std::string info_log (length, ' ');
	log (check_location, length, &length, &info_log[0]);
	return {false, info_log};
};

std::pair<bool, std::string> check_shader_error (GLuint check_location)
{
	std::pair<bool, std::string> result = check_error (
		check_location,
		GL_COMPILE_STATUS,
		glGetShaderiv,
		glGetShaderInfoLog);
	const std::string& message = result.second;
	result.second = "Error shader compilation failed:\n" + message + '\n';
	return result;
}

std::pair<bool, std::string> check_linker_error (GLuint check_location)
{
	std::pair<bool, std::string> result = check_error (
		check_location,
		GL_LINK_STATUS,
		glGetProgramiv,
		glGetProgramInfoLog);
	const std::string& message = result.second;
	result.second = "Error linking shader program:\n" + message + '\n';
	return result;
}

bool check_errors (GLuint check_location, Check_Error_Type type)
{
	auto [success, message] = type == Check_Error_Type::Shader
								  ? check_shader_error (check_location)
								  : check_linker_error (check_location);

	if (!success)
	{
		std::cerr << message;
	}

	return success;
}

const bool show_low_severity = false;
void       opengl_debug (
		  GLenum source,
		  GLenum type,
		  GLuint,
		  GLenum severity,
		  GLsizei,
		  GLchar const* message,
		  void const*)
{
	const bool is_low_severity
		= (type == GL_DEBUG_TYPE_OTHER
		   || severity == GL_DEBUG_SEVERITY_NOTIFICATION);
	if (is_low_severity && !show_low_severity)
	{
		return;
	}

	std::string prefixes;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         prefixes += "[H] "; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       prefixes += "[M] "; break;
	case GL_DEBUG_SEVERITY_LOW:          prefixes += "[L] "; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: prefixes += "[N] "; break;
	}

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             prefixes += "[Api] "; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   prefixes += "[Win] "; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: prefixes += "[Sha] "; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     prefixes += "[TrP] "; break;
	case GL_DEBUG_SOURCE_APPLICATION:     prefixes += "[App] "; break;
	case GL_DEBUG_SOURCE_OTHER:           prefixes += "[Oth] "; break;
	}

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               prefixes += "[Err ] "; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: prefixes += "[Depr] "; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  prefixes += "[Udef] "; break;
	case GL_DEBUG_TYPE_PORTABILITY:         prefixes += "[Port] "; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         prefixes += "[Perf] "; break;
	case GL_DEBUG_TYPE_MARKER:              prefixes += "[Mark] "; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          prefixes += "[Push] "; break;
	case GL_DEBUG_TYPE_POP_GROUP:           prefixes += "[Pop ] "; break;
	case GL_DEBUG_TYPE_OTHER:               prefixes += "[Oth ] "; break;
	}

	std::cerr << "Message: " << prefixes + message << std::endl;
}
} // namespace

namespace renderer
{
namespace gl
{

void init()
{
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cerr << "Error initialising GLEW: " << glewGetErrorString (status)
				  << "\n";
		return;
	}

	GLint flags;
	glGetIntegerv (GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable (GL_DEBUG_OUTPUT);
		glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback (opengl_debug, nullptr);
		glDebugMessageControl (
			GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			nullptr,
			GL_TRUE);
	}
}

std::pair<bool, GLuint>
compile_shader (std::string const& shader_source, Shader_Type shader_type)
{
	const char* source = shader_source.c_str();
	GLuint      shader = glCreateShader (static_cast<GLuint> (shader_type));
	glShaderSource (shader, 1, &source, NULL);
	glCompileShader (shader);
	bool success = check_errors (shader, Check_Error_Type::Shader);
	return {success, shader};
}

std::pair<bool, GLuint> create_program (
	const std::string& vertex_shader_code,
	const std::string& fragment_shader_code)
{
	GLuint program_id = glCreateProgram();
	auto [vertex_success, vertex_shader]
		= compile_shader (vertex_shader_code, Shader_Type::Vertex);
	if (!vertex_success)
	{
		return {false, 0};
	}

	auto [fragment_success, fragment_shader]
		= compile_shader (fragment_shader_code, Shader_Type::Fragment);
	if (!fragment_success)
	{
		return {false, 0};
	}

	glAttachShader (program_id, vertex_shader);
	glAttachShader (program_id, fragment_shader);

	glLinkProgram (program_id);
	bool program_success = check_errors (program_id, Check_Error_Type::Program);

	for (auto const& shader : {vertex_shader, fragment_shader})
	{
		glDetachShader (program_id, shader);
		glDeleteShader (shader);
	}

	return {program_success, program_id};
}

void set_uniform_values (GLuint location, std::vector<int> const& values)
{
	switch (values.size())
	{
	case 1: glUniform1iv (location, 1, values.data()); break;
	case 2: glUniform2iv (location, 1, values.data()); break;
	case 3: glUniform3iv (location, 1, values.data()); break;
	case 4: glUniform4iv (location, 1, values.data()); break;
	}
}

void set_uniform_values (GLuint location, std::vector<bool> const& values)
{
	const std::vector<int> int_values (values.begin(), values.end());
	set_uniform_values (location, int_values);
}

void set_uniform_values (
	GLuint                           location,
	std::vector<unsigned int> const& values)
{
	switch (values.size())
	{
	case 1: glUniform1uiv (location, 1, values.data()); break;
	case 2: glUniform2uiv (location, 1, values.data()); break;
	case 3: glUniform3uiv (location, 1, values.data()); break;
	case 4: glUniform4uiv (location, 1, values.data()); break;
	}
}

void set_uniform_values (GLuint location, std::vector<float> const& values)
{
	switch (values.size())
	{
	case 1: glUniform1fv (location, 1, values.data()); break;
	case 2: glUniform2fv (location, 1, values.data()); break;
	case 3: glUniform3fv (location, 1, values.data()); break;
	case 4: glUniform4fv (location, 1, values.data()); break;
	}
}

void set_uniform_values (GLuint location, std::vector<double> const& values)
{
	switch (values.size())
	{
	case 1: glUniform1dv (location, 1, values.data()); break;
	case 2: glUniform2dv (location, 1, values.data()); break;
	case 3: glUniform3dv (location, 1, values.data()); break;
	case 4: glUniform4dv (location, 1, values.data()); break;
	}
}

template <typename T>
void extract_vector (GLuint program_id, renderer::Uniform const& uniform_base)
{
	using renderer::Typed_Uniform;

	const auto uniform = dynamic_cast<const Typed_Uniform<T>*> (&uniform_base);
	const std::string name = uniform->get_name();
	GLint location         = glGetUniformLocation (program_id, name.c_str());
	set_uniform_values (location, uniform->get_values());
}

void set_uniform (GLuint program_id, renderer::Uniform const& uniform)
{
	glUseProgram (program_id);

	if (typeid (uniform).hash_code()
		== typeid (Typed_Uniform<bool>).hash_code())
	{
		extract_vector<bool> (program_id, uniform);
	}
	else if (
		typeid (uniform).hash_code() == typeid (Typed_Uniform<int>).hash_code())
	{
		extract_vector<int> (program_id, uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (Typed_Uniform<unsigned int>).hash_code())
	{
		extract_vector<unsigned int> (program_id, uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (Typed_Uniform<float>).hash_code())
	{
		extract_vector<float> (program_id, uniform);
	}
	else if (
		typeid (uniform).hash_code()
		== typeid (Typed_Uniform<double>).hash_code())
	{
		extract_vector<double> (program_id, uniform);
	}
	else
	{
		assert (false && "Uniform is of an unsupported type");
	}

	glUseProgram (0);
}

} // namespace gl
} // namespace renderer
