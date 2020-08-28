#
# boost_style_naming.cmake
#
# 2020-6-22 By GuoJH

# 平台工具集
function(get_toolset toolset)
	# https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
	
	set(_ToolSet "vc142")
	if(MSVC_VERSION LESS 1300)
		set(_ToolSet "vc6")
	elseif(MSVC_VERSION LESS 1310)
		set(_ToolSet "vc70")
	elseif(MSVC_VERSION LESS 1400)
		set(_ToolSet "vc71")
	elseif(MSVC_VERSION LESS 1500)
		set(_ToolSet "vc80")
	elseif(MSVC_VERSION LESS 1600)
		set(_ToolSet "vc90")
	elseif(MSVC_VERSION LESS 1700)
		set(_ToolSet "vc100")
	elseif(MSVC_VERSION LESS 1800)
		set(_ToolSet "vc110")
	elseif(MSVC_VERSION LESS 1900)
		set(_ToolSet "vc120")
	elseif(MSVC_VERSION LESS 1910)
		set(_ToolSet "vc140")
	elseif(MSVC_VERSION LESS 1920)
		set(_ToolSet "vc141")
	else()
		set(_ToolSet "vc142")
	endif()

	set(${toolset} ${_ToolSet} PARENT_SCOPE)

	# message(STATUS ">- MSVC_VERSION=${MSVC_VERSION}")
	# message(STATUS ">- _ToolSet=${_ToolSet}")
endfunction()

# 运行时
function(get_runtime c_or_cxx_flags is_mt)
	string(FIND ${c_or_cxx_flags} "MD" md_pos)
	string(FIND ${c_or_cxx_flags} "MT" mt_pos)

	# message("md_pos: ${md_pos} ${c_or_cxx_flags}\n"
	#		  "mt_pos: ${mt_pos}")

	if(${mt_pos} EQUAL -1)
		set(${is_mt} OFF PARENT_SCOPE)
	elseif(${md_pos} EQUAL -1)
		set(${is_mt} ON PARENT_SCOPE)
	elseif(${md_pos} GREATER ${mt_pos})
		set(${is_mt} OFF PARENT_SCOPE)
	else()
		set(${is_mt} ON PARENT_SCOPE)
	endif()
endfunction()

# 架构标检
function(get_architecture_tag archtag)
	#  -x86     Architecture and address model tag
	#           First character is the architecture, then word-size, either 32 or 64
	#           Only used in 'versioned' layout, added in Boost 1.66.0
	set(_ArchTag "")
	# {CMAKE_CXX_COMPILER_ARCHITECTURE_ID} is not currently set for all compilers
	if(NOT "x${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "x")
	  string(APPEND _ArchTag "-")
	  # This needs to be kept in-sync with the section of CMakePlatformId.h.in
	  # inside 'defined(_WIN32) && defined(_MSC_VER)'
	  if(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "IA64")
		string(APPEND _ArchTag "i")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "X86"
				OR ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "x64")
		string(APPEND _ArchTag "x")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} MATCHES "^ARM")
		string(APPEND _ArchTag "a")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "MIPS")
		string(APPEND _ArchTag "m")
	  endif()

	  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		string(APPEND _ArchTag "64")
	  else()
		string(APPEND _ArchTag "32")
	  endif()
	endif()

	set(${archtag} ${_ArchTag} PARENT_SCOPE)

	# message(STATUS ">- _ArchTag=${_ArchTag}")
	# message(STATUS ">- CMAKE_CXX_COMPILER_ARCHITECTURE_ID=${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endfunction()

