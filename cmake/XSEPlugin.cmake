option(BUILD_DEBUG "Enable debugging options" OFF)

option(BUILD_SKYRIMSE "Build for Skyrim pre-AE" OFF)
option(BUILD_SKYRIMAE "Build for Skyrim post-AE" OFF)
option(BUILD_SKYRIMVR "Build for Skyrim VR" OFF)
option(BUILD_FALLOUT4 "Build for Fallout 4" OFF)

if(BUILD_SKYRIMSE)
	add_compile_definitions(SKYRIMSE)
	set(CommonLibName "CommonLibSSE")
	set(CommonLibPath "external/CommonLibSSE")
	set(GamePath ${SkyrimVRPath})
	set(GameVersion "Skyrim VR")
elseif(BUILD_SKYRIMAE)
	add_compile_definitions(SKYRIMAE)
	set(CommonLibPath "external/CommonLibAE")
	set(CommonLibName "CommonLibSSE")
	set(GamePath ${SkyrimVRPath})
	set(GameVersion "Skyrim VR")
elseif(BUILD_SKYRIMVR)
	add_compile_definitions(SKYRIMVR)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	set(CommonLibName "CommonLibVR")
	set(CommonLibPath "external/CommonLibVR")
	set(GamePath ${SkyrimVRPath})
	set(GameVersion "Skyrim VR")
elseif(BUILD_FALLOUT4)
	add_compile_definitions(FALLOUT4)
	set(CommonLibPath "CommonLibF4/CommonLibF4")
	set(CommonLibName "external/CommonLibF4")
	set(GamePath ${SkyrimVRPath})
	set(GameVersion "Fallout 4")
else()
message(
FATAL_ERROR
	"A game must be selected."
)
endif()

if (NOT BUILD_DEBUG)
	add_compile_definitions(NDEBUG)
endif()

get_filename_component(
	Skyrim64Path
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim Special Edition;installed path]"
	ABSOLUTE CACHE
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${Skyrim64Path}/Data" CACHE PATH
		"Install path prefix (e.g. Skyrim Data directory or Mod Organizer virtual directory)."
		FORCE)
endif()

add_library("${PROJECT_NAME}" SHARED)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_23
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(AddCXXFiles)
add_cxx_files("${PROJECT_NAME}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Plugin.h.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/Plugin.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/version.rc
	@ONLY
)

target_sources(
	"${PROJECT_NAME}"
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/cmake/Plugin.h
		${CMAKE_CURRENT_BINARY_DIR}/cmake/version.rc
		.clang-format
		.editorconfig
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		include/PCH.h
)

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/cmake
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (CMAKE_GENERATOR MATCHES "Visual Studio")
	option(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD "Include INSTALL target to default build." OFF)
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC->"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

add_subdirectory(${CommonLibPath} ${CommonLibName} EXCLUDE_FROM_ALL)

find_package(spdlog CONFIG REQUIRED)