function (group_sources root_path sources)
	set (_sources ${sources} ${ARGN})
	foreach (source ${_sources})
		get_filename_component (group "${source}" DIRECTORY)
		string(REPLACE "${root_path}" "" group "${group}")
		string(REPLACE "/" "\\" group "${group}")
		string(REPLACE "/" "\\" source "${source}")
		source_group("${group}" FILES "${source}" NEWLINE_STYLE LF)
	endforeach()
endfunction()
