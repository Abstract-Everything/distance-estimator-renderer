#include "lexer.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>

namespace renderer::preprocessor
{

Lexer::Lexer (std::filesystem::path const& p_filepath) : filepath (p_filepath)
{
	auto file = io::load_file (filepath);
	if (!file.exists)
	{
		valid = false;
		errors.push_back (file.error);
		return;
	}

	std::string       line;
	std::stringstream file_stream (file.contents);
	while (std::getline (file_stream, line, '\n'))
	{
		tokenize (line);
		line_number++;
	}
}

bool Lexer::is_valid()
{
	return valid;
}

std::vector<std::string> Lexer::get_errors()
{
	return errors;
}

void Lexer::tokenize (std::string const& line)
{
	iterator     = line.begin();
	end_iterator = line.end();
	while (iterator != line.end())
	{
		switch (determine_character_type (iterator))
		{
		case Character_Type::Directive:
		{
			Token token (
				Token_Type::Directive,
				tokenize_directive(),
				line_number);
			tokens.push_back (token);
		}
		break;

		case Character_Type::Keyword:
		{
			Token_Type  type   = Token_Type::Keyword;
			std::string string = tokenize_block (
				{Character_Type::Keyword, Character_Type::Number});
			if (string.compare ("true") == 0 || string.compare ("false") == 0)
				type = Token_Type::Boolean;

			tokens.push_back (Token (type, string, line_number));
		}
		break;

		case Character_Type::Number:
		{
			std::string string = tokenize_number();
			Token_Type  type   = Token_Type::Integer;
			if (string.find ('.') != std::string::npos)
				type = Token_Type::Float;

			tokens.push_back (Token (type, string, line_number));
		}
		break;

		case Character_Type::Dot:
		case Character_Type::Punctuation:
		{
			std::string character = tokenize_punctuation();
			Token_Type  type      = Token_Type::Keyword;
			switch (character[0])
			{
			case '{': type = Token_Type::Open_Curly_Brace;  break;
			case '}': type = Token_Type::Close_Curly_Brace; break;
			case '(': type = Token_Type::Open_Round_Brace;  break;
			case ')': type = Token_Type::Close_Round_Brace; break;
			case ';': type = Token_Type::Semicolon;         break;
			case ',': type = Token_Type::Comma;             break;
			case '=': type = Token_Type::Equals;            break;
			case '-': type = Token_Type::Minus;             break;
			}
			tokens.push_back (Token (type, character, line_number));
		}
		break;

		case Character_Type::String:
		{
			std::string string = tokenize_string();
			tokens.push_back (Token (Token_Type::String, string, line_number));
		}
		break;

		case Character_Type::Whitespace:
		{
			std::string space = tokenize_block ({Character_Type::Whitespace});
			tokens.push_back (
				Token (Token_Type::Whitespace, space, line_number));
		}
		break;
		}
	}

	tokens.push_back (Token (Token_Type::Whitespace, "\n", line_number));
}

Character_Type Lexer::determine_character_type (
	std::string::const_iterator const& p_iterator) const
{
	if (*p_iterator == '#')
		return Character_Type::Directive;

	if (*p_iterator >= '0' && *p_iterator <= '9')
		return Character_Type::Number;

	if ((*p_iterator >= 'a' && *p_iterator <= 'z')
		|| (*p_iterator >= 'A' && *p_iterator <= 'Z') || *p_iterator == '_')
		return Character_Type::Keyword;

	if (*p_iterator == ' ' || *p_iterator == '\t' || *p_iterator == '\n')
		return Character_Type::Whitespace;

	if (*p_iterator == '.')
		return Character_Type::Dot;

	if (*p_iterator == '"')
		return Character_Type::String;

	return Character_Type::Punctuation;
}

std::string Lexer::tokenize_block (std::vector<Character_Type> const& types)
{
	auto check_end_character =
		[this, types] (std::string::const_iterator const& iterator) {
			const Character_Type type = determine_character_type (iterator);
			return std::find (types.begin(), types.end(), type) != types.end();
		};

	std::string::const_iterator start_iterator = iterator;
	while (iterator != end_iterator && check_end_character (iterator))
		++iterator;
	return std::string (start_iterator, iterator);
}

std::string Lexer::tokenize_directive()
{
	std::string value = std::string (iterator, iterator + 1);
	iterator++;
	value += tokenize_block ({Character_Type::Keyword});
	return value;
}

std::string Lexer::tokenize_punctuation()
{
	std::string value = std::string (iterator, iterator + 1);
	iterator++;
	return value;
}

std::string Lexer::tokenize_number()
{
	std::string number
		= tokenize_block ({Character_Type::Number, Character_Type::Dot});
	if (*iterator == 'f')
	{
		number += *iterator;
		iterator++;
	}
	return number;
}

std::string Lexer::tokenize_string()
{
	std::string::const_iterator start_iterator = iterator;
	if (*iterator != '"')
		assert (false && "String needs to start with a \"");

	do
	{
		iterator++;
	} while (iterator != end_iterator && *iterator != '"');

	if (*iterator != '"')
		register_error ("String does not have a matching end quote.");

	iterator++;
	return std::string (start_iterator + 1, iterator - 1);
}

std::vector<Token>& Lexer::get_tokens()
{
	return tokens;
}

void Lexer::register_error (std::string const& message)
{
	valid = false;
	errors.push_back (
		"Error parsing" + filepath.filename().string()
		+ "at line: " + std::to_string (line_number) + '\n' + message);
}

} // namespace renderer::preprocessor
