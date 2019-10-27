#include "file_loader.hpp"

namespace fs = std::filesystem;

namespace
{

fs::path glsl_path;

std::string form_error_message (fs::path const &path)
{
  std::string filename = path.filename().string();
  return filename + " was not found. Tried searching at: " + path.string();
}

}

void io::init (fs::path const &path)
{
  glsl_path = path;
}

fs::path io::get_glsl_path()
{
  return glsl_path;
}

io::file_query <std::string> io::load_file (fs::path const &filepath)
{
  if (!(fs::exists(filepath) && filepath.has_filename()))
    return { false, form_error_message (filepath), ""};

  std::ifstream file(filepath.string());
  std::string contents((std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());
  file.close();
  return { true, "", contents };
}

std::vector <io::file_query <fs::path>> io::load_recursive (
    std::vector <fs::path> const &directories,
    std::string const &extension
) {
  std::vector <io::file_query <fs::path>> files;
  for (fs::path const &directory : directories)
  {
    if (fs::exists(directory))
    {
      for (fs::path const &path : fs::recursive_directory_iterator (directory))
      {
        if (!path.has_extension()) continue;
        if (path.extension().string().find(extension) != std::string::npos)
          files.push_back ({ true, "", path });
      }
    }

    else
      files.push_back ({ false, form_error_message (directory), "" });

  }

  return files;
}
