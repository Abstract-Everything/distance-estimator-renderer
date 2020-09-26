#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace renderer
{

class Uniform
{
public:
	virtual ~Uniform() = default;

	virtual std::string get_name() const = 0;

protected:
	Uniform (std::string const& p_name) : name (p_name) {}

	std::string name;
};

template <typename T>
class Typed_Uniform : public Uniform
{
public:
	std::string get_name() const override
	{
		return this->name;
	}

	std::vector<T> get_values() const
	{
		return values;
	}

	Typed_Uniform (std::string const& p_name, std::vector<T> const& p_values)
		: Uniform (p_name)
		, values (p_values)
	{
	}

private:
	std::vector<T> values{};
};
} // namespace renderer
