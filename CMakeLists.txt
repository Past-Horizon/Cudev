cmake_minimum_required (VERSION 3.12)

file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/CudevVersion.txt" CUDEV_VERSION LIMIT_COUNT 1)
if (NOT CUDEV_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
	message (FATAL_ERROR "CudevVersion.txt must contain a semantic version in MAJOR.MINOR.PATCH format")
endif()

project ("Cudev" VERSION "${CUDEV_VERSION}" LANGUAGES CXX)

include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/CudevSources.cmake")

add_library (Cudev ${CUDEV_SOURCES})
target_include_directories (Cudev PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_property (TARGET Cudev PROPERTY CXX_STANDARD 20)
set_property (TARGET Cudev PROPERTY CXX_STANDARD_REQUIRED ON)

if (WIN32)
	set (CUDEV_PLATFORM "Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set (CUDEV_PLATFORM "Linux")
else()
	set (CUDEV_PLATFORM "${CMAKE_SYSTEM_NAME}")
endif()

set_target_properties (Cudev PROPERTIES
	OUTPUT_NAME "Cudev.${CUDEV_PLATFORM}.$<CONFIG>.${CUDEV_VERSION}"
)

include (CTest)
if (BUILD_TESTING)
	if (DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
		list (PREPEND CMAKE_PREFIX_PATH
			"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}"
		)
	endif()

	find_package (GTest CONFIG REQUIRED)

	add_executable (CudevTests
		tests/CoreTests.cpp
		tests/Utf8Tests.cpp
		tests/Utf16Tests.cpp
		tests/Utf32Tests.cpp
	)

	target_link_libraries (CudevTests
		PRIVATE
			Cudev
			GTest::gtest_main
	)

	include (GoogleTest)
	gtest_discover_tests (CudevTests)
endif()

