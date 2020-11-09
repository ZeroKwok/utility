#
# library_regular_naming.cmake
#
# 2020-6-22 By GuoJH

# ƽ̨���߼�
function(get_toolset toolset)
	# https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
	
	set(MSToolSet "vc142")
	if(MSVC_VERSION LESS 1300)
		set(MSToolSet "vc6")
	elseif(MSVC_VERSION LESS 1310)
		set(MSToolSet "vc70")
	elseif(MSVC_VERSION LESS 1400)
		set(MSToolSet "vc71")
	elseif(MSVC_VERSION LESS 1500)
		set(MSToolSet "vc80")
	elseif(MSVC_VERSION LESS 1600)
		set(MSToolSet "vc90")
	elseif(MSVC_VERSION LESS 1700)
		set(MSToolSet "vc100")
	elseif(MSVC_VERSION LESS 1800)
		set(MSToolSet "vc110")
	elseif(MSVC_VERSION LESS 1900)
		set(MSToolSet "vc120")
	elseif(MSVC_VERSION LESS 1910)
		set(MSToolSet "vc140")
	elseif(MSVC_VERSION LESS 1920)
		set(MSToolSet "vc141")
	else()
		set(MSToolSet "vc142")
	endif()

	set(${toolset} ${MSToolSet} PARENT_SCOPE)

	# message(STATUS ">- MSVC_VERSION=${MSVC_VERSION}")
	# message(STATUS ">- MSToolSet=${MSToolSet}")
endfunction()

# ����ʱ
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

# �ܹ����
function(get_architecture_tag arch_tag)
	#  -x86     Architecture and address model tag
	#           First character is the architecture, then word-size, either 32 or 64
	#           Only used in 'versioned' layout, added in Boost 1.66.0
	set(ARCH_TAG "")
	# {CMAKE_CXX_COMPILER_ARCHITECTURE_ID} is not currently set for all compilers
	if(NOT "x${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "x")
	  string(APPEND ARCH_TAG "-")
	  # This needs to be kept in-sync with the section of CMakePlatformId.h.in
	  # inside 'defined(_WIN32) && defined(_MSC_VER)'
	  if(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "IA64")
		string(APPEND ARCH_TAG "i")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "X86"
				OR ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "x64")
		string(APPEND ARCH_TAG "x")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} MATCHES "^ARM")
		string(APPEND ARCH_TAG "a")
	  elseif(${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} STREQUAL "MIPS")
		string(APPEND ARCH_TAG "m")
	  endif()

	  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		string(APPEND ARCH_TAG "64")
	  else()
		string(APPEND ARCH_TAG "32")
	  endif()
	endif()

	set(${arch_tag} ${ARCH_TAG} PARENT_SCOPE)
	# message(STATUS ">- ARCH_TAG=${ARCH_TAG}")
	# message(STATUS ">- CMAKE_CXX_COMPILER_ARCHITECTURE_ID=${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endfunction()

