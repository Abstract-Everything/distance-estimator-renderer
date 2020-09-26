#pragma once

#include "file_loader.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace renderer::preprocessor
{

enum class Character_Type
{
	Directive,
	Keyword,
	Punctuation,
	Dot,
	String,
	Number,
	Whitespace
};

enum class Token_Type
{
	Directive,
	Keyword,
	Open_Round_Brace,
	Close_Round_Brace,
	Open_Curly_Brace,
	Close_Curly_Brace,
	Semicolon,
	Comma,
	Equals,
	Minus,
	Boolean,
	Integer,
	Float,
	String,
	Whitespace
};

struct Token
{
	Token_Type  type;
	std::string string;
	int         line_number;

	Token (Token_Type p_type, std::string p_string, int p_line_number)
		: type (p_type)
		, string (p_string)
		, line_number (p_line_number)
	{
	}
};

class Lexer
{
public:
	std::vector<Token>& get_tokens();
	Lexer (std::filesystem::path const& filepath);

	bool                     is_valid();
	std::vector<std::string> get_errors();

private:
	bool                        valid       = true;
	int                         line_number = 1;
	std::string::const_iterator iterator;
	std::string::const_iterator end_iterator;

	const std::filesystem::path filepath;
	std::vector<Token>          tokens;
	std::vector<std::string>    errors;

	void           tokenize (std::string const& line);
	Character_Type determine_character_type (
		std::string::const_iterator const& iterator) const;

	std::string tokenize_block (std::vector<Character_Type> const& types);
	std::string tokenize_directive();
	std::string tokenize_string();
	std::string tokenize_number();
	std::string tokenize_punctuation();

	void register_error (std::string const& message);
};

} // namespace renderer::preprocessor
