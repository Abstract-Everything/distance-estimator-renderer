function (header_directories directories header_paths)
	set (_header_paths ${header_paths} ${ARGN})
	set (dirs "")
	foreach(path ${_header_paths})
		get_filename_component (directory "${path}" PATH)
		list (APPEND dirs "${directory}")
	endforeach()
	list (REMOVE_DUPLICATES dirs)
	set (${directories} ${dirs} PARENT_SCOPE)
endfunction()