function(library_regular_naming name)
	get_target_property(LIBRARY_TARGE_TYPE ${name} TYPE)
	if(STATIC_LIBRARY MATCHES ${LIBRARY_TARGE_TYPE})
		set(CONFIG_LIB_PREFIX "lib")
	elseif(SHARED_LIBRARY MATCHES ${LIBRARY_TARGE_TYPE})
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
		get_target_property(_TARGE_ORIGIN_NAME ${name} NAME)
		set(CONFIG_DEBUG_OUTPUT_NAME "${CONFIG_LIB_PREFIX}${_TARGE_ORIGIN_NAME}-${CONFIG_LIB_TOOLSET}${CONFIG_DEBUG_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}")
		set(CONFIG_RELEASE_OUTPUT_NAME "${CONFIG_LIB_PREFIX}${_TARGE_ORIGIN_NAME}-${CONFIG_LIB_TOOLSET}${CONFIG_RELEASE_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}")

		message(STATUS ">- ${name} Target CONFIG_DEBUG_OUTPUT_NAME=${CONFIG_DEBUG_OUTPUT_NAME}")
		message(STATUS ">- ${name} Target CONFIG_RELEASE_OUTPUT_NAME=${CONFIG_RELEASE_OUTPUT_NAME}")

		# static
		set_target_properties(${name} PROPERTIES 
			ARCHIVE_OUTPUT_NAME_DEBUG "${CONFIG_DEBUG_OUTPUT_NAME}"
			ARCHIVE_OUTPUT_NAME_RELEASE "${CONFIG_RELEASE_OUTPUT_NAME}"
			ARCHIVE_OUTPUT_NAME_MINSIZEREL "${CONFIG_RELEASE_OUTPUT_NAME}"
			ARCHIVE_OUTPUT_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_OUTPUT_NAME}")

		# shared
		if(NOT CONFIG_LIB_PREFIX MATCHES "lib")
			set_target_properties(${name} PROPERTIES 
			RUNTIME_OUTPUT_NAME_DEBUG "${CONFIG_DEBUG_OUTPUT_NAME}"
			RUNTIME_OUTPUT_NAME_RELEASE "${CONFIG_RELEASE_OUTPUT_NAME}"
			RUNTIME_OUTPUT_NAME_MINSIZEREL "${CONFIG_RELEASE_OUTPUT_NAME}"
			RUNTIME_OUTPUT_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_OUTPUT_NAME}")
		endif()

		# pdb
		set_target_properties(${name} PROPERTIES 
		  PDB_NAME_DEBUG "${CONFIG_DEBUG_OUTPUT_NAME}"
		  PDB_NAME_RELEASE "${CONFIG_RELEASE_OUTPUT_NAME}"
		  PDB_NAME_MINSIZEREL "${CONFIG_RELEASE_OUTPUT_NAME}"
		  PDB_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_OUTPUT_NAME}"
		  COMPILE_PDB_NAME_DEBUG "${CONFIG_DEBUG_OUTPUT_NAME}"
		  COMPILE_PDB_NAME_RELEASE "${CONFIG_RELEASE_OUTPUT_NAME}"
		  COMPILE_PDB_NAME_MINSIZERE "${CONFIG_RELEASE_OUTPUT_NAME}"
		  COMPILE_PDB_NAME_RELWITHDEBINFO "${CONFIG_RELEASE_OUTPUT_NAME}")
	endif()
endfunction()

function(library_regular_named_query library_debug library_release orgin)
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

	set(${library_debug} "${orgin}-${CONFIG_LIB_TOOLSET}${CONFIG_DEBUG_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}${CONFIG_PALTFORM_EXTENSION}" PARENT_SCOPE)
	set(${library_release} "${orgin}-${CONFIG_LIB_TOOLSET}${CONFIG_RELEASE_LIB_RT_OPT}${CONFIG_ARCHITECTURE_TAG}${CONFIG_PALTFORM_EXTENSION}" PARENT_SCOPE)
endfunction()

function(library_regular_named_query_for_static library_debug library_release orgin)
	library_regular_named_query(_debuglib _releaselib ${orgin})
	set(${library_debug} "lib${_debuglib}" PARENT_SCOPE)
	set(${library_release} "lib${_releaselib}" PARENT_SCOPE)
endfunction()

macro(create_named_static_import_target static_target orgin_name)
	library_regular_named_query_for_static(_debuglib _releaselib ${orgin_name})
	add_library(${static_target} STATIC IMPORTED)
	set_target_properties(${static_target} PROPERTIES
	  IMPORTED_LOCATION_DEBUG "${_debuglib}"
	  IMPORTED_LOCATION_RELEASE "${_releaselib}"
	  IMPORTED_LOCATION_MINSIZEREL "${_releaselib}"
	  IMPORTED_LOCATION_RELWITHDEBINFO "${_releaselib}")

	get_target_property(LIBRARY_TARGE_TYPE ${static_target} TYPE)
	message("_debuglib=${_debuglib} _releaselib=${_releaselib} LIBRARY_TARGE_TYPE=${LIBRARY_TARGE_TYPE}")
endmacro()

macro(create_named_shared_import_target static_target orgin_name)
	library_regular_named_query(_debuglib _releaselib ${orgin_name})
	add_library(${static_target} SHARED IMPORTED)
	set_target_properties(${static_target} PROPERTIES
	  IMPORTED_LOCATION_DEBUG "${_debuglib}"
	  IMPORTED_LOCATION_RELEASE "${_releaselib}"
	  IMPORTED_LOCATION_MINSIZEREL "${_releaselib}"
	  IMPORTED_LOCATION_RELWITHDEBINFO "${_releaselib}")
endmacro()