function(set_boost_style_naming tgt)
	
	get_target_property(_TARGE_TYPE ${tgt} TYPE)

	if(STATIC_LIBRARY MATCHES ${_TARGE_TYPE})
		set(CONFIG_LIB_PREFIX "lib")
	elseif(SHARED_LIBRARY MATCHES ${_TARGE_TYPE})
		set(CONFIG_LIB_PREFIX "")
	else()
		set(CONFIG_LIB_PREFIX "not-library")
	endif()

	if(NOT CONFIG_LIB_PREFIX MATCHES "not-library")
		get_toolset(CONFIG_LIB_TOOLSET)
		get_architecture_tag(CONFIG_ARCHITECTURE_TAG)

		# message(STATUS ">- CONFIG_LIB_TOOLSET=${CONFIG_LIB_TOOLSET}")
		# message(STATUS ">- CONFIG_ARCHITECTURE_TAG=${CONFIG_ARCHITECTURE_TAG}")

		get_runtime(${CMAKE_CXX_FLAGS_DEBUG}   is_debug_mt)
		get_runtime(${CMAKE_CXX_FLAGS_RELEASE} is_release_mt)

		if(is_debug_mt)
			set(CONFIG_DEBUG_LIB_RT_OPT "-mt-sgd")
		else()
			set(CONFIG_DEBUG_LIB_RT_OPT "-mt-gd")
		endif()

		if(is_release_mt)
			set(CONFIG_RELEASE_LIB_RT_OPT "-mt-s")
		else()
			set(CONFIG_RELEASE_LIB_RT_OPT "-mt")
		endif()

		# libname-vc100-mt-sgd-x32.lib

		get_target_property(_TARGE_ORIGIN_NAME ${tgt} NAME)

		set(CONFIG_DEBUG_ARCHIVE_OUTPUT_NAME "${CONFIG_LIB_PREFIX}${_TARGE_ORIGIN_NAME}-${CONFIG_LIB_TOOLSET}${CONFIG_DEBUG_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}")
		set(CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME "${CONFIG_LIB_PREFIX}${_TARGE_ORIGIN_NAME}-${CONFIG_LIB_TOOLSET}${CONFIG_RELEASE_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}")

		message(STATUS ">- ${tgt} Target CONFIG_DEBUG_ARCHIVE_OUTPUT_NAME=${CONFIG_DEBUG_ARCHIVE_OUTPUT_NAME}")
		message(STATUS ">- ${tgt} Target CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME=${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")

		set_target_properties(${tgt} PROPERTIES ARCHIVE_OUTPUT_NAME_DEBUG "${CONFIG_DEBUG_ARCHIVE_OUTPUT_NAME}")
		set_target_properties(${tgt} PROPERTIES ARCHIVE_OUTPUT_NAME_RELEASE "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")
		set_target_properties(${tgt} PROPERTIES ARCHIVE_OUTPUT_NAME_MINSIZEREL "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")
		set_target_properties(${tgt} PROPERTIES ARCHIVE_OUTPUT_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")

		if(NOT CONFIG_LIB_PREFIX MATCHES "lib")
			set_target_properties(${tgt} PROPERTIES RUNTIME_OUTPUT_NAME_DEBUG "${CONFIG_DEBUG_ARCHIVE_OUTPUT_NAME}")
			set_target_properties(${tgt} PROPERTIES RUNTIME_OUTPUT_NAME_RELEASE "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")
			set_target_properties(${tgt} PROPERTIES RUNTIME_OUTPUT_NAME_MINSIZEREL "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")
			set_target_properties(${tgt} PROPERTIES RUNTIME_OUTPUT_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_ARCHIVE_OUTPUT_NAME}")
		endif()
	endif()
endfunction()

function(get_named_shared_libraries debuglib releaselib orgin)
	get_toolset(CONFIG_LIB_TOOLSET)
	get_architecture_tag(CONFIG_ARCHITECTURE_TAG)

	get_runtime(${CMAKE_CXX_FLAGS_DEBUG}   is_debug_mt)
	get_runtime(${CMAKE_CXX_FLAGS_RELEASE} is_release_mt)

	if(is_debug_mt)
		set(CONFIG_DEBUG_LIB_RT_OPT "-mt-sgd")
	else()
		set(CONFIG_DEBUG_LIB_RT_OPT "-mt-gd")
	endif()

	if(is_release_mt)
		set(CONFIG_RELEASE_LIB_RT_OPT "-mt-s")
	else()
		set(CONFIG_RELEASE_LIB_RT_OPT "-mt")
	endif()

	if(WIN32)
		set(CONFIG_PALTFORM_EXTENSION ".lib")
	else()
		set(CONFIG_PALTFORM_EXTENSION ".a")
	endif()

	set(${debuglib} "${orgin}-${CONFIG_LIB_TOOLSET}${CONFIG_DEBUG_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}${CONFIG_PALTFORM_EXTENSION}" PARENT_SCOPE)
	set(${releaselib} "${orgin}-${CONFIG_LIB_TOOLSET}${CONFIG_RELEASE_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}${CONFIG_PALTFORM_EXTENSION}" PARENT_SCOPE)
endfunction()

function(get_named_static_libraries debuglib releaselib orgin)
	get_named_shared_libraries(_debuglib _releaselib ${orgin})
	set(${debuglib} "lib${_debuglib}" PARENT_SCOPE)
	set(${releaselib} "lib${_releaselib}" PARENT_SCOPE)
endfunction()

macro(create_named_static_import_target staticTarget orginName)
	get_named_static_libraries(_debuglib _releaselib ${orginName})
	add_library(${staticTarget} STATIC IMPORTED)
	set_target_properties(${staticTarget} PROPERTIES
	  IMPORTED_LOCATION_DEBUG "${_debuglib}"
	  IMPORTED_LOCATION_RELEASE "${_releaselib}"
	  IMPORTED_LOCATION_MINSIZEREL "${_releaselib}"
	  IMPORTED_LOCATION_RELWITHDEBINFO "${_releaselib}")

	get_target_property(_TARGE_TYPE ${staticTarget} TYPE)
	message("_debuglib=${_debuglib} _releaselib=${_releaselib} _TARGE_TYPE=${_TARGE_TYPE}")
endmacro()

macro(create_named_shared_import_target staticTarget orginName)
	get_named_shared_libraries(_debuglib _releaselib ${orginName})
	add_library(${staticTarget} SHARED IMPORTED)
	set_target_properties(${staticTarget} PROPERTIES
	  IMPORTED_LOCATION_DEBUG "${_debuglib}"
	  IMPORTED_LOCATION_RELEASE "${_releaselib}"
	  IMPORTED_LOCATION_MINSIZEREL "${_releaselib}"
	  IMPORTED_LOCATION_RELWITHDEBINFO "${_releaselib}")
endmacro()
