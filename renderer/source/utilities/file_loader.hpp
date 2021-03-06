#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace io
{

template <typename T>
struct file_query
{
	bool        exists;
	std::string error;
	T           contents;
};

file_query<std::string> load_file (std::filesystem::path const& filepath);
std::vector<file_query<std::filesystem::path>> load_recursive (
	std::vector<std::filesystem::path> const& directories,
	std::string const&                        extension);
} // namespace io
