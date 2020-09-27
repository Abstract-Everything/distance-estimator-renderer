#include "parser.hpp"

#include <iostream>

namespace renderer::preprocessor
{

Parser::Parser (
	std::filesystem::path const& include_search_path,
	std::filesystem::path const& path)
	: Parser (include_search_path, path, true)
{
}

Parser::Parser (
	std::filesystem::path const&                            include_search_path,
	std::filesystem::path const&                            p_path,
	bool                                                    is_implementation,
	std::vector<std::filesystem::path> const&               p_included_files,
	std::map<std::string, std::unique_ptr<Variable>> const& p_uniforms,
	std::map<std::string, std::vector<std::unique_ptr<Variable>>> const&
		p_struct_types)
	: path (p_path)
	, include_search_path (include_search_path)
	, included_files (p_included_files)
{
	for (auto const& [key, value] : p_uniforms)
		uniforms[key] = value->clone();

	for (auto const& [key, vector] : p_struct_types)
		for (std::unique_ptr<Variable> const& property : vector)
			struct_types[key].push_back (property->clone());

	if (!(path.has_filename() && path.has_extension()))
	{
		register_error ("Given path has no filename.");
		return;
	}

	process (path, is_implementation);
}

void Parser::process (
	std::filesystem::path const& p_path,
	bool                         is_implementation)
{
	Lexer lexer (p_path);
	if (!lexer.is_valid())
	{
		valid  = false;
		errors = lexer.get_errors();
		return;
	}

	std::vector<Token> tokens = lexer.get_tokens();
	end_iterator              = tokens.end();
	iterator                  = tokens.begin();

	while (iterator != tokens.end())
	{
		parse (is_implementation);
		iterator++;
	}

	if (is_vertex_shader (p_path))
		vertex_shader_code = glsl_shader_code;
	else if (is_fragment_shader (p_path))
		fragment_shader_code = glsl_shader_code;
}

void Parser::parse (bool is_implementation)
{
	if (iterator->type == Token_Type::Directive)
	{

		if (iterator->string.find ("#include") != std::string::npos)
			return include_file();

		else if (
			iterator->string.find ("#requires_implementation")
			!= std::string::npos)
		{
			if (iterator->line_number != 1)
				register_error ("#requires_implementation should be present in "
								"the first line");

			if (is_implementation)
				register_error (
					"This is not an implementation file, use a file "
					"which implements the required functions");
		}

		else if (iterator->string.find ("#vertex_shader") != std::string::npos)
		{
			if (vertex_shader_code.empty())
				include_vertex_shader();
			else
				register_error (
					"multiple vertex shaders are linked by this file.");
		}

		else
			glsl_shader_code += iterator->string;
	}

	else if (iterator->type == Token_Type::Keyword)
	{
		if (iterator->string.find ("struct") != std::string::npos)
			return register_struct();

		else if (iterator->string.find ("uniform") != std::string::npos)
			return register_uniform();

		else
			glsl_shader_code += iterator->string;
	}

	else if (iterator->type == Token_Type::String)
		glsl_shader_code += "\"" + iterator->string + "\"";

	else
		glsl_shader_code += iterator->string;
}

bool Parser::expect_token (
	Token_Type         expected_token,
	std::string const& error_message)
{
	glsl_shader_code += skip_whitespace (++iterator);
	if (iterator == end_iterator || iterator->type != expected_token)
	{
		register_error (error_message);
		return false;
	}
	return true;
}

bool Parser::peek_token (Token_Type check_token) const
{
	std::vector<Token>::const_iterator start_iterator = iterator + 1;
	skip_whitespace (start_iterator);
	return start_iterator->type == check_token;
}

std::vector<Token>::const_iterator
Parser::find_next_token (Token_Type const type) const
{
	std::vector<Token>::const_iterator search_iterator = iterator;
	while (search_iterator->type != type)
		search_iterator++;
	return search_iterator;
}

std::string
Parser::skip_whitespace (std::vector<Token>::const_iterator& p_iterator) const
{
	std::string space;
	while (p_iterator != end_iterator
		   && p_iterator->type == Token_Type::Whitespace)
		space += (p_iterator++)->string;
	return space;
}

std::unique_ptr<Variable> Parser::parse_variable()
{
	std::unique_ptr<Variable> variable
		= std::make_unique<Typed_Variable<bool>> (
			"Invalid",
			std::vector<bool>{false});

	if (!expect_token (Token_Type::Keyword, "Expected a type token."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return variable;
	}

	auto [type, size, type_name] = determine_variable_type();
	glsl_shader_code += type_name;

	if (!expect_token (Token_Type::Keyword, "Expected a variable name token."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return variable;
	}

	std::string variable_name = iterator->string;
	glsl_shader_code += variable_name;

	switch (type)
	{
	case Variable::Type::Boolean:
		variable = create_variable<bool> (variable_name, size);
		break;

	case Variable::Type::Integer:
		variable = create_variable<int> (variable_name, size);
		break;

	case Variable::Type::Uinteger:
		variable = create_variable<unsigned int> (variable_name, size);
		break;

	case Variable::Type::Float:
		variable = create_variable<float> (variable_name, size);
		break;

	case Variable::Type::Double:
		variable = create_variable<double> (variable_name, size);
		break;

	case Variable::Type::Struct:
		variable = std::make_unique<Struct_Variable> (
			variable_name,
			struct_types[type_name]);
		break;

	case Variable::Type::Invalid:
		assert (false && "Invalid variable type.");
		break;
	}

	expect_token (Token_Type::Semicolon, "Expected a semicolon.");
	glsl_shader_code += iterator->string;

	return variable;
}

template <typename T>
std::unique_ptr<Variable>
Parser::create_variable (std::string const& name, unsigned int size)
{
	std::vector<T> values = parse_assignment_values<T> (size);
	return std::make_unique<Typed_Variable<T>> (name, values);
}

template <typename T>
std::vector<T> Parser::parse_assignment_values (unsigned int size)
{
	std::vector<T> default_values (size);
	if (!peek_token (Token_Type::Equals))
		return default_values;

	else if (!expect_token (Token_Type::Equals, "Expected an equals sign."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return default_values;
	}

	bool should_be_brackated = (size != 1);
	if (should_be_brackated)
	{
		if (!expect_token (
				Token_Type::Open_Round_Brace,
				"Expected opening assignment value brace."))
		{
			iterator = find_next_token (Token_Type::Semicolon);
			return default_values;
		}
	}

	for (size_t i = 0; i < size; ++i)
	{
		default_values[i] = parse_value<T>();
		if (i + 1 < size)
		{
			if (!expect_token (
					Token_Type::Comma,
					"Expected a comma separating assignment values."))
			{
				iterator = find_next_token (Token_Type::Semicolon);
				return default_values;
			}
		}
	}

	if (should_be_brackated)
	{
		if (!expect_token (
				Token_Type::Close_Round_Brace,
				"Expected closing assignment value brace."))
		{
			iterator = find_next_token (Token_Type::Semicolon);
			return default_values;
		}
	}

	return default_values;
}

template <>
bool Parser::parse_value()
{
	if (!expect_token (Token_Type::Boolean, "Expected a boolean value."))
		return false;

	if (iterator->string == "true")
		return true;
	else if (iterator->string == "false")
		return false;
	register_error ("Boolean values should be true or false.");
	return false;
}

template <>
int Parser::parse_value()
{
	int negative = find_number (false);
	return negative * std::stoi (iterator->string);
}

template <>
unsigned int Parser::parse_value()
{
	find_number (false);
	return std::stoul (iterator->string);
}

template <>
float Parser::parse_value()
{
	int negative = find_number (true);
	return negative * std::stof (iterator->string);
}

template <>
double Parser::parse_value()
{
	int negative = find_number (true);
	return negative * std::stod (iterator->string);
}

int Parser::find_number (bool is_float)
{
	bool negative = peek_token (Token_Type::Minus);
	if (negative)
		expect_token (Token_Type::Minus, "");

	bool found_number
		= is_float
			  ? expect_token (
				  Token_Type::Float,
				  "Expected a floating point value.")
			  : expect_token (Token_Type::Integer, "Expected a integer value.");

	if (!found_number)
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return 1;
	}

	return negative ? -1 : 1;
}

void Parser::include_file()
{
	if (!expect_token (
			Token_Type::String,
			"#include should be followed by a file path."))
		return;

	std::filesystem::path include_path = include_search_path / iterator->string;
	if (!include_path.has_filename())
		register_error ("#include path has no filename.");

	if (std::find (included_files.begin(), included_files.end(), include_path)
		== included_files.end())
	{
		included_files.push_back (include_path);
		Parser file_parser (
			include_search_path,
			include_path,
			false,
			included_files,
			uniforms,
			struct_types);

		included_files = file_parser.included_files;
		uniforms       = std::move (file_parser.uniforms);
		struct_types   = std::move (file_parser.struct_types);

		if (file_parser.get_vertex_shader_code() != "")
		{
			if (vertex_shader_code != "")
				register_error ("Multiple vertex shaders linked by one file.");

			vertex_shader_code = file_parser.get_vertex_shader_code();
		}

		glsl_shader_code += file_parser.get_shader_code();
	}
}

void Parser::include_vertex_shader()
{
	if (!is_fragment_shader (path))
		register_error (
			"#vertex_shader should only be present in a fragment shader.");

	if (!expect_token (
			Token_Type::String,
			"#vertex_shader should be followed by a file path."))
		return;

	std::filesystem::path vertex_shader_path
		= include_search_path / iterator->string;
	if (!is_vertex_shader (vertex_shader_path))
		register_error (
			"#vertex_shader path does not point to a vertex shader.");

	Parser vertex_parser (include_search_path, vertex_shader_path, true);
	vertex_shader_code = vertex_parser.vertex_shader_code;
	uniforms.insert (
		std::make_move_iterator (vertex_parser.uniforms.begin()),
		std::make_move_iterator (vertex_parser.uniforms.end()));
	struct_types.insert (
		std::make_move_iterator (vertex_parser.struct_types.begin()),
		std::make_move_iterator (vertex_parser.struct_types.end()));
}

void Parser::register_struct()
{
	glsl_shader_code += iterator->string;
	if (!expect_token (
			Token_Type::Keyword,
			"Keyword struct should be followed by a name."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return;
	}

	std::string struct_name = iterator->string;
	glsl_shader_code += struct_name;

	if (!expect_token (
			Token_Type::Open_Curly_Brace,
			"Struct name should be followed by an open curly brace."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return;
	}
	glsl_shader_code += iterator->string;

	std::vector<std::unique_ptr<Variable>> properties;
	while (!peek_token (Token_Type::Close_Curly_Brace))
		properties.push_back (parse_variable());

	if (!expect_token (
			Token_Type::Close_Curly_Brace,
			"Expected a closing curly brace"))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return;
	}
	glsl_shader_code += iterator->string;

	if (!expect_token (Token_Type::Semicolon, "Expected a semicolon."))
	{
		iterator = find_next_token (Token_Type::Semicolon);
		return;
	}
	glsl_shader_code += iterator->string + "\n";

	if (struct_types.find (struct_name) == struct_types.end())
		struct_types[struct_name] = std::move (properties);
}

void Parser::register_uniform()
{
	glsl_shader_code += iterator->string;
	std::unique_ptr<Variable> variable = parse_variable();
	uniforms[variable->name]           = std::move (variable);
}

std::tuple<Variable::Type, unsigned int, std::string>
Parser::determine_variable_type()
{
	unsigned int   size = 1;
	std::string    type_name;
	Variable::Type type = Variable::Type::Invalid;

	if (iterator->string == "bool")
		type = Variable::Type::Boolean;

	else if (iterator->string == "int")
		type = Variable::Type::Integer;

	else if (iterator->string == "uint")
		type = Variable::Type::Uinteger;

	else if (iterator->string == "float")
		type = Variable::Type::Float;

	else if (iterator->string == "double")
		type = Variable::Type::Double;

	else if (iterator->string.find ("vec") != std::string::npos)
	{
		if (iterator->string.length() != 4 && iterator->string.length() != 5)
			register_error ("Vector type should be of the format TvecN where T "
							"is the type and N is the number of components.");

		std::string::const_iterator type_string = iterator->string.begin();
		if (*type_string == 'b')
			type = Variable::Type::Boolean;

		else if (*type_string == 'i')
			type = Variable::Type::Integer;

		else if (*type_string == 'u')
			type = Variable::Type::Uinteger;

		else if (*type_string == 'v')
			type = Variable::Type::Float;

		else if (*type_string == 'd')
			type = Variable::Type::Double;

		std::string::const_iterator size_string = iterator->string.end() - 1;
		size                                    = *size_string - '0';
	}

	else if (struct_types.find (iterator->string) != struct_types.end())
	{
		type      = Variable::Type::Struct;
		type_name = iterator->string;
	}

	if (type == Variable::Type::Invalid)
		register_error ("Type not recognized");

	if (type_name == "")
		type_name = iterator->string;

	return {type, size, type_name};
}

void Parser::register_error (std::string const& message)
{
	valid = false;
	errors.push_back (
		"\nError parsing" + path.string() + "at line: "
		+ std::to_string (iterator->line_number) + '\n' + message + '\n');
}

bool Parser::is_vertex_shader (std::filesystem::path const& p_path) const
{
	return p_path.has_extension()
		   && p_path.extension().string().find ("vert") != std::string::npos;
}

bool Parser::is_fragment_shader (std::filesystem::path const& p_path) const
{
	return p_path.has_extension()
		   && p_path.extension().string().find ("frag") != std::string::npos;
}

std::string Parser::get_shader_code() const
{
	return glsl_shader_code;
}

std::string Parser::get_vertex_shader_code() const
{
	return vertex_shader_code;
}

std::string Parser::get_fragment_shader_code() const
{
	return fragment_shader_code;
}

bool Parser::is_valid() const
{
	return valid;
}

std::vector<std::string> Parser::get_errors() const
{
	return errors;
}

std::vector<std::unique_ptr<Uniform>> Parser::get_uniforms() const
{
	std::vector<std::unique_ptr<Uniform>> uniform_variables;

	for (std::pair<const std::string, std::unique_ptr<Variable>> const&
			 uniform : uniforms)
	{
		std::vector<std::unique_ptr<Uniform>> properties
			= uniform.second->get_properties();

		for (std::unique_ptr<Uniform> const& property : properties)
			if (uniforms.find (property->get_name()) != uniforms.end())
				std::cerr << "Uniform " << property->get_name()
						  << " is present twice.";

		uniform_variables.insert (
			uniform_variables.end(),
			std::make_move_iterator (properties.begin()),
			std::make_move_iterator (properties.end()));
	}

	return uniform_variables;
}

} // namespace renderer::preprocessor
