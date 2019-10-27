#pragma once

#include "uniform.hpp"
#include "lexer.hpp"

#include <cassert>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

namespace renderer::preprocessor
{

struct Variable
{
  enum class Type { Invalid, Boolean, Integer, Uinteger, Float, Double, Struct };
  std::string name;

  virtual ~Variable () {};
  virtual std::unique_ptr <Variable> clone () const = 0;
  virtual std::vector <std::unique_ptr <Uniform>> get_properties (std::string const& prefix = "") const = 0;
};

template <typename T>
struct Typed_Variable : public Variable
{
  std::vector<T> values {};

  Typed_Variable (std::string const &p_name, std::vector <T> const &p_values)
  {
    name = p_name;
    values = p_values;
  }

  std::unique_ptr <Variable> clone () const override
  {
    return std::make_unique <Typed_Variable<T>> (name, values);
  }

  std::vector <std::unique_ptr <Uniform>> get_properties (std::string const& prefix = "") const override
  {
    std::string uniform_name = prefix.empty() ? name : prefix + '.' + name;
    std::vector <std::unique_ptr <Uniform>> uniforms (1);
    uniforms[0] = std::make_unique <Typed_Uniform<T>> (uniform_name, values);
    return uniforms;
  }
};

struct Struct_Variable : public Variable
{
  std::vector <std::unique_ptr <Variable>> properties {};

  Struct_Variable (std::string const &p_name, std::vector <std::unique_ptr <Variable>> const & p_properties)
  {
    this->name = p_name;
    for (std::unique_ptr <Variable> const &property : p_properties)
      properties.push_back (property->clone());
  }

  std::unique_ptr <Variable> clone () const override
  {
    return std::make_unique <Struct_Variable> (name, properties);
  }

  std::vector <std::unique_ptr <Uniform>> get_properties (std::string const& prefix = "") const override
  {
    std::vector <std::unique_ptr <Uniform>> uniforms;
    std::string uniform_prefix = prefix.empty() ? name : prefix + '.' + name;

    for (std::unique_ptr <Variable> const &property : properties)
    {
      std::vector <std::unique_ptr <Uniform>> property_uniforms = property->get_properties (uniform_prefix);
      uniforms.insert (uniforms.end(),
          std::make_move_iterator (property_uniforms.begin()),
          std::make_move_iterator (property_uniforms.end()));
    }

    return uniforms;
  }
};

class Parser
{
private:
  const std::filesystem::path path;

  bool valid = true;
  std::vector <std::string> errors;

  std::vector<Token>::const_iterator iterator;
  std::vector<Token>::const_iterator end_iterator;
  std::map<std::string, std::unique_ptr <Variable>> uniforms;
  std::map<std::string, std::vector <std::unique_ptr <Variable>>> struct_types;
  std::vector <std::filesystem::path> included_files;

  std::string glsl_shader_code = "";
  std::string vertex_shader_code = "";
  std::string fragment_shader_code = "";

  void process (std::filesystem::path const &path, bool should_be_implementation);
  void parse (bool should_be_implementation);

  bool expect_token (Token_Type expected_token, std::string const &error_message);
  bool peek_token (Token_Type check_token) const;
  std::vector <Token>::const_iterator find_next_token (Token_Type const type) const;
  std::string skip_whitespace (std::vector<Token>::const_iterator &iterator) const;

  std::unique_ptr <Variable> parse_variable ();
  template <typename T> std::unique_ptr<Variable> create_variable (std::string const &name, unsigned int size);
  template <typename T> std::vector<T> parse_assignment_values (unsigned int size);
  template <typename T> T parse_value ();
  int find_number (bool is_float);

  void include_file ();
  void include_vertex_shader ();
  void register_struct ();
  void register_uniform ();
  std::tuple<Variable::Type, unsigned int, std::string> determine_variable_type();

  void register_error (std::string const &message);

  bool is_vertex_shader(std::filesystem::path const &path) const;
  bool is_fragment_shader(std::filesystem::path const &path) const;

  Parser (
    std::filesystem::path const &path,
    bool is_implementation,
    std::vector <std::filesystem::path> const &included_files = {},
    std::map<std::string, std::unique_ptr <Variable>> const &p_uniforms = {},
    std::map<std::string, std::vector <std::unique_ptr <Variable>>> const &p_struct_types = {});

public:
  std::string get_shader_code() const;
  std::string get_vertex_shader_code() const;
  std::string get_fragment_shader_code() const;

  bool is_valid () const;
  std::vector <std::string> get_errors () const;
  std::vector <std::unique_ptr <Uniform>> get_uniforms () const;
  Parser (std::filesystem::path const &path);
};

}
